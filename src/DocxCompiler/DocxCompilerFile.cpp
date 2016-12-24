
#include "DocxFactory/DocxCompiler/DocxCompilerFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerStyleFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerSettingsFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItemFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItemGroup.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItem.h"
#include "DocxFactory/DocxCompiler/DocxCompilerBookmark.h"
#include "DocxFactory/DocxCompiler/DocxCompilerField.h"
#include "DocxFactory/DocxCompiler/DocxCompilerChartField.h"
#include "DocxFactory/DocxCompiler/DocxCompilerXmlString.h"
#include "DocxFactory/DocxCompiler/DocxCompilerId.h"

#include "DocxFactory/opc/OpcFunc.h"
#include "DocxFactory/opc/OpcPackage.h"
#include "DocxFactory/opc/OpcPart.h"
#include "DocxFactory/opc/OpcRelationships.h"
#include "DocxFactory/opc/OpcRelationship.h"
#include "DocxFactory/opc/OpcContentTypes.h"
#include "DocxFactory/opc/PartTypeNotFoundException.h"
#include "DocxFactory/opc/ContentTypeNotFoundException.h"

#include "DocxFactory/xml/XmlInitializer.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/UnzipFile.h"

#include "DocxFactory/os/OsFunc.h"
#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include "boost/scoped_ptr.hpp"

using namespace DocxFactory;
using namespace std;



DocxCompilerFile::DocxCompilerFile() : OpcPackage()
{
	XmlInitializer::getInstance();

	m_documentPart	= NULL;
	m_styleFile		= NULL;
	m_settingsFile	= NULL;
	m_altChunkDir	= "";
	m_chartDir		= "";
	m_hasToc		= false;

	m_headerSeq		= 0;
	m_footerSeq		= 0;
} // c'tor

DocxCompilerFile::~DocxCompilerFile()
{
	list<DocxCompilerItemFile*>::iterator	l_itemFileIterator;
	map<string, DocxCompilerId*>::iterator	l_idIterator;

	if ( m_styleFile )
		delete m_styleFile;

	if ( m_settingsFile )
		delete m_settingsFile;

	FOR_EACH( l_itemFileIterator, &m_itemFiles )
	{
		delete *l_itemFileIterator;
	}

	FOR_EACH( l_idIterator, &m_idsById )
	{
		delete l_idIterator ->second;
	}
} // d'tor



void DocxCompilerFile::compile( const string& p_srcFileName, const string& p_dstFileName )
{
	load( p_srcFileName );
	save( p_dstFileName );
} // compile

void DocxCompilerFile::load( const string& p_fileName )
{
	map<string, OpcRelationship*>::const_iterator	l_relationshipIterator;
	const map<string, OpcRelationship*>*			l_relationships;

	UnzipFile*	l_unzipFile	= getUnzipFile();
	string		l_altChunkName;

	l_unzipFile ->open( p_fileName );
	OpcPackage::load( p_fileName );

	l_relationships	= getRelationships() ->getRelationshipsById();
	FOR_EACH( l_relationshipIterator, l_relationships )
	{
		if ( l_relationshipIterator ->second ->getType() == "http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" )
			break;
	}

	if ( l_relationshipIterator == l_relationships ->end() )
		throw PartTypeNotFoundException( "http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument", __FILE__, __LINE__ );

	m_documentPart	= l_relationshipIterator ->second ->getTargetPart();
	m_altChunkDir	= OpcFunc::getSubPath( m_documentPart ->getFullPath(), OpcFunc::SUBPATH_DIR, OpcFunc::SUBPATH_DIR );
	m_chartDir		= OpcFunc::getSubPath( m_documentPart ->getFullPath(), OpcFunc::SUBPATH_DIR, OpcFunc::SUBPATH_DIR ) + "charts/";

	loadStyleFile();
	loadSettingsFile();
	loadItemFiles();
	loadBookmarks();
	loadItems();
	loadFields();
	loadStripes();
	loadXmlStrings();
} // load

