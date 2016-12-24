
#include "DocxFactory/ConverterPrinter/OdfConverterUtil.h"
#include "DocxFactory/ConverterPrinter/GradeGroup.h"
#include "DocxFactory/ConverterPrinter/PrintingNotSupportedException.h"
#include "DocxFactory/ConverterPrinter/UtilNotFoundException.h"
#include "DocxFactory/ConverterPrinter/UtilFailedException.h"

#include "DocxFactory/os/OsFunc.h"

#ifdef __unix__

#include <sys/types.h>
#include <sys/stat.h>

#endif

#include <cstdlib>

using namespace DocxFactory;
using namespace std;



#if defined( _WIN32 ) || defined( _WIN64 )

	#define ODF_CONVERTER_CMD( p_exec, p_importFile, p_exportFile ) \
		string( "(\"" ) + p_exec + "\" /I \"" + p_importFile + "\" /O \"" + p_exportFile + "\" /F 1> nul 2> nul)"

#else

	#define ODF_CONVERTER_CMD( p_exec, p_importFile, p_exportFile ) \
		string( "\"" ) + p_exec + "\" /I \"" + p_importFile + "\" /O \"" + p_exportFile + "\" /F 1> /dev/null 2> /dev/null"

#endif



OdfConverterUtil::OdfConverterUtil() : ConverterPrinterUtil()
{
	string l_exec;

	#if defined( _WIN32 ) || defined( _WIN64 )
		l_exec = "OdfConverter.exe";
	#else
		l_exec = "OdfConverter";
	#endif

	m_exec = OsFunc::searchPath( l_exec );

	if ( m_exec == "" )
		throw UtilNotFoundException( __FILE__, __LINE__ );
	
	initializeGrades();
} // c'tor

OdfConverterUtil::~OdfConverterUtil()
{
	list<GradeGroup*>::iterator l_gradeGroupIterator;
	FOR_EACH( l_gradeGroupIterator, &m_gradeGroups )
	{
		delete *l_gradeGroupIterator;
	}
} // d'tor



void OdfConverterUtil::saveAs( const string& p_importFile, const string& p_exportFile )
{
	string	l_importExt = OsFunc::getSubPath( p_importFile, OsFunc::SUBPATH_EXT, OsFunc::SUBPATH_EXT );
	string	l_exportExt = OsFunc::getSubPath( p_exportFile, OsFunc::SUBPATH_EXT, OsFunc::SUBPATH_EXT );

	if ( isOpenXml( l_importExt ) && isOpenXml( l_exportExt ) )
	{
		string l_tempExt	= openXmlToOdf( l_importExt );
		string l_tempFile	= OsFunc::getTempFile( l_tempExt );

		try
		{
			saveAs( p_importFile, l_tempFile );
			saveAs( l_tempFile, p_exportFile );

			OsFunc::remove( l_tempFile );
		}

		catch ( ... )
		{
			if( OsFunc::isFileExists( l_tempFile ) )
				OsFunc::remove( l_tempFile );

			throw;
		}
	}

	else
	{
		string	l_cmd		= ODF_CONVERTER_CMD( m_exec, p_importFile, p_exportFile );
		int		l_exitCode	= system( l_cmd.c_str() );

		if ( l_exitCode != 0 )
			throw UtilFailedException( m_exec, __FILE__, __LINE__ );

		#ifdef __unix__
			if ( chmod( p_exportFile.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH ) != 0 )
				throw UtilFailedException( "chmod", __FILE__, __LINE__ );
		#endif
	}
} // saveAs

void OdfConverterUtil::print( const string& p_fileName, const string& p_printerName, int p_copyCnt )
{
	// this function will never be called since getPrintingGrade() always returns -1
	throw PrintingNotSupportedException( p_fileName, __FILE__, __LINE__ );
} // print



double OdfConverterUtil::getConversionGrade( const string& p_importExt, const string& p_exportExt )
{
	list<GradeGroup*>::iterator	l_gradeGroupIterator;
	GradeGroup*					l_gradeGroup;

	double	l_grade1;
	double	l_grade2;
	double	l_grade = -1;

	if ( isOpenXml( p_importExt ) && isOpenXml( p_exportExt ) )
	{
		string l_tempExt = openXmlToOdf( p_importExt );

		l_grade1 = getConversionGrade( p_importExt, l_tempExt );
		l_grade2 = getConversionGrade( l_tempExt, p_exportExt );

		if( l_grade1 != -1 && l_grade2 != -1 )
		{
			l_grade = ( l_grade1 + l_grade2 ) / 2;
		}
	}

	else
	{
		FOR_EACH( l_gradeGroupIterator, &m_gradeGroups )
		{
			l_gradeGroup = *l_gradeGroupIterator;

			l_grade1 = l_gradeGroup ->getImportGrade( p_importExt );
			l_grade2 = l_gradeGroup ->getExportGrade( p_exportExt );

			if( l_grade1 != -1 && l_grade2 != -1 )
			{
				l_grade = ( l_grade1 + l_grade2 ) / 2;
				break;
			}
		}
	}

	return l_grade;
} // getConversionGrade

