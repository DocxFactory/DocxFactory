
#include "DocxFactory/opc/OpcPackage.h"
#include "DocxFactory/opc/OpcPart.h"
#include "DocxFactory/opc/OpcRelationships.h"
#include "DocxFactory/opc/OpcContentTypes.h"
#include "DocxFactory/opc/OpcImageCache.h"
#include "DocxFactory/opc/OpcImageFile.h"
#include "DocxFactory/opc/OpcFunc.h"

#include "DocxFactory/xml/XmlFunc.h"
#include "DocxFactory/xml/Utf8ToXMLCh.h"

#include "DocxFactory/zip/UnzipFile.h"
#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/FileInfo.h"

#include "DocxFactory/os/OsFunc.h"
#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include "xercesc/dom/DOM.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"

#include "boost/scoped_ptr.hpp"

using namespace DocxFactory;
using namespace std;



OpcPart::OpcPart( OpcPackage* p_package, const string& p_fullPath )
{
	m_package		= p_package;
	m_fullPath		= p_fullPath;
	m_relationships	= new OpcRelationships( this );

	m_refCnt		= 0;
	m_doc			= NULL;
	m_imageFile		= NULL;
	m_changeStatus	= PART_NOT_CHANGED;
} // c'tor

OpcPart::~OpcPart()
{
	delete m_relationships;
} // d'tor



void OpcPart::loadDoc()
{
	if ( m_doc )
		return;

	size_t					l_bufSize;
	boost::scoped_ptr<byte> l_buf( m_package ->getUnzipFile() ->extractEntryToBuf( OpcFunc::opcToZipPath( m_fullPath ), l_bufSize ) );

	// load/parse the doc using the package dom parser to keep it in scope for the life of the package
	m_doc = XmlFunc::parseBufToDoc( m_package ->getDomParser(), l_buf.get(), l_bufSize );
	XmlFunc::unindentDoc( m_doc );
} // loadDoc

void OpcPart::save()
{
	size_t					l_bufSize;
	boost::scoped_ptr<byte> l_buf;

	int			l_method;
	int			l_level;
	FileInfo*	l_fileInfo;

	switch ( m_changeStatus )
	{
		case PART_NOT_CHANGED:

			l_buf.reset( m_package ->getUnzipFile() ->extractEntryToRaw(
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

			break;

		case PART_CHANGED_IN_DOM:

			l_buf.reset( XmlFunc::saveDocToBuf( m_doc, l_bufSize ) );

			m_package ->getZipFile() ->addEntryFromBuf(
				OpcFunc::opcToZipPath( m_fullPath ),
				l_buf.get(),
				l_bufSize );

			break;

		case PART_CHANGED_IN_IMAGE_FILE:

			m_imageFile ->save( m_fullPath );
			break;

		case PART_CHANGED_IN_BUFFER:

			m_package ->getZipFile() ->addEntryFromBuf(
				OpcFunc::opcToZipPath( m_fullPath ),
				( const byte* ) m_buf,
				m_bufSize );

			break;

		case PART_SAVED:
			break;
	}

	m_relationships ->save();
	m_changeStatus = PART_SAVED; // mark the part as saved so it will not be saved again
} // save



void OpcPart::updateRefCnt( int p_refCnt )
{
	m_refCnt += p_refCnt;

	if ( m_refCnt <= 0 )
		destroy();
} // updateRefCnt

void OpcPart::destroy()
{
	map<string, OpcRelationship*>::const_iterator	l_relationshipIterator;
	OpcRelationship*								l_relationship;
	OpcPart*										l_part;

	// only decrease the reference counter. the relationships will be deleted together with the part when it is deleted 
	// which would be both simpler and more efficient

	FOR_EACH( l_relationshipIterator, m_relationships ->getRelationshipsById() )
	{
		l_relationship = l_relationshipIterator ->second;
		if ( l_relationship ->getTargetMode() == OpcRelationship::INTERNAL_MODE )
		{
			l_part = l_relationship ->getTargetPart();
			l_part ->updateRefCnt( -1 );
		}
	}
	
	m_package ->getContentTypes() ->deleteOverride( this );
	m_package ->deletePart( this );
	
	// if the object was not dynamic, deleting the object would cause the program to crash
	// because the d'tor would have been called again when the object would go out of scope
	// but there's no risk of that happening since all parts are created dynamically and internally by the package.
	delete this;
} // destroy



void OpcPart::setDoc( xercesc::DOMDocument* p_doc )
{
	m_doc = p_doc;
} // setDom

void OpcPart::setImageFile( OpcImageFile* p_imageFile )
{
	m_imageFile = p_imageFile;
} // setImageFile

void OpcPart::setBuf( const char* p_buf, size_t p_bufSize )
{
	m_buf		= p_buf;
	m_bufSize	= p_bufSize;
} // setBuf

void OpcPart::setChangeStatus( PartChangeStatus p_changeStatus )
{
	m_changeStatus = p_changeStatus;
} // setChangeStatus



OpcImageFile* OpcPart::insertImageFile(
	OpcImageFile::TargetMode	p_targetMode,
	const string&				p_fileName,
	const string&				p_fileExt,
	const string&				p_fileUrl,
	string&						p_fileRId )
{
	OpcImageFile*		l_imageFile;
	OpcRelationship*	l_relationship;
	OpcPart*			l_part;

	l_imageFile = m_package ->getImageCache() ->insertImageFile( p_fileName, p_fileExt );

	if ( p_targetMode == OpcImageFile::EXTERNAL_MODE && l_imageFile ->isConverted() )
		 p_targetMode = OpcImageFile::INTERNAL_MODE;

	if ( p_targetMode == OpcImageFile::INTERNAL_MODE )
	{
		l_part			= m_package ->insertImageFilePart( l_imageFile );
		l_relationship	= m_relationships ->insertRelationship( "http://schemas.openxmlformats.org/officeDocument/2006/relationships/image", l_part );
		p_fileRId		= l_relationship ->getId();
	}

	else
	{
		l_relationship	= m_relationships ->insertRelationship( "http://schemas.openxmlformats.org/officeDocument/2006/relationships/image", p_fileUrl );
		p_fileRId		= l_relationship ->getId();
	}

	return l_imageFile;
} // insertImageFile



OpcPackage* OpcPart::getPackage() const
{
	return m_package;
} // getPackage

string OpcPart::getFullPath() const
{
	return m_fullPath;
} // getFullPath

OpcRelationships* OpcPart::getRelationships() const
{
	return m_relationships;
} // getRelationships

xercesc::DOMDocument* OpcPart::getDoc() const
{
	return m_doc;
} // getDom

OpcImageFile* OpcPart::getImageFile() const
{
	return m_imageFile;
} // getImageFile

const char* OpcPart::getBuf( size_t& p_bufSize ) const
{
	p_bufSize = m_bufSize;

	return m_buf;
} // getBuf

OpcPart::PartChangeStatus OpcPart::getChangeStatus() const
{
	return m_changeStatus;
} // getChangeStatus