void DocxCompilerFile::save( const string& p_fileName )
{
	ZipFile*	l_zipFile		= getZipFile();
	UnzipFile*	l_unzipFile		= getUnzipFile();
	string		l_templateFile	= l_unzipFile ->getFileName();

	l_zipFile ->open( p_fileName, ZipFile::ZIP_CREATE );
	OpcPackage::save( p_fileName );

	serialize				( l_zipFile );
	serializeSettingsFile	( l_zipFile );
	serializeItemFiles		( l_zipFile );
	serializeItemGroups		( l_zipFile );
	serializeItems			( l_zipFile );
	serializeFields			( l_zipFile );
	serializeIds			( l_zipFile );
	serializeXmlStrings		( l_zipFile );

	l_zipFile ->addEntryFromFile(
		l_templateFile,
		"#" + OsFunc::getSubPath( l_templateFile, OsFunc::SUBPATH_FILE, OsFunc::SUBPATH_EXT ),
		Z_DEFLATED,
		Z_NO_COMPRESSION );

	l_zipFile ->close();
	l_unzipFile ->close();
} // save



void DocxCompilerFile::loadStyleFile()
{
	m_styleFile = new DocxCompilerStyleFile( this );
	m_styleFile ->load();
} // loadStyleFile

void DocxCompilerFile::loadSettingsFile()
{
	m_settingsFile = new DocxCompilerSettingsFile( this );
	m_settingsFile ->load();
} // loadSettingsFile

void DocxCompilerFile::loadItemFiles()
{
	list<DocxCompilerItemFile*>::iterator			l_itemFileIterator;
	DocxCompilerItemFile*							l_itemFile = NULL;

	map<string, OpcRelationship*>::const_iterator	l_relationshipIterator;
	const map<string, OpcRelationship*>*			l_relationships;

	OpcPart*	l_part;
	string		l_relationshipType;
	
	try
	{
		l_itemFile = new DocxCompilerItemFile( this, m_documentPart, DocxCompilerItemFile::DOCUMENT );
		l_itemFile ->load();

		m_itemFiles.push_back( l_itemFile );
		l_itemFile = NULL;

		l_relationships = m_documentPart ->getRelationships() ->getRelationshipsById();
		FOR_EACH( l_relationshipIterator, l_relationships )
		{
			l_relationshipType = l_relationshipIterator ->second ->getType();

			if ( l_relationshipType == "http://schemas.openxmlformats.org/officeDocument/2006/relationships/header"
			  || l_relationshipType == "http://schemas.openxmlformats.org/officeDocument/2006/relationships/footer" )
			{
				l_part		= l_relationshipIterator ->second ->getTargetPart();

				l_itemFile	= new DocxCompilerItemFile( this, l_part, ( l_relationshipType == "http://schemas.openxmlformats.org/officeDocument/2006/relationships/header" ? DocxCompilerItemFile::HEADER : DocxCompilerItemFile::FOOTER ) );
				l_itemFile ->load();

				m_itemFiles.push_back( l_itemFile );
				l_itemFile = NULL;
			}
		}
	}

	catch ( ... )
	{
		if ( l_itemFile )
			delete l_itemFile;

		FOR_EACH( l_itemFileIterator, &m_itemFiles )
		{
			delete *l_itemFileIterator;
		}

		throw;
	}
} // loadItemFiles

void DocxCompilerFile::loadBookmarks()
{
	list<DocxCompilerItemFile*>::iterator l_itemFileIterator;
	FOR_EACH( l_itemFileIterator, &m_itemFiles )
	{
		( *l_itemFileIterator ) ->loadBookmarks();
	}
} // loadBookmarks

void DocxCompilerFile::loadItems()
{
	list<DocxCompilerItemFile*>::iterator l_itemFileIterator;
	FOR_EACH( l_itemFileIterator, &m_itemFiles )
	{
		( *l_itemFileIterator ) ->loadItems();
	}
} // loadItems

