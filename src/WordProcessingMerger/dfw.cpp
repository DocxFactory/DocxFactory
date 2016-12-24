
#define DOCXFACTORY_DLL_EXPORT
#include "DocxFactory/WordProcessingMerger/dfw.h"
#undef  DOCXFACTORY_DLL_EXPORT

#include "DocxFactory/WordProcessingMerger/InvalidItemNameException.h"
#include "DocxFactory/WordProcessingMerger/TemplateNotLoadedException.h"

#include "DocxFactory/DocxMerger/DocxMergerFile.h"
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/ConverterPrinter/ConverterPrinter.h"

#include "DocxFactory/xml/XmlFunc.h"
#include "DocxFactory/xml/DomException.h"

#include "DocxFactory/os/OsFunc.h"
#include "DocxFactory/os/UnsupportedFileTypeException.h"
#include "DocxFactory/os/FileNotFoundException.h"
#include "DocxFactory/os/DirNotFoundException.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/locale/InvalidFirstWeekDayException.h"
#include "DocxFactory/str/StrFunc.h"

#include "DocxFactory/util/DocxFactoryDefs.h"
#include "DocxFactory/util/DocxFactoryException.h"
#include "DocxFactory/util/GeneralException.h"
#include "DocxFactory/util/UnknownException.h"

#include "xercesc/dom/DOMException.hpp"

#include <exception>
#include <string>
#include <cstring>

using namespace DocxFactory;
using namespace std;



DocxMergerFile*	dfw_docxMergerFile;
string			dfw_templateFile;
unsigned char	dfw_updateTocMethod;

string			dfw_errorWhat;
string			dfw_errorWhere;
bool			dfw_errorFlag;
bool			dfw_initialized = false;


#ifdef __cplusplus
extern "C" {
#endif

void dfw_initialize()
{
	dfw_initialized			= true;

	dfw_docxMergerFile		= NULL;
	dfw_templateFile		= "";
	dfw_updateTocMethod		= 1;

	dfw_errorWhat			= "";
	dfw_errorWhere			= "";
	dfw_errorFlag			= false;

	ConverterPrinter::setUpdateToc( dfw_updateTocMethod > 0 );
} // dfw_initialize

void dfw_destroy()
{
	if ( dfw_docxMergerFile )
	{
		delete dfw_docxMergerFile;

		dfw_docxMergerFile	= NULL;
		dfw_templateFile	= "";
	}
} // dfw_destroy



void dfw_load( const char* p_fileName )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	string l_fileName( p_fileName );

	try
	{
		try
		{
			l_fileName = OsFunc::normalizePath( l_fileName );
			l_fileName = OsFunc::getFullPath( l_fileName );

			if ( OsFunc::getSubPath( l_fileName, OsFunc::SUBPATH_EXT, OsFunc::SUBPATH_EXT ) != ".dfw" )
				throw UnsupportedFileTypeException( l_fileName, ".dfw", __FILE__, __LINE__ );

			if ( !OsFunc::isFileExists( l_fileName ) )
				throw FileNotFoundException( l_fileName, __FILE__, __LINE__ );

			dfw_destroy();

			dfw_docxMergerFile = new DocxMergerFile();
			dfw_docxMergerFile ->load( l_fileName );

			dfw_templateFile = l_fileName;
		}
		
		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what( ), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		dfw_destroy();
	}
} // dfw_load

void dfw_save( const char* p_fileName )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	string l_fileName( p_fileName );
	string l_tempFile;
	string l_dir;
	string l_file;
	string l_ext;

	try
	{
		try
		{
			if ( !dfw_docxMergerFile )
				throw TemplateNotLoadedException( __FILE__, __LINE__ );

			if(!l_fileName.empty() ) 
			{
				l_fileName = OsFunc::normalizePath( l_fileName );
				l_fileName = OsFunc::getFullPath( l_fileName );

				/*
				if ( OsFunc::isRelativePath( l_fileName ) )

					l_fileName = OsFunc::normalizePath(
						  OsFunc::getSubPath( m_templateFile, OsFunc::SUBPATH_DIR, OsFunc::SUBPATH_DIR ) + "/"
						+ l_fileName );
				*/

				if ( OsFunc::isDirExists( l_fileName ) )

					l_fileName = OsFunc::normalizePath( l_fileName + "/"
						+ OsFunc::getSubPath( dfw_templateFile, OsFunc::SUBPATH_FILE, OsFunc::SUBPATH_FILE )
						+ ".docx" );

				else
				{
					OsFunc::breakPath(
						l_fileName,
						l_dir,
						l_file,
						l_ext );

					if ( !OsFunc::isDirExists( l_dir ) )
						throw DirNotFoundException( l_dir, __FILE__, __LINE__ );

					if( l_ext.empty() )
						l_ext = ".docx";

					l_fileName = l_dir + l_file + l_ext;
				}
			}

			else
			{
				l_ext = ".docx";

				l_fileName = OsFunc::getSubPath( dfw_templateFile, OsFunc::SUBPATH_DIR, OsFunc::SUBPATH_FILE ) + l_ext;
			}

			if ( OsFunc::isFileExists( l_fileName ) )
				 OsFunc::remove( l_fileName );

			if ( l_ext == ".docx" )
			{
				dfw_docxMergerFile ->save( l_fileName,
					dfw_docxMergerFile ->hasToc() && dfw_updateTocMethod == 1 );

				if( dfw_docxMergerFile ->hasToc() && dfw_updateTocMethod == 2 )
					ConverterPrinter::getInstance().saveAs( l_fileName, l_fileName );
			}

			else
			{
				l_tempFile = OsFunc::getTempFile( ".docx" );
				dfw_docxMergerFile ->save( l_tempFile );

				ConverterPrinter::getInstance().saveAs( l_tempFile, l_fileName );
				OsFunc::remove( l_tempFile );
			}

			dfw_destroy();
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what( ), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		if ( !l_tempFile.empty() && OsFunc::isFileExists( l_tempFile ) )
			OsFunc::remove( l_tempFile );

		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		dfw_destroy();
	}
} // dfw_save