double OdfConverterUtil::getPrintingGrade( const string& p_ext )
{
	return -1;
} // getPrintingGrade



void OdfConverterUtil::initializeGrades()
{
	GradeGroup* l_gradeGroup;

	// ODS2XLSX
	l_gradeGroup = new GradeGroup();

	l_gradeGroup ->insertImportGrade( ".ods", 3 );
	l_gradeGroup ->insertExportGrade( ".xlsx", 3 );
//	l_gradeGroup ->insertExportGrade( ".xlsm", 3 ); - conversion fails

	m_gradeGroups.push_back( l_gradeGroup );

	// OTS2XLTX
	l_gradeGroup = new GradeGroup();

	l_gradeGroup ->insertImportGrade( ".ots", 3 );
	l_gradeGroup ->insertExportGrade( ".xltx", 3 );
//	l_gradeGroup ->insertExportGrade( ".xltm", 3 ); - conversion fails

	m_gradeGroups.push_back( l_gradeGroup );

	// XLSX2ODS
	l_gradeGroup = new GradeGroup();

	l_gradeGroup ->insertImportGrade( ".xlsx", 3 );
	l_gradeGroup ->insertImportGrade( ".xlsm", 3 );
	l_gradeGroup ->insertExportGrade( ".ods", 3 );

	m_gradeGroups.push_back( l_gradeGroup );

	// XLTX2OTS
	l_gradeGroup = new GradeGroup();

	l_gradeGroup ->insertImportGrade( ".xltx", 3 );
	l_gradeGroup ->insertImportGrade( ".xltm", 3 );
	l_gradeGroup ->insertExportGrade( ".ots", 3 );

	m_gradeGroups.push_back( l_gradeGroup );

	// ODT2DOCX
	l_gradeGroup = new GradeGroup();

	l_gradeGroup ->insertImportGrade( ".odt", 2 );
	l_gradeGroup ->insertExportGrade( ".docx", 2 );
//	l_gradeGroup ->insertExportGrade( ".docm", 2 ); - conversion fails

	m_gradeGroups.push_back( l_gradeGroup );

	// OTT2DOTX
	l_gradeGroup = new GradeGroup();

	l_gradeGroup ->insertImportGrade( ".ott", 2 );
	l_gradeGroup ->insertExportGrade( ".dotx", 2 );
//	l_gradeGroup ->insertExportGrade( ".dotm", 2 ); - conversion fails

	m_gradeGroups.push_back( l_gradeGroup );

	// DOCX2ODT
	l_gradeGroup = new GradeGroup();

	l_gradeGroup ->insertImportGrade( ".docx", 2 );
	l_gradeGroup ->insertImportGrade( ".docm", 2 );
	l_gradeGroup ->insertExportGrade( ".odt", 2 );

	m_gradeGroups.push_back( l_gradeGroup );

	// DOTX2OTT
	l_gradeGroup = new GradeGroup();

	l_gradeGroup ->insertImportGrade( ".dotx", 2 );
	l_gradeGroup ->insertImportGrade( ".dotm", 2 );
	l_gradeGroup ->insertExportGrade( ".ott", 2 );

	m_gradeGroups.push_back( l_gradeGroup );
} // initializeGrades



string OdfConverterUtil::openXmlToOdf( const string& p_openXmlExt )
{
	string l_odfExt;

		 if ( p_openXmlExt == ".docx" || p_openXmlExt == ".docm" ) l_odfExt = ".odt";
	else if ( p_openXmlExt == ".dotx" || p_openXmlExt == ".dotm" ) l_odfExt = ".ott";
	else if ( p_openXmlExt == ".xlsx" || p_openXmlExt == ".xlsm" ) l_odfExt = ".ods";
	else if ( p_openXmlExt == ".xltx" || p_openXmlExt == ".xltm" ) l_odfExt = ".ots";

	return l_odfExt;
} // openXmlToOdf

bool OdfConverterUtil::isOpenXml( const string& p_ext )
{
	return
       ( p_ext == ".docx"
	  || p_ext == ".docm"
	  || p_ext == ".dotx"
	  || p_ext == ".dotm"
	  || p_ext == ".xlsx"
	  || p_ext == ".xlsm"
	  || p_ext == ".xltx"
	  || p_ext == ".xltm" );
} // isOpenXml