void DocxCompilerFile::loadFields()
{
	list<DocxCompilerItemFile*>::iterator l_itemFileIterator;
	FOR_EACH( l_itemFileIterator, &m_itemFiles )
	{
		( *l_itemFileIterator ) ->loadFields();
	}
} // loadFields

void DocxCompilerFile::loadStripes()
{
	list<DocxCompilerItemFile*>::iterator l_itemFileIterator;
	FOR_EACH( l_itemFileIterator, &m_itemFiles )
	{
		( *l_itemFileIterator ) ->loadStripes();
	}
} // loadStripes

void DocxCompilerFile::loadXmlStrings()
{
	list<DocxCompilerItemFile*>::iterator l_itemFileIterator;
	FOR_EACH( l_itemFileIterator, &m_itemFiles )
	{
		( *l_itemFileIterator ) ->loadXmlStrings();
	}
} // loadXmlStrings



void DocxCompilerFile::serialize( ZipFile* p_zipFile )
{
	list<DocxCompilerItemFile*>::iterator			l_itemFileIterator;
	list<DocxCompilerItemGroup*>::iterator			l_itemGroupIterator;
	map<string, DocxCompilerItem*>::iterator		l_itemIterator;
	multimap<string, DocxCompilerField*>::iterator	l_fieldIterator;
	list<DocxCompilerXmlString*>::iterator			l_xmlStringIterator;
	map<string, DocxCompilerId*>::iterator			l_idIterator;

	p_zipFile ->openStream(
		"#docx.bin",
		Z_DEFLATED,
		Z_NO_COMPRESSION );

	p_zipFile ->writeStr		( DOCX_TEMPLATE_VERSION );
	p_zipFile ->writePtr		( this );
	p_zipFile ->writeStr		( m_altChunkDir );
	p_zipFile ->writeStr		( m_chartDir );
	p_zipFile ->writeNum<uint8> ( m_hasToc );
	
	p_zipFile ->writeNum<uint32>( ( uint32 ) m_itemFiles.size() );
	FOR_EACH( l_itemFileIterator, &m_itemFiles )
	{
		p_zipFile ->writePtr( *l_itemFileIterator );
	}

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_itemGroups.size() );
	FOR_EACH( l_itemGroupIterator, &m_itemGroups )
	{
		p_zipFile ->writePtr( *l_itemGroupIterator );
	}

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_itemsByName.size() );
	FOR_EACH( l_itemIterator, &m_itemsByName )
	{
		p_zipFile ->writeStr( l_itemIterator ->first );
		p_zipFile ->writePtr( l_itemIterator ->second );
	}

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_fieldsByName.size() );
	FOR_EACH( l_fieldIterator, &m_fieldsByName )
	{
		p_zipFile ->writeStr( l_fieldIterator ->first );
		p_zipFile ->writePtr( l_fieldIterator ->second );
	}

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_headerFieldsByName.size() );
	FOR_EACH( l_fieldIterator, &m_headerFieldsByName )
	{
		p_zipFile ->writeStr( l_fieldIterator ->first );
		p_zipFile ->writePtr( l_fieldIterator ->second );
	}

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_xmlStrings.size() );
	FOR_EACH( l_xmlStringIterator, &m_xmlStrings )
	{
		p_zipFile ->writePtr( *l_xmlStringIterator );
	}

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_idsById.size() );
	FOR_EACH( l_idIterator, &m_idsById )
	{
		p_zipFile ->writePtr( l_idIterator -> second);
	}

	p_zipFile ->closeStream();
} // serialize

void DocxCompilerFile::serializeSettingsFile( ZipFile* p_zipFile )
{
	list<DocxCompilerItemFile*>::iterator l_itemFileIterator;

	p_zipFile ->openStream(
		"#settingsfile.bin",
		Z_DEFLATED,
		Z_NO_COMPRESSION );

	m_settingsFile ->serialize( p_zipFile );

	p_zipFile ->closeStream();
} // serializeSettingsFile