void dfw_print( const char* p_printerName, unsigned short p_copyCnt )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	string l_tempFile;

	try
	{
		try
		{
			if ( !dfw_docxMergerFile )
				throw TemplateNotLoadedException( __FILE__, __LINE__ );

			l_tempFile = OsFunc::getTempFile( ".docx" );
			dfw_docxMergerFile ->save( l_tempFile );

			ConverterPrinter::getInstance().print( l_tempFile, p_printerName, p_copyCnt );
			OsFunc::remove( l_tempFile );

			dfw_destroy();
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what( ), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		if ( !l_tempFile.empty() && OsFunc::isFileExists( l_tempFile ) )
			OsFunc::remove( l_tempFile );

		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		dfw_destroy();
	}
} // dfw_print

void dfw_close()
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	dfw_destroy();
} // dfw_close

void dfw_merge( const char* p_data )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	try
	{
		try
		{
			if ( !dfw_docxMergerFile )
				throw TemplateNotLoadedException( __FILE__, __LINE__ );

			if ( p_data && p_data[0] )
			{
				// notepad unicode prefixes

				/*** not currently supported ***
				if ( StrFunc::begins( p_data, "\xFE\xFF\x00" ) )
				{
					setCodePage( "UTF-16" );
					m_docxMergerFile ->merge( StrFunc::trim( p_data.substr(3) ) );
				}

				else
				if ( StrFunc::begins( p_data, "\xFF\xFE" ) )
				{
					setCodePage( "UTF-16" );
					m_docxMergerFile ->merge( StrFunc::trim( p_data.substr(2) ) );
				}

				else
				***/

				if ( p_data[0] == '\xEF' 
				  && p_data[1] == '\xBB' 
				  && p_data[2] == '\xBF' )
				{
					dfw_setCodePage( "UTF-8" );
					dfw_docxMergerFile ->merge( StrFunc::trim( p_data + 3 ) );
				}

				else
				if ( p_data[0] == ' '
				  || p_data[0] == '\t'
				  || p_data[0] == '\n'
				  || p_data[0] == '\r' )
					dfw_docxMergerFile ->merge( StrFunc::trim( p_data ) );

				else
					dfw_docxMergerFile ->merge( p_data );
			}
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what( ), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		dfw_destroy();
	}
} // dfw_merge

void dfw_mergeW( const wchar_t*	p_data )
{
	dfw_merge( LocaleFunc::convertToUtf8( p_data ).c_str() );
} // dfw_mergeW

void dfw_setChartValueByStrStr( const char* p_itemName, const char* p_fieldName, const char* p_series, const char* p_category, double p_value )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	string l_itemName	= StrFunc::lc( StrFunc::trim( p_itemName ) );
	string l_fieldName	= StrFunc::lc( StrFunc::trim( p_fieldName ) );
	string l_series		= LocaleFunc::convertToUtf8( StrFunc::trim( p_series ) );
	string l_category	= LocaleFunc::convertToUtf8( StrFunc::trim( p_category ) );

	try
	{
		try
		{
			if ( !dfw_docxMergerFile )
				throw TemplateNotLoadedException( __FILE__, __LINE__ );

			if ( StrFunc::begins( l_itemName, "_" ) && l_itemName != "_header" && l_itemName != "_footer" )
				throw InvalidItemNameException( l_itemName, __FILE__, __LINE__ );

			dfw_docxMergerFile ->setChartValue( l_itemName, l_fieldName, l_series, l_category, p_value );
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		dfw_destroy();
	}
} // dfw_setChartValueByStrStr

