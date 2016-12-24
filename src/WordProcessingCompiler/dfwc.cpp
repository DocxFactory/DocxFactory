
#define DOCXFACTORY_DLL_EXPORT
#include "DocxFactory/WordProcessingCompiler/dfwc.h"
#undef  DOCXFACTORY_DLL_EXPORT

#include "DocxFactory/DocxCompiler/DocxCompilerFile.h"

#include "DocxFactory/xml/XmlFunc.h"
#include "DocxFactory/xml/DomException.h"

#include "DocxFactory/os/OsFunc.h"
#include "DocxFactory/os/UnsupportedFileTypeException.h"
#include "DocxFactory/os/FileNotFoundException.h"
#include "DocxFactory/os/DirNotFoundException.h"

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



string	dfwc_errorWhat;
string	dfwc_errorWhere;
bool	dfwc_errorFlag;
bool	dfwc_initialized = false;


#ifdef __cplusplus
extern "C" {
#endif

void dfwc_initialize()
{
	dfwc_initialized	= true;

	dfwc_errorWhat		= "";
	dfwc_errorWhere		= "";
	dfwc_errorFlag		= false;
} // dfwc_initialize



void dfwc_compile( const char* p_srcFileName, const char* p_dstFileName )
{
	if ( !dfwc_initialized )
		dfwc_initialize();

	dfwc_errorWhat	= "";
	dfwc_errorWhere	= "";
	dfwc_errorFlag	= false;

	DocxCompilerFile* l_docxCompilerFile = NULL;

	string	l_srcFileName( p_srcFileName );
	string	l_dstFileName( p_dstFileName );
	string	l_dir;
	string	l_file;
	string	l_ext;

	try
	{
		try
		{
			l_srcFileName = OsFunc::normalizePath( l_srcFileName );
			l_srcFileName = OsFunc::getFullPath( l_srcFileName );

			if ( OsFunc::getSubPath( l_srcFileName, OsFunc::SUBPATH_EXT, OsFunc::SUBPATH_EXT ) != ".docx" )
				throw UnsupportedFileTypeException( l_srcFileName, ".docx", __FILE__, __LINE__ );

			if ( !OsFunc::isFileExists( l_srcFileName ) )
				throw FileNotFoundException( l_srcFileName, __FILE__, __LINE__ );

			if(!l_dstFileName.empty() ) 
			{
				l_dstFileName = OsFunc::normalizePath( l_dstFileName );
				l_dstFileName = OsFunc::getFullPath( l_dstFileName );

				/*
				if ( OsFunc::isRelativePath( l_dstFileName ) )

					l_dstFileName = OsFunc::normalizePath(
						  OsFunc::getSubPath( l_srcFileName, OsFunc::SUBPATH_DIR, OsFunc::SUBPATH_DIR ) + "/"
						+ l_dstFileName );
				*/

				if ( OsFunc::isDirExists( l_dstFileName ) )

					l_dstFileName = OsFunc::normalizePath( l_dstFileName + "/"
						+ OsFunc::getSubPath( l_srcFileName, OsFunc::SUBPATH_FILE, OsFunc::SUBPATH_FILE )
						+ ".dfw" );

				else
				{
					OsFunc::breakPath(
						l_dstFileName,
						l_dir,
						l_file,
						l_ext );

					if ( !OsFunc::isDirExists( l_dir ) )
						throw DirNotFoundException( l_dir, __FILE__, __LINE__ );

					if( l_ext.empty() )
						l_ext = ".dfw";

					l_dstFileName = l_dir + l_file + l_ext;
				}
			}

			else
			{
				l_dstFileName = OsFunc::getSubPath( l_srcFileName, OsFunc::SUBPATH_DIR, OsFunc::SUBPATH_FILE ) + ".dfw";
			}

			if( OsFunc::isFileExists( l_dstFileName ) )
				OsFunc::remove( l_dstFileName );

			l_docxCompilerFile = new DocxCompilerFile();
			l_docxCompilerFile ->compile( l_srcFileName, l_dstFileName );

			delete l_docxCompilerFile;
		}

		catch ( DocxFactoryException )				{ throw; }
		catch ( const xercesc::DOMException& e )	{ throw DomException	( XmlFunc::XMLChToUtf8( e.getMessage() ), __FILE__, __LINE__ ); }
		catch ( const exception& e )				{ throw GeneralException( e.what(), __FILE__, __LINE__ ); }
		catch ( ... )								{ throw UnknownException( __FILE__, __LINE__ ); }
	}

	catch ( const DocxFactoryException& e )
	{ 
		dfwc_errorWhat	= e.what();
		dfwc_errorWhere	= e.where();
		dfwc_errorFlag	= true;
	}
} // dfwc_compile



void dfwc_setTempDir( const char* p_dir )
{
	if ( !dfwc_initialized )
		dfwc_initialize();

	dfwc_errorWhat	= "";
	dfwc_errorWhere	= "";
	dfwc_errorFlag	= false;

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
		dfwc_errorWhat	= e.what();
		dfwc_errorWhere	= e.where();
		dfwc_errorFlag	= true;
	}
} // dfwc_setTempDir

const char* dfwc_getTempDir()
{
	if ( !dfwc_initialized )
		dfwc_initialize();

	dfwc_errorWhat	= "";
	dfwc_errorWhere	= "";
	dfwc_errorFlag	= false;

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
		dfwc_errorWhat	= e.what();
		dfwc_errorWhere	= e.where();
		dfwc_errorFlag	= true;

		return "?";
	}
} // dfwc_getTempDir

const char* dfwc_getWorkDir()
{
	if ( !dfwc_initialized )
		dfwc_initialize();

	dfwc_errorWhat	= "";
	dfwc_errorWhere	= "";
	dfwc_errorFlag	= false;

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
		dfwc_errorWhat	= e.what();
		dfwc_errorWhere	= e.where();
		dfwc_errorFlag	= true;

		return "?";
	}
} // dfwc_getWorkDir



const char* dfwc_getErrorWhat()
{
	if ( !dfwc_initialized )
		dfwc_initialize();

	static char l_retVal[1024];

	strncpy( l_retVal, dfwc_errorWhat.c_str(), 1023 );
	l_retVal[1023] = '\0';
	return l_retVal;
} // dfwc_getErrorWhat

const char* dfwc_getErrorWhere()
{
	if ( !dfwc_initialized )
		dfwc_initialize();

	static char l_retVal[256];

	strncpy( l_retVal, dfwc_errorWhere.c_str(), 255 );
	l_retVal[255] = '\0';
	return l_retVal;
} // dfwc_getErrorWhere

unsigned char dfwc_getErrorFlag()
{
	if ( !dfwc_initialized )
		dfwc_initialize();

	return ( dfwc_errorFlag ? 1 : 0 );
} // dfwc_getErrorFlag

#ifdef __cplusplus
}
#endif