void DocxCompilerFile::serializeItemFiles( ZipFile* p_zipFile )
{
	list<DocxCompilerItemFile*>::iterator l_itemFileIterator;

	p_zipFile ->openStream(
		"#itemfiles.bin",
		Z_DEFLATED,
		Z_NO_COMPRESSION );

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_itemFiles.size() );
	FOR_EACH( l_itemFileIterator, &m_itemFiles )
	{
		( *l_itemFileIterator ) ->serialize( p_zipFile );
	}

	p_zipFile ->closeStream();
} // serializeItemFiles

void DocxCompilerFile::serializeItemGroups( ZipFile* p_zipFile )
{
	list<DocxCompilerItemGroup*>::iterator l_itemGroupIterator;

	p_zipFile ->openStream(
		"#itemgroups.bin",
		Z_DEFLATED,
		Z_NO_COMPRESSION );

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_itemGroups.size() );
	FOR_EACH( l_itemGroupIterator, &m_itemGroups )
	{
		( *l_itemGroupIterator ) ->serialize( p_zipFile );
	}

	p_zipFile ->closeStream();
} // serializeItemGroups

void DocxCompilerFile::serializeItems( ZipFile* p_zipFile )
{
	map<string, DocxCompilerItem*>::iterator l_itemIterator;

	p_zipFile ->openStream(
		"#items.bin",
		Z_DEFLATED,
		Z_NO_COMPRESSION );

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_itemsByName.size() );
	FOR_EACH( l_itemIterator, &m_itemsByName )
	{
		l_itemIterator ->second ->serialize( p_zipFile );
	}

	p_zipFile ->closeStream();
} // serializeItems

void DocxCompilerFile::serializeFields( ZipFile* p_zipFile )
{
	multimap<string, DocxCompilerField*>::iterator l_fieldIterator;

	p_zipFile ->openStream(
		"#fields.bin",
		Z_DEFLATED,
		Z_NO_COMPRESSION );

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_fieldsByName.size() );
	FOR_EACH( l_fieldIterator, &m_fieldsByName )
	{
		p_zipFile ->writeNum<int16>( l_fieldIterator ->second ->getType() );

		if ( l_fieldIterator ->second ->getType() == DocxCompilerField::TYPE_CHART )

		p_zipFile ->writeNum<int16>( ( ( DocxCompilerChartField* ) l_fieldIterator ->second ) ->getChartType() );

		l_fieldIterator ->second ->serialize( p_zipFile );
	}

	p_zipFile ->closeStream();
} // serializeFields

void DocxCompilerFile::serializeXmlStrings( ZipFile* p_zipFile )
{
	list<DocxCompilerXmlString*>::iterator l_xmlStringIterator;

	p_zipFile ->openStream(
		"#xmlstrings.bin",
		Z_DEFLATED,
		Z_NO_COMPRESSION );

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_xmlStrings.size() );
	FOR_EACH( l_xmlStringIterator, &m_xmlStrings )
	{
		( *l_xmlStringIterator ) ->serialize( p_zipFile );
	}

	p_zipFile ->closeStream();
} // serializeXmlStrings

void DocxCompilerFile::serializeIds( ZipFile* p_zipFile )
{
	map<string, DocxCompilerId*>::iterator l_idIterator;

	p_zipFile ->openStream(
		"#ids.bin",
		Z_DEFLATED,
		Z_NO_COMPRESSION );

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_idsById.size() );
	FOR_EACH( l_idIterator, &m_idsById )
	{
		l_idIterator ->second ->serialize( p_zipFile );
	}

	p_zipFile ->closeStream();
} // serializeIds



unsigned int DocxCompilerFile::nextHeaderSeq()
{
	return ++m_headerSeq;
} // nextHeaderSeq

unsigned int DocxCompilerFile::nextFooterSeq()
{
	return ++m_footerSeq;
} // nextFooterSeq