void dfw_setChartValueByStrStrW( const wchar_t* p_itemName, const wchar_t* p_fieldName, const wchar_t* p_series, const wchar_t* p_category, double p_value )
{
	dfw_setChartValueByStrStr(
		LocaleFunc::convertToUtf8( p_itemName ).c_str(),
		LocaleFunc::convertToUtf8( p_fieldName ).c_str(),
		LocaleFunc::convertToUtf8( p_series ).c_str(),
		LocaleFunc::convertToUtf8( p_category ).c_str(),
		p_value );
} // dfw_setChartValueByStrStrW

void dfw_setChartValueByStrDouble( const char* p_itemName, const char* p_fieldName, const char* p_series, double p_category, double p_value )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	string l_itemName	= StrFunc::lc( StrFunc::trim( p_itemName ) );
	string l_fieldName	= StrFunc::lc( StrFunc::trim( p_fieldName ) );
	string l_series		= LocaleFunc::convertToUtf8( StrFunc::trim( p_series ) );

	try
	{
		try
		{
			if ( !dfw_docxMergerFile )
				throw TemplateNotLoadedException( __FILE__, __LINE__ );

			if ( StrFunc::begins( l_itemName, "_" ) && l_itemName != "_header" && l_itemName != "_footer" )
				throw InvalidItemNameException( l_itemName, __FILE__, __LINE__ );

			dfw_docxMergerFile ->setChartValue( l_itemName, l_fieldName, l_series, p_category, p_value );
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		dfw_destroy();
	}
} // dfw_setChartValueByStrDouble

void dfw_setChartValueByStrDoubleW( const wchar_t* p_itemName, const wchar_t* p_fieldName, const wchar_t* p_series, double p_category, double p_value )
{
	dfw_setChartValueByStrDouble(
		LocaleFunc::convertToUtf8( p_itemName ).c_str(),
		LocaleFunc::convertToUtf8( p_fieldName ).c_str(),
		LocaleFunc::convertToUtf8( p_series ).c_str(),
		p_category,
		p_value );
} // dfw_setChartValueByStrDoubleW

void dfw_setChartValueByDoubleDouble( const char* p_itemName, const char* p_fieldName, double p_series, double p_category, double p_value )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	string l_itemName	= StrFunc::lc( StrFunc::trim( p_itemName ) );
	string l_fieldName	= StrFunc::lc( StrFunc::trim( p_fieldName ) );

	try
	{
		try
		{
			if ( !dfw_docxMergerFile )
				throw TemplateNotLoadedException( __FILE__, __LINE__ );

			if ( StrFunc::begins( l_itemName, "_" ) && l_itemName != "_header" && l_itemName != "_footer" )
				throw InvalidItemNameException( l_itemName, __FILE__, __LINE__ );

			dfw_docxMergerFile ->setChartValue( l_itemName, l_fieldName, p_series, p_category, p_value );
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		dfw_destroy();
	}
} // dfw_setChartValueByDoubleDouble

void dfw_setClipboardValueByStr( const char* p_itemName, const char* p_fieldName, const char* p_value )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	if ( !p_value || !p_value[0] )
		 return;

	string l_itemName	= StrFunc::lc( StrFunc::trim( p_itemName ) );
	string l_fieldName	= StrFunc::lc( StrFunc::trim( p_fieldName ) );
	string l_value		= LocaleFunc::convertToUtf8( p_value );

	try
	{
		try
		{
			if ( !dfw_docxMergerFile )
				throw TemplateNotLoadedException( __FILE__, __LINE__ );

			if ( StrFunc::begins( l_itemName, "_" ) && l_itemName != "_header" && l_itemName != "_footer" )
				throw InvalidItemNameException( l_itemName, __FILE__, __LINE__ );

			dfw_docxMergerFile ->setClipboardValue( l_itemName, l_fieldName, l_value );
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		dfw_destroy();
	}
} // dfw_setClipboardValueByStr

void dfw_setClipboardValueByStrW( const wchar_t* p_itemName, const wchar_t* p_fieldName, const wchar_t* p_value )
{
	dfw_setClipboardValueByStr(
		LocaleFunc::convertToUtf8( p_itemName ).c_str(),
		LocaleFunc::convertToUtf8( p_fieldName ).c_str(),
		LocaleFunc::convertToUtf8( p_value ).c_str() );
} // dfw_setClipboardValueByStrW

void dfw_setClipboardValueByDouble( const char* p_itemName, const char* p_fieldName, double p_value )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	string l_itemName	= StrFunc::lc( StrFunc::trim( p_itemName ) );
	string l_fieldName	= StrFunc::lc( StrFunc::trim( p_fieldName ) );

	try
	{
		try
		{
			if ( !dfw_docxMergerFile )
				throw TemplateNotLoadedException( __FILE__, __LINE__ );

			if ( StrFunc::begins( l_itemName, "_" ) && l_itemName != "_header" && l_itemName != "_footer" )
				throw InvalidItemNameException( l_itemName, __FILE__, __LINE__ );

			dfw_docxMergerFile ->setClipboardValue( l_itemName, l_fieldName, p_value );
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		dfw_destroy();
	}
} // dfw_setClipboardValueByDouble

