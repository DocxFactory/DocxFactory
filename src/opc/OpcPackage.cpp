
#include "DocxFactory/opc/OpcPackage.h"
#include "DocxFactory/opc/OpcPart.h"
#include "DocxFactory/opc/OpcRelationships.h"
#include "DocxFactory/opc/OpcRelationship.h"
#include "DocxFactory/opc/OpcContentTypes.h"
#include "DocxFactory/opc/OpcImageCache.h"
#include "DocxFactory/opc/OpcImageFile.h"
#include "DocxFactory/opc/OpcFunc.h"
#include "DocxFactory/opc/PartNotFoundException.h"

#include "DocxFactory/xml/XmlFunc.h"
#include "DocxFactory/xml/XmlInitializer.h"
#include "DocxFactory/xml/Utf8ToXMLCh.h"

#include "DocxFactory/zip/UnzipFile.h"
#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/ZipFunc.h"
#include "DocxFactory/zip/FileInfo.h"
#include "DocxFactory/os/OsFunc.h"

#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include "xercesc/dom/DOM.hpp"

using namespace DocxFactory;
using namespace std;



OpcPackage::OpcPackage()
{
	XmlInitializer::getInstance();
	xercesc::DOMImplementation* l_impl = xercesc::DOMImplementationRegistry::getDOMImplementation( _X( "ls" ) );

	m_relationships	= new OpcRelationships( this );
	m_contentTypes	= new OpcContentTypes( this );
	m_imageCache	= new OpcImageCache( this );
	m_unzipFile		= new UnzipFile();
	m_zipFile		= new ZipFile();
	m_domParser		= l_impl ->createLSParser( xercesc::DOMImplementation::MODE_SYNCHRONOUS, NULL );
} // c'tor

OpcPackage::~OpcPackage()
{
	map<string, OpcPart*>::iterator l_partIterator;
	OpcPart*						l_part;

	FOR_EACH( l_partIterator, &m_partsByFullPath )
	{
		l_part = l_partIterator ->second;

		// deleting parts will also delete their relationships
		delete l_part;
	}
	
	delete m_relationships;
	delete m_contentTypes;
	delete m_imageCache;

	// if the zip/unzip files are open their d'tor will close them first.
	delete m_unzipFile;
	delete m_zipFile;

	m_domParser ->release();
} // d'tor

void OpcPackage::load( const string& p_fileName )
{
	bool l_fileOpen = m_unzipFile ->isFileOpen();

	if ( !l_fileOpen )
		m_unzipFile ->open( p_fileName );

	loadParts();
	m_contentTypes ->load();
	m_imageCache ->load();

	if ( !l_fileOpen )
		m_unzipFile ->close();
} // load

void OpcPackage::loadParts()
{
	const map<string, FileInfo*>*			l_entryList;
	map<string, FileInfo*>::const_iterator	l_entryIterator;
	map<string, OpcPart*>::const_iterator	l_partIterator;

	OpcPart*			l_part;
	OpcRelationships*	l_relationships;

	string	l_entryPath;
	string	l_partPath;

	// 1. iterate through zip entries that are parts (not relationships (*.rels), not dirs (ending with /) and not the content types([Content_Types].xml))
	// and create their part objects.

	l_entryList = m_unzipFile ->getEntryList();
	FOR_EACH( l_entryIterator, l_entryList )
	{
		l_entryPath = l_entryIterator ->first;

		if ( l_entryPath.at( l_entryPath.length() - 1 )										!= '/'
		  && ZipFunc::getSubPath( l_entryPath, ZipFunc::SUBPATH_EXT, ZipFunc::SUBPATH_EXT )	!= ".rels"
		  && l_entryPath																	!= "[Content_Types].xml" )
		{
			l_partPath	= OpcFunc::normalizePath( OpcFunc::zipToOpcPath( l_entryPath ) );
			l_part		= new OpcPart( this, l_partPath );

			m_partsByFullPath.insert( make_pair( l_partPath, l_part ) );

			// the part objects are created but no xml is loaded/parsed, no content is loaded.
			// the xml is loaded selectively by the word or excel objects not in the opc object.

			// the relationships objects are created by the part objects when the part object is created.
			// the relationships object is empty, in other words without any relationships when it's created.
		}
	}



	// 2. after all the parts have been created iterate through all the zip entries that are relationships (*.rel)
	// using path manipulations to find their respective parts, get their relationship object and call the relationship load() function.

	// the reason the relationships are loaded after all the parts have been created
	// is because relationships store pointers to parts and relationships cannot point to parts if they haven't been created yet.

	// even though we can just iterate through all the parts and tell them to load their relationships
	// there are relationships (*.rel) for a small fraction of these parts and only calling the ones that have relationships is more efficient.

	FOR_EACH( l_entryIterator, l_entryList )
	{
		l_entryPath = l_entryIterator ->first;
		
		if ( l_entryPath																		!= "_rels/.rels" // not package relationships
		  && ZipFunc::getSubPath( l_entryPath, ZipFunc::SUBPATH_EXT, ZipFunc::SUBPATH_EXT )		== ".rels" )
		{
			l_partPath	= OpcFunc::normalizePath( '/' + l_entryPath );
			l_partPath	= OpcFunc::getSubPath( l_partPath, OpcFunc::SUBPATH_DIR, OpcFunc::SUBPATH_DIR );	// get "_rels" directory
			l_partPath	= l_partPath.substr( 0, l_partPath.length() - 1 );									// remove '/' from the end
			l_partPath	= OpcFunc::getSubPath( l_partPath, OpcFunc::SUBPATH_DIR, OpcFunc::SUBPATH_DIR );	// remove "_rels" directory
			l_partPath += ZipFunc::getSubPath( l_entryPath, ZipFunc::SUBPATH_FILE, ZipFunc::SUBPATH_FILE ); // add the file name without .rels

			l_partIterator = m_partsByFullPath.find( l_partPath );

			if ( l_partIterator == m_partsByFullPath.end() )
				throw PartNotFoundException( l_partPath, __FILE__, __LINE__ );

			l_part = l_partIterator ->second;

			l_relationships	= l_part ->getRelationships();
			l_relationships ->load();
		}
	}

	// package relationships create needs to be done separately, because it's not for any part

	m_relationships = new OpcRelationships( this );
	m_relationships ->load();



	// 3. after all the parts and relationships have been created, iterate through the package parts map
	// and call the updateRefCnt( 0 ) so that parts with no/0 reference count will be removed.

	l_partIterator = m_partsByFullPath.begin();
	while ( l_partIterator != m_partsByFullPath.end() )
	{
		l_part = l_partIterator ->second;
		l_partIterator++;

		// erasing an entry from a map that the iterator is standing on invalidates the iterator and will cause the program to crash
		// so updateRefCnt() is called, possibly erasing the entry from the map, after the iterator passes it.

		l_part ->updateRefCnt( 0 );
	}
} // loadParts