void DocxCompilerFile::insertBookmark( DocxCompilerBookmark* p_bookmark )
{
	map<string, DocxCompilerBookmark*>::iterator l_bookmarkIterator = m_bookmarksByName.find( p_bookmark ->getName() );

	if ( l_bookmarkIterator != m_bookmarksByName.end() )
	{
		if( l_bookmarkIterator ->second != p_bookmark )
			l_bookmarkIterator ->second = p_bookmark;
	}

	else
		m_bookmarksByName.insert( make_pair( p_bookmark ->getName(), p_bookmark ) );
} // insertBookmark

void DocxCompilerFile::insertItemGroup( DocxCompilerItemGroup* p_itemGroup )
{
	m_itemGroups.push_back( p_itemGroup );
} // insertItemGroup

void DocxCompilerFile::insertItem( DocxCompilerItem* p_item )
{
	map<string, DocxCompilerItem*>::iterator l_itemIterator = m_itemsByName.find( p_item ->getName() );

	if ( l_itemIterator != m_itemsByName.end() )
	{
		if( l_itemIterator ->second != p_item )
			l_itemIterator ->second = p_item;
	}

	else
		m_itemsByName.insert( make_pair( p_item ->getName(), p_item ) );
} // insertItem

void DocxCompilerFile::insertField( DocxCompilerField* p_field )
{
	DocxCompilerItemFile::ItemFileType	l_type	= p_field ->getItem() ->getItemFile() ->getType();
	string								l_name	= p_field ->getName();

	m_fieldsByName.insert( make_pair( l_name, p_field ) );

	if ( l_type == DocxCompilerItemFile::HEADER
	  || l_type == DocxCompilerItemFile::FOOTER )

		m_headerFieldsByName.insert( make_pair( l_name, p_field ) );
} // insertField

void DocxCompilerFile::insertXmlString( DocxCompilerXmlString* p_xmlString )
{
	m_xmlStrings.push_back( p_xmlString );
} // insertXmlString

DocxCompilerId* DocxCompilerFile::insertId( DocxCompilerItem* p_item, const string& p_tag, const string& p_id )
{
	string									l_str			= p_tag + "_" + p_id;
	map<string, DocxCompilerId*>::iterator	l_idIterator	= m_idsById.find( l_str );
	DocxCompilerId*							l_id;

	if( l_idIterator != m_idsById.end() )
		l_id = l_idIterator ->second;

	else
	{
		l_id = new DocxCompilerId( p_item, p_tag, p_id );
		m_idsById.insert( make_pair( l_str, l_id ) );
	}

	return l_id;
} // insertId

void DocxCompilerFile::setToc( bool p_hasToc )
{
	m_hasToc = p_hasToc;
} // setToc



DocxCompilerStyleFile* DocxCompilerFile::getStyleFile() const
{
	return m_styleFile;
} // getStyleFile

DocxCompilerSettingsFile* DocxCompilerFile::getSettingsFile() const
{
	return m_settingsFile;
} // getSettingsFile

const map<string, DocxCompilerBookmark*>* DocxCompilerFile::getBookmarksByName() const
{
	return &m_bookmarksByName;
} // getBookmarksByName

const map<string, DocxCompilerItem*>* DocxCompilerFile::getItemsByName() const
{
	return &m_itemsByName;
} // getItemsByName

const multimap<string, DocxCompilerField*>* DocxCompilerFile::getFieldsByName() const
{
	return &m_fieldsByName;
} // getFieldsByName

const multimap<string, DocxCompilerField*>* DocxCompilerFile::getHeaderFieldsByName() const
{
	return &m_headerFieldsByName;
} // getHeaderFieldsByName

const map<string, DocxCompilerId*>*	DocxCompilerFile::getIdsById() const
{
	return &m_idsById;
} // getIds

bool DocxCompilerFile::hasToc() const
{
	return m_hasToc;
} // getToc