void dfw_paste( const char* p_itemName )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	string l_itemName = StrFunc::lc( StrFunc::trim( p_itemName ) );
	
	try
	{
		try
		{
			if ( !dfw_docxMergerFile )
				throw TemplateNotLoadedException( __FILE__, __LINE__ );

			if ( StrFunc::begins( l_itemName, "_" ) )
				throw InvalidItemNameException( l_itemName, __FILE__, __LINE__ );

			dfw_docxMergerFile ->paste( l_itemName );
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		dfw_destroy();
	}
} // dfw_paste



void dfw_setUpdateTocMethod( unsigned char p_method )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	dfw_updateTocMethod = p_method;

	ConverterPrinter::setUpdateToc( dfw_updateTocMethod > 0 );
} // dfw_setUpdateTocMethod

unsigned char dfw_getUpdateTocMethod()
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	return dfw_updateTocMethod;
} // dfw_getUpdateTocMethod



const char* dfw_getFields()
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	multimap<string, DocxMergerField*>::const_iterator	l_fieldIterator;
	const multimap<string, DocxMergerField*>*			l_fields;
	string												l_fieldNames;

	static char	l_retVal[3072];

	try
	{
		try
		{
			if ( !dfw_docxMergerFile )
				throw TemplateNotLoadedException( __FILE__, __LINE__ );

			l_fieldNames = "";

			l_fields = dfw_docxMergerFile ->getFieldsByName();
			FOR_EACH( l_fieldIterator, l_fields )
			{
				l_fieldNames
					+= ( l_fieldNames.empty() ? "" :  "," )
					+ l_fieldIterator ->first;
			}

			strncpy( l_retVal, l_fieldNames.c_str(), 3071 );
			l_retVal[3071] = '\0';
			return l_retVal;
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		dfw_destroy();

		return "?";
	}
} // dfw_getFields

const char* dfw_getItems()
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	map<string, DocxMergerItem*>::const_iterator	l_itemIterator;
	const map<string, DocxMergerItem*>*				l_items;
	string											l_itemNames;

	static char	l_retVal[3072];

	try
	{
		try
		{
			if ( !dfw_docxMergerFile )
				throw TemplateNotLoadedException( __FILE__, __LINE__ );

			l_itemNames = "";

			l_items = dfw_docxMergerFile ->getItemsByName();
			FOR_EACH( l_itemIterator, l_items )
			{
				l_itemNames
					+= ( l_itemNames.empty() ? "" : "," )
					+ l_itemIterator ->first;
			}

			strncpy( l_retVal, l_itemNames.c_str(), 3071 );
			l_retVal[3071] = '\0';
			return l_retVal;
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		dfw_destroy();

		return "?";
	}
} // dfw_getItems

const char* dfw_getItemParent( const char* p_itemName )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	map<string, DocxMergerItem*>::const_iterator	l_itemIterator;
	const map<string, DocxMergerItem*>*				l_items;
	DocxMergerItem*									l_item;

	list<DocxMergerItem*>::const_reverse_iterator	l_itemPathRIterator;
	const list<DocxMergerItem*>*					l_itemPath;
	string											l_itemName;

	static char l_retVal[256];

	try
	{
		try
		{
			l_itemName = StrFunc::lc( StrFunc::trim( p_itemName ) );
			if ( l_itemName.empty() )
				return ""; // does nothing or returns nothing just like set clipbord value for non existing item

			l_items = dfw_docxMergerFile ->getItemsByName();
			l_itemIterator = l_items ->find( l_itemName );
			if ( l_itemIterator == l_items ->end() )
				return "";

			l_item		= l_itemIterator ->second;
			l_itemPath	= l_item ->getPath();

			l_itemPathRIterator = l_itemPath ->crbegin();
			l_itemPathRIterator++;
			if ( l_itemPathRIterator == l_itemPath ->crend() )
				return "";

			strncpy( l_retVal, ( ( *l_itemPathRIterator ) ->getName() ).c_str(), 255 );
			l_retVal[255] = '\0';
			return l_retVal;
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		dfw_destroy();

		return "?";
	}
} // dfw_getItemParent

