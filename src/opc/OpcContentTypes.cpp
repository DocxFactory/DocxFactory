
#include "DocxFactory/opc/OpcPackage.h"
#include "DocxFactory/opc/OpcFunc.h"
#include "DocxFactory/opc/OpcPart.h"
#include "DocxFactory/opc/OpcContentTypes.h"
#include "DocxFactory/opc/PartNotFoundException.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/UnzipFile.h"

#include "DocxFactory/xml/XmlFunc.h"
#include "DocxFactory/xml/Utf8ToXMLCh.h"
#include "DocxFactory/xml/NodeNotFoundException.h"

#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include "xercesc/dom/DOM.hpp"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"

#include "boost/scoped_ptr.hpp"

using namespace DocxFactory;
using namespace std;



OpcContentTypes::OpcContentTypes( OpcPackage* p_package )

	// [Content_Types].xml is the only file in the package that is guaranteed to have the same file name
	: m_fullPath( "/[Content_Types].xml" )
{
	m_package	= p_package;
	m_changed	= false;
} // c'tor

OpcContentTypes::~OpcContentTypes()
{

} // d'tor



void OpcContentTypes::load()
{
	map<string, OpcPart*>::const_iterator	l_partIterator;
	OpcPart*								l_part;

	xercesc::DOMDocument*	l_doc;
	xercesc::DOMElement*	l_rootNode;
	xercesc::DOMNodeList*	l_nodeList;
	xercesc::DOMElement*	l_node;
	XMLSize_t				l_len;
	XMLSize_t				l_pos;

	string	l_contentType;
	string	l_partFullPath;
	string	l_ext;

	size_t l_bufSize;
	boost::scoped_ptr<byte> l_buf( m_package ->getUnzipFile() ->extractEntryToBuf(
		OpcFunc::opcToZipPath( m_fullPath ), l_bufSize ) );

	l_doc = XmlFunc::parseBufToDoc( m_package ->getDomParser(), l_buf.get(), l_bufSize );
	XmlFunc::unindentDoc( l_doc );

	l_rootNode	= l_doc ->getDocumentElement();
	l_nodeList	= l_rootNode ->getChildNodes();

	if ( l_rootNode ->getNodeType() != xercesc::DOMNode::ELEMENT_NODE
	  || !XmlFunc::XMLChCmp( l_rootNode ->getLocalName(),		_X( "Types" ) )
	  || !XmlFunc::XMLChCmp( l_rootNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/package/2006/content-types" ) ) )
		throw NodeNotFoundException( "Types", __FILE__, __LINE__ );

	l_len = l_nodeList ->getLength();
	for ( l_pos = 0; l_pos < l_len; ++l_pos )
	{
		l_node = ( xercesc::DOMElement* ) l_nodeList ->item( l_pos );

		if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
		  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "Override" ) )
		  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/package/2006/content-types" ) ) )
		{
			l_contentType	= XmlFunc::XMLChToUtf8( l_node ->getAttribute( _X( "ContentType" ) ) );
			l_partFullPath	= XmlFunc::XMLChToUtf8( l_node ->getAttribute( _X( "PartName" ) ) );

			l_partIterator	= m_package ->getPartsByFullPath() ->find( l_partFullPath );
			if ( l_partIterator == m_package ->getPartsByFullPath() ->end() )
				throw PartNotFoundException( l_partFullPath, __FILE__, __LINE__ );

			l_part = l_partIterator ->second;
			m_overridesByPart.insert( make_pair( l_part, l_contentType ) );
		}

		else 
		if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
		  || XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "Default" ) )
		  || XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/package/2006/content-types" ) ) )
		{
			l_contentType	= XmlFunc::XMLChToUtf8( l_node ->getAttribute( _X( "ContentType" ) ) );
			l_ext			= XmlFunc::XMLChToUtf8( l_node ->getAttribute( _X( "Extension" ) ) );
			l_ext			= StrFunc::lc( l_ext );

			m_defaultsByExt.insert( make_pair( l_ext, l_contentType ) );
		}
	}
} // load

