
#include "DocxFactory/opc/OpcPackage.h"
#include "DocxFactory/opc/OpcPart.h"
#include "DocxFactory/opc/OpcRelationships.h"
#include "DocxFactory/opc/OpcRelationship.h"
#include "DocxFactory/opc/OpcFunc.h"
#include "DocxFactory/opc/PartNotFoundException.h"

#include "DocxFactory/xml/XmlFunc.h"
#include "DocxFactory/xml/Utf8ToXMLCh.h"
#include "DocxFactory/xml/NodeNotFoundException.h"

#include "DocxFactory/zip/UnzipFile.h"
#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/ZipFunc.h"

#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include "xercesc/dom/DOM.hpp"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"

#include "boost/scoped_ptr.hpp"

using namespace DocxFactory;
using namespace std;



OpcRelationships::OpcRelationships( OpcPackage* p_package )
{
	m_package		= p_package;
	m_part			= NULL;
	m_fullPath		= "/_rels/.rels";
	m_relativeDir	= "/";
	m_idSeq			= 0;
	m_changed		= false;
} // c'tor

OpcRelationships::OpcRelationships( OpcPart* p_part )
{
	string l_dir;
	string l_file;
	string l_ext;

	OpcFunc::breakPath(
		p_part ->getFullPath(),
		l_dir,
		l_file,
		l_ext );

	m_package		= p_part ->getPackage();
	m_part			= p_part;
	m_fullPath		= l_dir + "_rels/" + l_file + l_ext + ".rels";
	m_relativeDir	= l_dir;
	m_idSeq			= 0;
	m_changed		= false;
} // c'tor

OpcRelationships::~OpcRelationships()
{
	map<string, OpcRelationship*>::iterator l_relationshipIterator;
	FOR_EACH( l_relationshipIterator, &m_relationshipsById )
	{
		delete l_relationshipIterator ->second;
	}
} // d'tor