const char* dfw_getItemFields( const char* p_itemName )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	multimap<string, DocxMergerField*>::const_iterator	l_fieldIterator;
	const multimap<string, DocxMergerField*>*			l_fields;
	string												l_fieldNames;

	map<string, DocxMergerItem*>::const_iterator		l_itemIterator;
	const map<string, DocxMergerItem*>*					l_items;
	DocxMergerItem*										l_item;
	string												l_itemName;

	static char l_retVal[3072];

	try
	{
		try
		{
			l_itemName = StrFunc::lc( StrFunc::trim( p_itemName ) );
			if ( l_itemName.empty() )
				return "";

			l_items = dfw_docxMergerFile ->getItemsByName();
			l_itemIterator = l_items ->find( l_itemName );
			if ( l_itemIterator == l_items ->end() )
				return "";

			l_fieldNames = "";

			l_item		= l_itemIterator ->second;
			l_fields	= l_item ->getFieldsByName();
			FOR_EACH( l_fieldIterator, l_fields )
			{
				l_fieldNames
					+= ( l_fieldNames.empty() ? "" :  "," )
					+ l_fieldIterator ->first;
			}

			strncpy( l_retVal, l_fieldNames.c_str(), 3071 );
			l_retVal[3071] = '\0';
			return l_retVal;
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		dfw_destroy();

		return "?";
	}
} // dfw_getItemFields



void dfw_setCodePage( const char* p_codePage )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	try
	{
		try
		{
			if ( p_codePage && p_codePage[0] )
				LocaleFunc::setCodePage( p_codePage );
			else
				LocaleFunc::setCodePage();
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;
	}
} // dfw_setCodePage

void dfw_setNumFracSep( char p_frac )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	try
	{
		try
		{
			LocaleFunc::setNumFracSep( p_frac );
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;
	}
} // dfw_setNumFracSep

void dfw_setNumThSep( char p_th )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	try
	{
		try
		{
			LocaleFunc::setNumThSep( p_th );
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;
	}
} // dfw_setNumThSep

void dfw_setDateFormat( const char* p_format )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	try
	{
		try
		{
			LocaleFunc::setDateFormat( p_format );
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;
	}
} // dfw_setDateFormat

void dfw_setYearOffset( unsigned short p_year )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	try
	{
		try
		{
			LocaleFunc::setYearOffset( p_year );
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;
	}
} // dfw_setYearOffset

void dfw_setFirstWeekDay( unsigned short p_weekday )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	try
	{
		try
		{
			if ( p_weekday > 2 )
				throw InvalidFirstWeekDayException( p_weekday, __FILE__, __LINE__ );

			if ( p_weekday == 0 )
				LocaleFunc::setFirstWeekDay();
			else
				LocaleFunc::setFirstWeekDay( ( LocaleFunc::FirstWeekDay ) p_weekday );
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;
	}
} // dfw_setFirstWeekDay

void dfw_setWeekDayNames0()
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	try
	{
		try
		{
			LocaleFunc::setWeekDayNames();
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;
	}
} // dfw_setWeekDayNames0

void dfw_setWeekDayNames7(
	const char* p_weekDay1,
	const char* p_weekDay2,
	const char* p_weekDay3,
	const char* p_weekDay4,
	const char* p_weekDay5,
	const char* p_weekDay6,
	const char* p_weekDay7 )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	try
	{
		try
		{
			LocaleFunc::setWeekDayNames(
				p_weekDay1,
				p_weekDay2,
				p_weekDay3,
				p_weekDay4,
				p_weekDay5,
				p_weekDay6,
				p_weekDay7 );
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;
	}
} // dfw_setWeekDayNames7

void dfw_setWeekDayNames7W(
	const wchar_t* p_weekDay1,
	const wchar_t* p_weekDay2,
	const wchar_t* p_weekDay3,
	const wchar_t* p_weekDay4,
	const wchar_t* p_weekDay5,
	const wchar_t* p_weekDay6,
	const wchar_t* p_weekDay7 )
{
	dfw_setWeekDayNames7(
		LocaleFunc::convertToUtf8( p_weekDay1 ).c_str(),
		LocaleFunc::convertToUtf8( p_weekDay2 ).c_str(),
		LocaleFunc::convertToUtf8( p_weekDay3 ).c_str(),
		LocaleFunc::convertToUtf8( p_weekDay4 ).c_str(),
		LocaleFunc::convertToUtf8( p_weekDay5 ).c_str(),
		LocaleFunc::convertToUtf8( p_weekDay6 ).c_str(),
		LocaleFunc::convertToUtf8( p_weekDay7 ).c_str() );
} // dfw_setWeekDayNames7W

void dfw_setWeekDayNames14(
	const char* p_fullName1,
	const char* p_fullName2,
	const char* p_fullName3,
	const char* p_fullName4,
	const char* p_fullName5,
	const char* p_fullName6,
	const char* p_fullName7,

	const char* p_shortName1,
	const char* p_shortName2,
	const char* p_shortName3,
	const char* p_shortName4,
	const char* p_shortName5,
	const char* p_shortName6,
	const char* p_shortName7 )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	try
	{
		try
		{
			LocaleFunc::setWeekDayNames(
				p_fullName1,
				p_fullName2,
				p_fullName3,
				p_fullName4,
				p_fullName5,
				p_fullName6,
				p_fullName7,
			
				p_shortName1,
				p_shortName2,
				p_shortName3,
				p_shortName4,
				p_shortName5,
				p_shortName6,
				p_shortName7 );
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;
	}
} // dfw_setWeekDayNames14