void OpcPackage::save( const string& p_fileName )
{
	bool l_fileOpen = m_zipFile ->isFileOpen();

	if ( !l_fileOpen )
		m_zipFile ->open( p_fileName, ZipFile::ZIP_CREATE );

	saveParts();
	m_contentTypes ->save();

	if ( !l_fileOpen )
		m_zipFile ->close();
} // save

void OpcPackage::saveParts()
{
	map<string, OpcPart*>::iterator l_partIterator;
	OpcPart*						l_part;
	
	FOR_EACH( l_partIterator, &m_partsByFullPath )
	{
		l_part = l_partIterator ->second;

		// saving the parts will also save their reationships
		l_part ->save();
	}

	// package relationships save needs to be called separately, because it's not for any part
	m_relationships ->save();
} // saveParts



OpcPart* OpcPackage::insertPart( const string& p_fullPath )
{
	map<string, OpcPart*>::iterator l_partIterator;
	OpcPart*						l_part;

	l_partIterator = m_partsByFullPath.find( p_fullPath );
	if ( l_partIterator == m_partsByFullPath.end() )
	{
		l_part = new OpcPart( this, p_fullPath );
		m_partsByFullPath.insert( make_pair( p_fullPath, l_part ) );
	}

	else
		l_part = l_partIterator ->second;

	return l_part;
	
	// when calling the insertPart() function, the setDoc() function and then the setChangeStatus() functions must be called
	// or the save() function will cause the program to crash.
} // insertPart

OpcPart* OpcPackage::insertImageFilePart( OpcImageFile* p_imageFile )
{
	map<OpcImageFile*, OpcPart*>::iterator	l_partIterator;
	OpcPart*								l_part;

	string	l_partFullPath;
	string	l_contentType;

	l_partIterator = m_partsByImageFile.find( p_imageFile );
	if ( l_partIterator == m_partsByImageFile.end() )
	{
		l_partFullPath = m_imageCache ->getFullPath() + "/image" + StrFunc::intToStr( m_imageCache ->nextSeq() ) + p_imageFile ->getExt();
		l_part = new OpcPart( this, l_partFullPath );

		m_partsByFullPath.insert( make_pair( l_partFullPath, l_part ) );
		m_partsByImageFile.insert( make_pair( p_imageFile, l_part ) );

		l_part ->setImageFile( p_imageFile );
		l_part ->setChangeStatus( OpcPart::PART_CHANGED_IN_IMAGE_FILE );
	}

	else
		l_part = l_partIterator ->second;

	return l_part;
} // insertImageFile

void OpcPackage::deletePart( OpcPart* p_part )
{
	map<string, OpcPart*>::iterator l_partIterator = m_partsByFullPath.find( p_part ->getFullPath() );

	if ( l_partIterator != m_partsByFullPath.end() )
		m_partsByFullPath.erase( p_part ->getFullPath() );
} // deletePart



const map<string, OpcPart*>* OpcPackage::getPartsByFullPath() const
{
	return &m_partsByFullPath;
} // getPartsByFullPath

OpcRelationships* OpcPackage::getRelationships() const
{
	return m_relationships;
} // getRelationships

OpcContentTypes* OpcPackage::getContentTypes() const
{
	return m_contentTypes;
} // getContentTypes

OpcImageCache* OpcPackage::getImageCache() const
{
	return m_imageCache;
} // getImageCache

UnzipFile* OpcPackage::getUnzipFile() const
{
	return m_unzipFile;
} // getUnzipFile

ZipFile* OpcPackage::getZipFile() const
{
	return m_zipFile;
} // getZipFile

xercesc::DOMLSParser* OpcPackage::getDomParser() const
{
	return m_domParser;
} // getDomParser