void OpcRelationships::load()
{
	OpcPart*								l_part;
	map<string, OpcPart*>::const_iterator	l_partIterator;
	OpcRelationship*						l_relationship;

	xercesc::DOMDocument*	l_doc;
	xercesc::DOMElement*	l_relationshipsNode;
	xercesc::DOMNodeList*	l_relationshipNodeList;
	xercesc::DOMElement*	l_relationshipNode;
	XMLSize_t				l_len;
	XMLSize_t				l_pos;
	
	string			l_id;
	string			l_type;
	string			l_targetPath;
	string			l_targetMode;
	unsigned int	l_idSeq;

	size_t l_bufSize;
	boost::scoped_ptr<byte> l_buf( m_package ->getUnzipFile() ->extractEntryToBuf(
		OpcFunc::opcToZipPath( m_fullPath ), l_bufSize ) );

	l_doc = XmlFunc::parseBufToDoc( m_package ->getDomParser(), l_buf.get(), l_bufSize );
	XmlFunc::unindentDoc( l_doc );

	l_relationshipsNode		= l_doc ->getDocumentElement();
	l_relationshipNodeList	= l_relationshipsNode ->getChildNodes();

	if ( l_relationshipsNode ->getNodeType() != xercesc::DOMNode::ELEMENT_NODE
	  || !XmlFunc::XMLChCmp( l_relationshipsNode ->getLocalName(),		_X( "Relationships" ) )
	  || !XmlFunc::XMLChCmp( l_relationshipsNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/package/2006/relationships" ) ) )
		throw NodeNotFoundException( "Relationships", __FILE__, __LINE__ );

	l_len = l_relationshipNodeList ->getLength();
	for ( l_pos = 0; l_pos < l_len; ++l_pos )
	{
		l_relationshipNode = ( xercesc::DOMElement* ) l_relationshipNodeList ->item( l_pos );

		if ( l_relationshipNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
		  && XmlFunc::XMLChCmp( l_relationshipNode ->getLocalName(),	_X( "Relationship" ) )
 		  && XmlFunc::XMLChCmp( l_relationshipNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/package/2006/relationships" ) ) )
		{
			l_id			= XmlFunc::XMLChToUtf8( l_relationshipNode ->getAttribute( _X( "Id" ) ) );
			l_type			= XmlFunc::XMLChToUtf8( l_relationshipNode ->getAttribute( _X( "Type" ) ) );
			l_targetPath	= XmlFunc::XMLChToUtf8( l_relationshipNode ->getAttribute( _X( "Target" ) ) );
			l_targetMode	= XmlFunc::XMLChToUtf8( l_relationshipNode ->getAttribute( _X( "TargetMode" ) ) );

			if ( StrFunc::lc( l_targetMode ) == "external" )
				l_relationship = new OpcRelationship( this, l_id, l_type, l_targetPath );

			else
			{
				l_targetPath	= OpcFunc::normalizePath( m_relativeDir + l_targetPath );
				l_partIterator	= m_package ->getPartsByFullPath() ->find( l_targetPath );

				if ( l_partIterator == m_package ->getPartsByFullPath() ->end() )
					throw PartNotFoundException( l_targetPath, __FILE__, __LINE__ );

				l_part			= l_partIterator ->second;
				l_part ->updateRefCnt( 1 );
				l_relationship	= new OpcRelationship( this, l_id, l_type, l_part );
			}

			m_relationshipsById.insert( make_pair( l_id, l_relationship ) );

			if ( l_id.length() > 3 
			  && l_id.substr( 0, 3 ) == "rId"
			  && StrFunc::isNum	( l_id.substr(3) ) )
			{
				l_idSeq = StrFunc::strToInt<unsigned int>( l_id.substr(3) );

				if ( l_idSeq > m_idSeq )
					 m_idSeq = l_idSeq;
			}
		}
	}
} // load

void OpcRelationships::save()
{
	map<string, OpcRelationship*>::iterator l_relationshipIterator;
	OpcRelationship*						l_relationship;
	ZipFile*								l_zipFile;

	size_t		l_bufSize;
	int			l_method;
	int			l_level;
	FileInfo*	l_fileInfo;

	if ( m_relationshipsById.empty() )
		return;

	if ( m_changed )
	{
		l_zipFile = m_package ->getZipFile();
		l_zipFile ->openStream( OpcFunc::opcToZipPath( m_fullPath ) );

		( *l_zipFile ) << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
		( *l_zipFile ) << "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">";

		FOR_EACH( l_relationshipIterator, &m_relationshipsById )
		{
			l_relationship = l_relationshipIterator ->second;
			if ( l_relationship ->getTargetMode() == OpcRelationship::INTERNAL_MODE )
			{
				( *l_zipFile ) << string( "<Relationship " ) 
					+ "Id=\""	+ l_relationship ->getId() + "\" "
					+ "Type=\"" + l_relationship ->getType() + "\" "
					+ "Target=\"" + OpcFunc::getRelativePath( m_relativeDir, l_relationship ->getTargetPart() ->getFullPath() ) + "\"/>";
					// convert target path back to relative path when saving the .rels file
			}

			else
			{ 
				( *l_zipFile ) << string( "<Relationship " ) 
					+ "Id=\"" + l_relationship ->getId() + "\" "
					+ "Type=\"" + l_relationship ->getType() + "\" "
					+ "Target=\"" + l_relationship ->getExternalPath() + "\" "
					+ "TargetMode=\"External\"/>";
			}
		}

		( *l_zipFile ) << "</Relationships>";
		l_zipFile ->closeStream();
	}

	else
	{
		boost::scoped_ptr<byte>	l_buf( m_package ->getUnzipFile() ->extractEntryToRaw(
			OpcFunc::opcToZipPath( m_fullPath ),
			l_method,
			l_level,
			l_fileInfo,
			l_bufSize ) );

		m_package ->getZipFile() ->addEntryFromRaw(
			OpcFunc::opcToZipPath( m_fullPath ),
			l_buf.get(),
			l_bufSize,
			l_method,
			l_level,
			l_fileInfo );
	}
} // save



// insert an internal relationship
OpcRelationship* OpcRelationships::insertRelationship(
	const string&	p_type,
	OpcPart*		p_part )
{
	map<string, OpcRelationship*>::iterator	l_relationshipIterator;
	OpcRelationship*						l_relationship;
	string									l_id;

	FOR_EACH( l_relationshipIterator, &m_relationshipsById )
	{
		if ( l_relationshipIterator ->second ->getType()		== p_type
		  && l_relationshipIterator ->second ->getTargetPart()	== p_part
		  && l_relationshipIterator ->second ->getTargetMode()	== OpcRelationship::INTERNAL_MODE )
			break;
	}

	if ( l_relationshipIterator == m_relationshipsById.end() )
	{
		++m_idSeq;
		l_id			= string( "rId" ) + StrFunc::intToStr( m_idSeq );
		l_relationship	= new OpcRelationship( this, l_id, p_type, p_part );

		m_relationshipsById.insert( make_pair( l_id, l_relationship ) );
		p_part ->updateRefCnt( 1 );

		m_changed = true;
	}

	else
		l_relationship = l_relationshipIterator ->second;

	return l_relationship;
} // insertRelationship

// insert an external relationship
OpcRelationship* OpcRelationships::insertRelationship(
	const string&	p_type,
	const string&	p_externalPath )
{
	map<string, OpcRelationship*>::iterator	l_relationshipIterator;
	OpcRelationship*						l_relationship;
	string									l_id;

	FOR_EACH( l_relationshipIterator, &m_relationshipsById )
	{
		if ( l_relationshipIterator ->second ->getType()			== p_type
		  && l_relationshipIterator ->second ->getExternalPath()	== p_externalPath
		  && l_relationshipIterator ->second ->getTargetMode()		== OpcRelationship::EXTERNAL_MODE )
			break;
	}

	if ( l_relationshipIterator == m_relationshipsById.end() )
	{
		++m_idSeq;
		l_id			= string( "rId" ) + StrFunc::intToStr( m_idSeq );
		l_relationship	= new OpcRelationship( this, l_id, p_type, p_externalPath );

		m_relationshipsById.insert( make_pair( l_id, l_relationship ) );
		m_changed = true;
	}

	else
		l_relationship = l_relationshipIterator ->second;

	return l_relationship;
} // insertRelationship

// insert raw relationship. can be used to recreate a relationships file with the same id's
OpcRelationship* OpcRelationships::insertRelationship(
	const string&				p_id,
	const string&				p_type,
	OpcPart*					p_part,
	const string&				p_externalPath,
	OpcRelationship::TargetMode	p_targetMode )
{
	OpcRelationship* l_relationship;

	if ( p_targetMode == OpcRelationship::INTERNAL_MODE )
	{
		l_relationship = new OpcRelationship( this, p_id, p_type, p_part );
		p_part ->updateRefCnt( 1 );

	}

	else
	{
		l_relationship = new OpcRelationship( this, p_id, p_type, p_externalPath );
	}

	m_relationshipsById.insert( make_pair( p_id, l_relationship ) );
	m_changed = true;

	return l_relationship;
} // insertRelationship

void OpcRelationships::deleteRelationship( OpcRelationship* p_relationship )
{
	if ( p_relationship ->getTargetMode() == OpcRelationship::INTERNAL_MODE )
		 p_relationship ->getTargetPart() ->updateRefCnt( -1 );

	m_relationshipsById.erase( p_relationship ->getId() );
	delete p_relationship;

	m_changed = true;
} // deleteRelationship



OpcPackage* OpcRelationships::getPackage() const
{
	return m_package;
} // getPackage

OpcPart* OpcRelationships::getPart() const
{
	return m_part;
} // getPart

const map<string, OpcRelationship*>* OpcRelationships::getRelationshipsById() const
{
	return &m_relationshipsById;
} // getRelationshipsById
