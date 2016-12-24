
#include "DocxFactory/opc/OpcPackage.h"
#include "DocxFactory/opc/OpcPart.h"
#include "DocxFactory/opc/OpcRelationships.h"
#include "DocxFactory/opc/OpcRelationship.h"
#include "DocxFactory/opc/OpcImageCache.h"
#include "DocxFactory/opc/OpcImageFile.h"
#include "DocxFactory/opc/OpcContentTypes.h"
#include "DocxFactory/opc/OpcFunc.h"
#include "DocxFactory/opc/PartTypeNotFoundException.h"

#include "DocxFactory/os/OsFunc.h"
#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

using namespace DocxFactory;
using namespace std;



OpcImageCache::OpcImageCache( OpcPackage* p_package )
{
	m_package		= p_package;
	m_fullPath		= "";
	m_imageFileSeq	= 0;
} // c'tor

OpcImageCache::~OpcImageCache()
{
	map<string, OpcImageFile*>::iterator l_imageFileIterator;
	FOR_EACH( l_imageFileIterator, &m_imageFilesByFileName )
	{
		delete l_imageFileIterator ->second;
	}
} // d'tor



void OpcImageCache::load()
{
	map<string, OpcRelationship*>::const_iterator	l_relationshipIterator;
	map<string, OpcPart*>::const_iterator			l_partIterator;
	map<string, OpcPart*>::const_iterator			l_partEnd;

	const map<string, OpcRelationship*>*	l_relationshipsById;
	const map<string, OpcPart*>*			l_partsByFullPath;

	OpcPart*		l_part;
	string			l_file;
	unsigned int	l_imageFileSeq;

	// by convention media files are saved in a "media" dir in the main part directory (for example: /xl/media, /word/media); and
	// image files in the media dir are saved with "image" prefix followed by a sequence and image type (for example: image1.jpg).

	// 1. get the main part for setting the media dir

	l_relationshipsById	= m_package ->getRelationships() ->getRelationshipsById();
	l_partsByFullPath	= m_package ->getPartsByFullPath();

	FOR_EACH( l_relationshipIterator, l_relationshipsById )
	{
		if ( l_relationshipIterator ->second ->getType() == "http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" )
			break;
	}

	if ( l_relationshipIterator == l_relationshipsById ->end() )
		throw PartTypeNotFoundException( "http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument", __FILE__, __LINE__ );

	l_part = l_relationshipIterator ->second ->getTargetPart();
	m_fullPath = OpcFunc::getSubPath( l_part ->getFullPath(), OpcFunc::SUBPATH_DIR, OpcFunc::SUBPATH_DIR ) + "media";



	// 2. get image files max sequence in the media dir, to avoid duplicate file name conflicts

	l_partEnd		= l_partsByFullPath ->upper_bound( m_fullPath + "/image999999.zzz" );
	l_partIterator	= l_partsByFullPath ->lower_bound( m_fullPath );
	for (; l_partIterator != l_partEnd; ++l_partIterator )
	{
		l_file = OpcFunc::getSubPath( l_partIterator ->second ->getFullPath(), OpcFunc::SUBPATH_FILE, OpcFunc::SUBPATH_FILE );

		if ( l_file.length() > 5
		  && l_file.substr( 0, 5 ) == "image"
		  && StrFunc::isNum( l_file.substr( 5 ) ) )
		{
			l_imageFileSeq = StrFunc::strToInt<unsigned int>( l_file.substr(5) );

			if ( l_imageFileSeq > m_imageFileSeq )
				 m_imageFileSeq = l_imageFileSeq;
		}
	}
} // load

OpcImageFile* OpcImageCache::insertImageFile( const string& p_srcFileName, const string& p_fileExt )
{
	map<string, OpcImageFile*>::iterator	l_imageFileIterator;
	OpcImageFile*							l_imageFile;

	string	l_ext;
	string	l_contentType;
	
	l_imageFileIterator = m_imageFilesByFileName.find( p_srcFileName );
	if ( l_imageFileIterator == m_imageFilesByFileName.end() )
	{
		l_imageFile = new OpcImageFile( this, p_srcFileName, p_fileExt );
		m_imageFilesByFileName.insert( make_pair( p_srcFileName, l_imageFile ) );

		// the image type/ext might have been changed/converted.
		l_ext = l_imageFile ->getExt();

		// these are the only image type options
		// all other image types are converted to these image types.
			 if ( l_ext == ".jpeg" )	l_contentType = "image/jpeg";
		else if ( l_ext == ".png" )		l_contentType = "image/png";
		else if ( l_ext == ".tiff" )	l_contentType = "image/tiff";
		else if ( l_ext == ".gif" )		l_contentType = "image/gif";

		m_package ->getContentTypes() ->insertDefault( l_ext, l_contentType );
	}

	else
		l_imageFile = l_imageFileIterator ->second;

	return l_imageFile;
} // insertImageFile



OpcPackage* OpcImageCache::getPackage() const
{
	return m_package;
} // getPackage

string OpcImageCache::getFullPath() const
{
	return m_fullPath;
} // getFullPath

const map<string, OpcImageFile*>* OpcImageCache::getImageFilesByFileName() const
{
	return &m_imageFilesByFileName;
} // getImageFilesByFileName

unsigned int OpcImageCache::nextSeq()
{
	return ++m_imageFileSeq;
} // nextSeq