void OpcContentTypes::save()
{
	ZipFile*						l_zipFile;
	map<string, string>::iterator	l_defaultIterator;
	map<OpcPart*, string>::iterator	l_overrideIterator;

	int			l_method;
	int			l_level;
	FileInfo*	l_fileInfo;
	size_t		l_bufSize;

	if ( m_changed )
	{
		l_zipFile = m_package ->getZipFile();
		l_zipFile ->openStream( OpcFunc::opcToZipPath( m_fullPath ) );

		( *l_zipFile ) << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
		( *l_zipFile ) << "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">";

		FOR_EACH( l_defaultIterator, &m_defaultsByExt )
		{
			( *l_zipFile ) << string( "<Default " ) 
				+ "Extension=\"" + l_defaultIterator ->first + "\" "
				+ "ContentType=\"" + l_defaultIterator ->second + "\"/>";
		}

		FOR_EACH( l_overrideIterator, &m_overridesByPart )
		{
			( *l_zipFile ) << string( "<Override " ) 
				+ "PartName=\"" + l_overrideIterator ->first ->getFullPath() + "\" "
				+ "ContentType=\"" + l_overrideIterator ->second + "\"/>";
		}

		( *l_zipFile ) << "</Types>";
		l_zipFile ->closeStream();
	}

	else
	{
		// copy raw from unzip to zip
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



void OpcContentTypes::insertDefault( const string& p_ext, const string& p_contentType )
{
	map<string, string>::iterator	l_defaultIterator;
	string							l_ext;

	l_ext = StrFunc::lc( p_ext );

	if ( l_ext.length() > 0
	  && l_ext[0] == '.' )
		 l_ext = l_ext.substr( 1 );

	l_defaultIterator = m_defaultsByExt.find( l_ext );
	if ( l_defaultIterator != m_defaultsByExt.end() )
	{
		if ( l_defaultIterator ->second != p_contentType )
		{
			l_defaultIterator ->second = p_contentType;
			m_changed = true;
		}
	}

	else
	{
		m_defaultsByExt.insert( make_pair( l_ext, p_contentType ) );
		m_changed = true;
	}
} // insertDefault

void OpcContentTypes::insertOverride( OpcPart* p_part, const string& p_contentType )
{
	map<OpcPart*, string>::iterator l_overrideIterator = m_overridesByPart.find( p_part );
	if ( l_overrideIterator != m_overridesByPart.end() )
	{
		if ( l_overrideIterator ->second != p_contentType )
		{
			l_overrideIterator ->second = p_contentType;
			m_changed = true;
		}
	}

	else
	{
		m_overridesByPart.insert( make_pair( p_part, p_contentType ) );
		m_changed = true;
	}
} // insertOverride

void OpcContentTypes::deleteDefault( const string& p_ext )
{
	map<string, string>::iterator	l_defaultIterator;
	string							l_ext;
	
	l_ext = StrFunc::lc( p_ext );

	if ( l_ext.length() > 0
	  && l_ext[0] == '.' )
		 l_ext = l_ext.substr( 1 );

	l_defaultIterator = m_defaultsByExt.find( l_ext );
	if ( l_defaultIterator != m_defaultsByExt.end() )
	{
		m_defaultsByExt.erase( l_defaultIterator );
		m_changed = true;
	}
} // deleteDefault

void OpcContentTypes::deleteOverride( OpcPart* p_part )
{
	map<OpcPart*, string>::iterator l_overrideIterator = m_overridesByPart.find( p_part );
	if ( l_overrideIterator != m_overridesByPart.end() )
	{
		m_overridesByPart.erase( l_overrideIterator );
		m_changed = true;
	}
} // deleteOverride



OpcPackage* OpcContentTypes::getPackage() const
{
	return m_package;
} // getPackage

const map<string, string>* OpcContentTypes::getDefaultsByExt() const
{
	return &m_defaultsByExt;
} // getDefaultsByExt

const map<OpcPart*, string>* OpcContentTypes::getOverridesByPart() const
{
	return &m_overridesByPart;
} // getOverridesByPart