void dfw_setWeekDayNames14W(
	const wchar_t* p_fullName1,
	const wchar_t* p_fullName2,
	const wchar_t* p_fullName3,
	const wchar_t* p_fullName4,
	const wchar_t* p_fullName5,
	const wchar_t* p_fullName6,
	const wchar_t* p_fullName7,

	const wchar_t* p_shortName1,
	const wchar_t* p_shortName2,
	const wchar_t* p_shortName3,
	const wchar_t* p_shortName4,
	const wchar_t* p_shortName5,
	const wchar_t* p_shortName6,
	const wchar_t* p_shortName7 )
{
	dfw_setWeekDayNames14(
		LocaleFunc::convertToUtf8( p_fullName1 ).c_str(),
		LocaleFunc::convertToUtf8( p_fullName2 ).c_str(),
		LocaleFunc::convertToUtf8( p_fullName3 ).c_str(),
		LocaleFunc::convertToUtf8( p_fullName4 ).c_str(),
		LocaleFunc::convertToUtf8( p_fullName5 ).c_str(),
		LocaleFunc::convertToUtf8( p_fullName6 ).c_str(),
		LocaleFunc::convertToUtf8( p_fullName7 ).c_str(),
		
		LocaleFunc::convertToUtf8( p_shortName1 ).c_str(),
		LocaleFunc::convertToUtf8( p_shortName2 ).c_str(),
		LocaleFunc::convertToUtf8( p_shortName3 ).c_str(),
		LocaleFunc::convertToUtf8( p_shortName4 ).c_str(),
		LocaleFunc::convertToUtf8( p_shortName5 ).c_str(),
		LocaleFunc::convertToUtf8( p_shortName6 ).c_str(),
		LocaleFunc::convertToUtf8( p_shortName7 ).c_str() );
} // dfw_setWeekDayNames14W

void dfw_setMonthNames0()
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	try
	{
		try
		{
			LocaleFunc::setMonthNames();
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;
	}
} // dfw_setMonthNames0

void dfw_setMonthNames12(
	const char* p_month1,
	const char* p_month2,
	const char* p_month3,
	const char* p_month4,
	const char* p_month5,
	const char* p_month6,
	const char* p_month7,
	const char* p_month8,
	const char* p_month9,
	const char* p_month10,
	const char* p_month11,
	const char* p_month12 )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	try
	{
		try
		{
			LocaleFunc::setMonthNames(
				p_month1,
				p_month2,
				p_month3,
				p_month4,
				p_month5,
				p_month6,
				p_month7,
				p_month8,
				p_month9,
				p_month10,
				p_month11,
				p_month12 );
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;
	}
} // dfw_setMonthNames12

void dfw_setMonthNames12W(
	const wchar_t* p_month1,
	const wchar_t* p_month2,
	const wchar_t* p_month3,
	const wchar_t* p_month4,
	const wchar_t* p_month5,
	const wchar_t* p_month6,
	const wchar_t* p_month7,
	const wchar_t* p_month8,
	const wchar_t* p_month9,
	const wchar_t* p_month10,
	const wchar_t* p_month11,
	const wchar_t* p_month12 )
{
	dfw_setMonthNames12(
		LocaleFunc::convertToUtf8( p_month1 ).c_str(),
		LocaleFunc::convertToUtf8( p_month2 ).c_str(),
		LocaleFunc::convertToUtf8( p_month3 ).c_str(),
		LocaleFunc::convertToUtf8( p_month4 ).c_str(),
		LocaleFunc::convertToUtf8( p_month5 ).c_str(),
		LocaleFunc::convertToUtf8( p_month6 ).c_str(),
		LocaleFunc::convertToUtf8( p_month7 ).c_str(),
		LocaleFunc::convertToUtf8( p_month8 ).c_str(),
		LocaleFunc::convertToUtf8( p_month9 ).c_str(),
		LocaleFunc::convertToUtf8( p_month10 ).c_str(),
		LocaleFunc::convertToUtf8( p_month11 ).c_str(),
		LocaleFunc::convertToUtf8( p_month12 ).c_str() );
} // dfw_setMonthNames12W

void dfw_setMonthNames24(
	const char* p_fullName1,
	const char* p_fullName2,
	const char* p_fullName3,
	const char* p_fullName4,
	const char* p_fullName5,
	const char* p_fullName6,
	const char* p_fullName7,
	const char* p_fullName8,
	const char* p_fullName9,
	const char* p_fullName10,
	const char* p_fullName11,
	const char* p_fullName12,

	const char* p_shortName1,
	const char* p_shortName2,
	const char* p_shortName3,
	const char* p_shortName4,
	const char* p_shortName5,
	const char* p_shortName6,
	const char* p_shortName7,
	const char* p_shortName8,
	const char* p_shortName9,
	const char* p_shortName10,
	const char* p_shortName11,
	const char* p_shortName12 )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	try
	{
		try
		{
			LocaleFunc::setMonthNames(
				p_fullName1,
				p_fullName2,
				p_fullName3,
				p_fullName4,
				p_fullName5,
				p_fullName6,
				p_fullName7,
				p_fullName8,
				p_fullName9,
				p_fullName10,
				p_fullName11,
				p_fullName12,

				p_shortName1,
				p_shortName2,
				p_shortName3,
				p_shortName4,
				p_shortName5,
				p_shortName6,
				p_shortName7,
				p_shortName8,
				p_shortName9,
				p_shortName10,
				p_shortName11,
				p_shortName12 );
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;
	}
} // dfw_setMonthNames24

void dfw_setMonthNames24W(
	const wchar_t* p_fullName1,
	const wchar_t* p_fullName2,
	const wchar_t* p_fullName3,
	const wchar_t* p_fullName4,
	const wchar_t* p_fullName5,
	const wchar_t* p_fullName6,
	const wchar_t* p_fullName7,
	const wchar_t* p_fullName8,
	const wchar_t* p_fullName9,
	const wchar_t* p_fullName10,
	const wchar_t* p_fullName11,
	const wchar_t* p_fullName12,

	const wchar_t* p_shortName1,
	const wchar_t* p_shortName2,
	const wchar_t* p_shortName3,
	const wchar_t* p_shortName4,
	const wchar_t* p_shortName5,
	const wchar_t* p_shortName6,
	const wchar_t* p_shortName7,
	const wchar_t* p_shortName8,
	const wchar_t* p_shortName9,
	const wchar_t* p_shortName10,
	const wchar_t* p_shortName11,
	const wchar_t* p_shortName12 )
{
	dfw_setMonthNames24(
		LocaleFunc::convertToUtf8( p_fullName1 ).c_str(),
		LocaleFunc::convertToUtf8( p_fullName2 ).c_str(),
		LocaleFunc::convertToUtf8( p_fullName3 ).c_str(),
		LocaleFunc::convertToUtf8( p_fullName4 ).c_str(),
		LocaleFunc::convertToUtf8( p_fullName5 ).c_str(),
		LocaleFunc::convertToUtf8( p_fullName6 ).c_str(),
		LocaleFunc::convertToUtf8( p_fullName7 ).c_str(),
		LocaleFunc::convertToUtf8( p_fullName8 ).c_str(),
		LocaleFunc::convertToUtf8( p_fullName9 ).c_str(),
		LocaleFunc::convertToUtf8( p_fullName10 ).c_str(),
		LocaleFunc::convertToUtf8( p_fullName11 ).c_str(),
		LocaleFunc::convertToUtf8( p_fullName12 ).c_str(),
		
		LocaleFunc::convertToUtf8( p_shortName1 ).c_str(),
		LocaleFunc::convertToUtf8( p_shortName2 ).c_str(),
		LocaleFunc::convertToUtf8( p_shortName3 ).c_str(),
		LocaleFunc::convertToUtf8( p_shortName4 ).c_str(),
		LocaleFunc::convertToUtf8( p_shortName5 ).c_str(),
		LocaleFunc::convertToUtf8( p_shortName6 ).c_str(),
		LocaleFunc::convertToUtf8( p_shortName7 ).c_str(),
		LocaleFunc::convertToUtf8( p_shortName8 ).c_str(),
		LocaleFunc::convertToUtf8( p_shortName9 ).c_str(),
		LocaleFunc::convertToUtf8( p_shortName10 ).c_str(),
		LocaleFunc::convertToUtf8( p_shortName11 ).c_str(),
		LocaleFunc::convertToUtf8( p_shortName12 ).c_str() );
} // dfw_setMonthNames24W



const char*	dfw_getCodePage()
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	static char l_retVal[256];

	try
	{
		try
		{
			strncpy( l_retVal, LocaleFunc::getCodePage().c_str(), 255 );
			l_retVal[255] = '\0';
			return l_retVal;
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		return 0;
	}
} // dfw_getCodePage

char dfw_getNumFracSep()
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	try
	{
		try
		{
			return LocaleFunc::getNumFracSep();
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		return 0;
	}
} // dfw_getNumFracSep

char dfw_getNumThSep()
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	try
	{
		try
		{
			return LocaleFunc::getNumThSep();
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		return 0;
	}
} // dfw_getNumThSep

const char* dfw_getDateFormat()
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	static char l_retVal[4];

	try
	{
		try
		{
			strncpy( l_retVal, LocaleFunc::getDateFormat().c_str(), 3 );
			l_retVal[3] = '\0';
			return l_retVal;
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		return "?";
	}
} // dfw_getDateFormat

unsigned short dfw_getYearOffset()
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	try
	{
		try
		{
			return LocaleFunc::getYearOffset();
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		return 0;
	}
} // dfw_getYearOffset

unsigned short dfw_getFirstWeekDay()
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	try
	{
		try
		{
			return LocaleFunc::getFirstWeekDay();
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		return 0;
	}
} // dfw_getFirstWeekDay

const char* dfw_getWeekDayFullNames()
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	static char l_retVal[1024];

	try
	{
		try
		{
			strncpy( l_retVal, LocaleFunc::getWeekDayFullNames() ->c_str(), 1023 );
			l_retVal[1023] = '\0';
			return l_retVal;
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		return "?";
	}
} // dfw_getWeekDayFullNames

const wchar_t* dfw_getWeekDayFullNamesW()
{
	return LocaleFunc::convertToUtf16( dfw_getWeekDayFullNames() ).c_str();
} // dfw_getWeekDayFullNamesW

const char* dfw_getWeekDayShortNames()
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	static char l_retVal[1024];

	try
	{
		try
		{
			strncpy( l_retVal, LocaleFunc::getWeekDayShortNames() ->c_str(), 1023 );
			l_retVal[1023] = '\0';
			return l_retVal;
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		return "?";
	}
} // dfw_getWeekDayShortNames

const wchar_t* dfw_getWeekDayShortNamesW()
{
	return LocaleFunc::convertToUtf16( dfw_getWeekDayShortNames() ).c_str();
} // dfw_getWeekDayShortNamesW

const char* dfw_getMonthFullNames()
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	static char l_retVal[1024];

	try
	{
		try
		{
			strncpy( l_retVal, LocaleFunc::getMonthFullNames() ->c_str(), 1023 );
			l_retVal[1023] = '\0';
			return l_retVal;
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		return "?";
	}
} // getMonthFullNames

const wchar_t* dfw_getMonthFullNamesW()
{
	return LocaleFunc::convertToUtf16( dfw_getMonthFullNames() ).c_str();
} // dfw_getMonthFullNamesW

const char* dfw_getMonthShortNames()
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	static char l_retVal[1024];

	try
	{
		try
		{
			strncpy( l_retVal, LocaleFunc::getMonthShortNames() ->c_str(), 1023 );
			l_retVal[1023] = '\0';
			return l_retVal;
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		return "?";
	}
} // dfw_getMonthShortNames

const wchar_t* dfw_getMonthShortNamesW()
{
	return LocaleFunc::convertToUtf16( dfw_getMonthShortNames() ).c_str();
} // dfw_getMonthShortNamesW



void dfw_setTempDir( const char* p_dir )
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	try
	{
		try
		{
			if ( p_dir && p_dir[0] )
				OsFunc::setTempDir( p_dir );
			else
				OsFunc::setTempDir();
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;
	}
} // dfw_setTempDir

const char* dfw_getTempDir()
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	static char l_retVal[256];

	try
	{
		try
		{
			strncpy( l_retVal, OsFunc::getTempDir().c_str(), 255 );
			l_retVal[255] = '\0';
			return l_retVal;
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		return "?";
	}
} // dfw_getTempDir

const char* dfw_getWorkDir()
{
	if ( !dfw_initialized )
		dfw_initialize();

	dfw_errorWhat	= "";
	dfw_errorWhere	= "";
	dfw_errorFlag	= false;

	static char l_retVal[256];

	try
	{
		try
		{
			strncpy( l_retVal, OsFunc::getWorkDir().c_str(), 255 );
			l_retVal[255] = '\0';
			return l_retVal;
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfw_errorWhat	= e.what();
		dfw_errorWhere	= e.where();
		dfw_errorFlag	= true;

		return "?";
	}
} // dfw_getWorkDir



const char* dfw_getErrorWhat()
{
	if ( !dfw_initialized )
		dfw_initialize();

	static char l_retVal[1024];

	strncpy( l_retVal, dfw_errorWhat.c_str(), 1023 );
	l_retVal[1023] = '\0';
	return l_retVal;
} // dfw_getErrorWhat

const char* dfw_getErrorWhere()
{
	if ( !dfw_initialized )
		dfw_initialize();

	static char l_retVal[256];

	strncpy( l_retVal, dfw_errorWhere.c_str(), 255 );
	l_retVal[255] = '\0';
	return l_retVal;
} // dfw_getErrorWhere

unsigned char dfw_getErrorFlag()
{
	if ( !dfw_initialized )
		dfw_initialize();

	return ( dfw_errorFlag ? 1 : 0 );
} // dfw_getErrorFlag

#ifdef __cplusplus
}
#endif
