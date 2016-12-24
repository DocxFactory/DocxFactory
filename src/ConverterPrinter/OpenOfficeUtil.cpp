
#include "DocxFactory/ConverterPrinter/OpenOfficeUtil.h"
#include "DocxFactory/ConverterPrinter/OdfConverterUtil.h"
#include "DocxFactory/ConverterPrinter/GradeGroup.h"
#include "DocxFactory/ConverterPrinter/ConverterPrinterException.h"
#include "DocxFactory/ConverterPrinter/ConversionNotSupportedException.h"
#include "DocxFactory/ConverterPrinter/UtilNotFoundException.h"

#include "DocxFactory/UnoUtil/UnoUtil.h"

#include "DocxFactory/os/OsFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include <cstdio>

using namespace DocxFactory;
using namespace std;



OpenOfficeUtil::OpenOfficeUtil( OdfConverterUtil* p_odfConverter ) : ConverterPrinterUtil()
{
	m_odfConverter			= p_odfConverter;
	m_unoUtil				= NULL;
	m_destroyUnoUtilFunc	= NULL;

	initializeUnoUtil();

	if ( !m_unoUtil )
		throw UtilNotFoundException( __FILE__, __LINE__ );

	initializeGrades();
} // c'tor

OpenOfficeUtil::~OpenOfficeUtil()
{
	list<GradeGroup*>::iterator l_gradeGroupIterator;
	FOR_EACH( l_gradeGroupIterator, &m_gradeGroups )
	{
		delete *l_gradeGroupIterator;
	}

	m_destroyUnoUtilFunc( m_unoUtil );
} // d'tor



void OpenOfficeUtil::saveAs( const string& p_importFile, const string& p_exportFile )
{
	string	l_importExt		= OsFunc::getSubPath( p_importFile, OsFunc::SUBPATH_EXT, OsFunc::SUBPATH_EXT );
	string	l_exportExt		= OsFunc::getSubPath( p_exportFile, OsFunc::SUBPATH_EXT, OsFunc::SUBPATH_EXT );

	bool	l_importOpenXml	= isOpenXml( l_importExt );
	bool	l_exportOpenXml	= isOpenXml( l_exportExt );

	if ( m_odfConverter && ( l_importOpenXml || l_exportOpenXml ) )
	{
		string l_tempExt;
		string l_tempFile;

		try
		{
			if ( l_importOpenXml )
			{
				l_tempExt	= openXmlToOdf( l_importExt );
				l_tempFile	= OsFunc::getTempFile( l_tempExt );

				m_odfConverter ->saveAs( p_importFile, l_tempFile );
				m_unoUtil ->saveAs( l_tempFile, p_exportFile );

				OsFunc::remove( l_tempFile );
			}

			else
			{
				l_tempExt	= openXmlToOdf( l_exportExt );
				l_tempFile	= OsFunc::getTempFile( l_tempExt );

				m_unoUtil ->saveAs( p_importFile, l_tempFile );
				m_odfConverter ->saveAs( l_tempFile, p_exportFile );

				OsFunc::remove( l_tempFile );
			}
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
		m_unoUtil ->saveAs( p_importFile, p_exportFile );
	}
} // saveAs

void OpenOfficeUtil::print( const string& p_fileName, const string& p_printerName, int p_copyCnt )
{
	string l_ext = OsFunc::getSubPath( p_fileName, OsFunc::SUBPATH_EXT, OsFunc::SUBPATH_EXT );
	if ( m_odfConverter && isOpenXml( l_ext ) )
	{
		string l_tempExt;
		string l_tempFile;

		try
		{
			l_tempExt	= openXmlToOdf( l_ext );
			l_tempFile	= OsFunc::getTempFile( l_tempExt );

			m_odfConverter ->saveAs( p_fileName, l_tempFile );
			m_unoUtil ->print( l_tempFile, p_printerName, p_copyCnt );

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
		m_unoUtil ->print( p_fileName, p_printerName, p_copyCnt );
	}
} // print



double OpenOfficeUtil::getConversionGrade( const string& p_importExt, const string& p_exportExt )
{
	list<GradeGroup*>::iterator	l_gradeGroupIterator;
	GradeGroup*					l_gradeGroup;

	bool	l_importOpenXml	= isOpenXml( p_importExt );
	bool	l_exportOpenXml	= isOpenXml( p_exportExt );

	double	l_grade1;
	double	l_grade2;
	double	l_grade = -1;

	if ( m_odfConverter && ( l_importOpenXml || l_exportOpenXml ) )
	{
		if ( m_odfConverter ->getConversionGrade( p_importExt, p_exportExt ) == -1 )
		{
			string l_tempExt;

			if ( l_importOpenXml )
			{
				l_tempExt = openXmlToOdf( p_importExt );

				l_grade1 = m_odfConverter ->getConversionGrade( p_importExt, l_tempExt );
				l_grade2 = getConversionGrade( l_tempExt, p_exportExt );

				if( l_grade1 != -1 && l_grade2 != -1 )
					l_grade = ( l_grade1 + l_grade2 ) / 2;
			}

			else
			{
				l_tempExt = openXmlToOdf( p_exportExt );

				l_grade1 = getConversionGrade( l_tempExt, p_exportExt );
				l_grade2 = m_odfConverter ->getConversionGrade( p_importExt, l_tempExt );

				if( l_grade1 != -1 && l_grade2 != -1 )
					l_grade = ( l_grade1 + l_grade2 ) / 2;
			}
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

double OpenOfficeUtil::getPrintingGrade( const string& p_ext )
{
	list<GradeGroup*>::iterator	l_gradeGroupIterator;
	GradeGroup*					l_gradeGroup;

	double	l_grade1;
	double	l_grade2;
	double	l_grade = -1;

	if ( m_odfConverter && isOpenXml( p_ext ) )
	{
		string l_tempExt = openXmlToOdf( p_ext );

		l_grade1 = m_odfConverter ->getConversionGrade( p_ext, l_tempExt );
		l_grade2 = getPrintingGrade( l_tempExt );

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

			l_grade = l_gradeGroup ->getImportGrade( p_ext );
			if ( l_grade != -1 )
				break;
		}
	}

	return l_grade;
} // getPrintingGrade



void OpenOfficeUtil::initializeGrades()
{
	GradeGroup* l_gradeGroup;

	// Writer
	l_gradeGroup = new GradeGroup();

	l_gradeGroup ->insertImportGrade( ".odf", 1 );
	l_gradeGroup ->insertImportGrade( ".odt", 1 );
	l_gradeGroup ->insertImportGrade( ".sdw", 1 );
	l_gradeGroup ->insertImportGrade( ".sxw", 1 );
	l_gradeGroup ->insertImportGrade( ".uot", 1 );
	l_gradeGroup ->insertImportGrade( ".wpd", 1 );
	l_gradeGroup ->insertImportGrade( ".hwp", 1 );
	l_gradeGroup ->insertImportGrade( ".602", 1 );

	l_gradeGroup ->insertImportGrade( ".doc", 2 );
	l_gradeGroup ->insertImportGrade( ".htm", 2 );
	l_gradeGroup ->insertImportGrade( ".html", 2 );
	l_gradeGroup ->insertImportGrade( ".rtf", 2 );
	l_gradeGroup ->insertImportGrade( ".txt", 2 );

	l_gradeGroup ->insertImportGrade( ".docx", 4 );
	l_gradeGroup ->insertImportGrade( ".docm", 4 );
	l_gradeGroup ->insertImportGrade( ".dotx", 4 );
	l_gradeGroup ->insertImportGrade( ".dotm", 4 );

	l_gradeGroup ->insertExportGrade( ".odt", 1 );
	l_gradeGroup ->insertExportGrade( ".sdw", 1 );
	l_gradeGroup ->insertExportGrade( ".sxw", 1 );
	l_gradeGroup ->insertExportGrade( ".uot", 1 );
	l_gradeGroup ->insertExportGrade( ".ltx", 1 );
	l_gradeGroup ->insertExportGrade( ".bib", 1 );

	l_gradeGroup ->insertExportGrade( ".doc", 2 );
	l_gradeGroup ->insertExportGrade( ".pdf", 2 );
	l_gradeGroup ->insertExportGrade( ".htm", 2 );
	l_gradeGroup ->insertExportGrade( ".html", 2 );
	l_gradeGroup ->insertExportGrade( ".xhtml", 2 );
	l_gradeGroup ->insertExportGrade( ".rtf", 2 );
	l_gradeGroup ->insertExportGrade( ".txt", 2 );

	m_gradeGroups.push_back( l_gradeGroup );

	// Calc
	l_gradeGroup = new GradeGroup();

	l_gradeGroup ->insertImportGrade( ".ods", 1 );
	l_gradeGroup ->insertImportGrade( ".sdc", 1 );
	l_gradeGroup ->insertImportGrade( ".sxc", 1 );
	l_gradeGroup ->insertImportGrade( ".uos", 1 );
	l_gradeGroup ->insertImportGrade( ".wp2", 1 );
	l_gradeGroup ->insertImportGrade( ".wpf", 1 );
	l_gradeGroup ->insertImportGrade( ".wk1", 1 );
	l_gradeGroup ->insertImportGrade( ".wks", 1 );
	l_gradeGroup ->insertImportGrade( ".123", 1 );

	l_gradeGroup ->insertImportGrade( ".xlsx", 2 );
	l_gradeGroup ->insertImportGrade( ".xlsm", 2 );
	l_gradeGroup ->insertImportGrade( ".xltx", 2 );
	l_gradeGroup ->insertImportGrade( ".xltm", 2 );
	l_gradeGroup ->insertImportGrade( ".xls", 2 );
	l_gradeGroup ->insertImportGrade( ".dbf", 2 );
	l_gradeGroup ->insertImportGrade( ".dif", 2 );
	l_gradeGroup ->insertImportGrade( ".htm", 2 );
	l_gradeGroup ->insertImportGrade( ".html", 2 );
	l_gradeGroup ->insertImportGrade( ".xhtml", 2 );
	l_gradeGroup ->insertImportGrade( ".slk", 2 );
	l_gradeGroup ->insertImportGrade( ".csv", 2 );
	l_gradeGroup ->insertImportGrade( ".tsv", 2 );
	l_gradeGroup ->insertImportGrade( ".rtf", 2 );

	l_gradeGroup ->insertExportGrade( ".ods", 1 );
	l_gradeGroup ->insertExportGrade( ".sdc", 1 );
	l_gradeGroup ->insertExportGrade( ".sxc", 1 );
	l_gradeGroup ->insertExportGrade( ".uos", 1 );

	l_gradeGroup ->insertExportGrade( ".xls", 2 );
	l_gradeGroup ->insertExportGrade( ".xlt", 2 );
	l_gradeGroup ->insertExportGrade( ".dbf", 2 );
	l_gradeGroup ->insertExportGrade( ".dif", 2 );
	l_gradeGroup ->insertExportGrade( ".pdf", 2 );
	l_gradeGroup ->insertExportGrade( ".htm", 2 );
	l_gradeGroup ->insertExportGrade( ".html", 2 );
	l_gradeGroup ->insertExportGrade( ".xhtml", 2 );
	l_gradeGroup ->insertExportGrade( ".slk", 2 );
	l_gradeGroup ->insertExportGrade( ".csv", 2 );
	l_gradeGroup ->insertExportGrade( ".tsv", 2 );

	m_gradeGroups.push_back( l_gradeGroup );
} // initializeGradeGroup



string OpenOfficeUtil::openXmlToOdf( const string& p_openXmlExt )
{
	string l_odfExt;

		 if ( p_openXmlExt == ".docx" || p_openXmlExt == ".docm" ) l_odfExt = ".odt";
	else if ( p_openXmlExt == ".dotx" || p_openXmlExt == ".dotm" ) l_odfExt = ".ott";
	else if ( p_openXmlExt == ".xlsx" || p_openXmlExt == ".xlsm" ) l_odfExt = ".ods";
	else if ( p_openXmlExt == ".xltx" || p_openXmlExt == ".xltm" ) l_odfExt = ".ots";

	return l_odfExt;
} // openXmlToOdf

bool OpenOfficeUtil::isOpenXml( const string& p_ext )
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



#if defined( _WIN32 ) || defined( _WIN64 )

	void OpenOfficeUtil::initializeUnoUtil()
	{
		#ifndef _WIN64

			__try
			{
				m_unoUtil				= createUnoUtil();
				m_destroyUnoUtilFunc	= destroyUnoUtil;
			}

			__except ( 1 )
			{
				m_unoUtil				= NULL;
				m_destroyUnoUtilFunc	= NULL;
			}

		#endif
	} // initializeUnoUtil

#else

	#include <dlfcn.h>

	void OpenOfficeUtil::initializeUnoUtil()
	{
		typedef UnoUtil* ( *create_func_ptr )();
		typedef void ( *destroy_func_ptr )( UnoUtil* );

		void*				l_hndl			= NULL;
		create_func_ptr		l_createFunc	= NULL;
		destroy_func_ptr	l_destroyFunc	= NULL;

		l_hndl = dlopen( "libUnoUtil.so", RTLD_NOW | RTLD_GLOBAL );

		if ( l_hndl != NULL )
		{
			l_createFunc	= ( create_func_ptr ) dlsym( l_hndl, "createUnoUtil" );
			l_destroyFunc	= ( destroy_func_ptr ) dlsym( l_hndl, "destroyUnoUtil" );

			if ( l_createFunc != NULL && l_destroyFunc != NULL )
			{
				m_unoUtil				= l_createFunc();
				m_destroyUnoUtilFunc	= l_destroyFunc;
			}
		}
	} // initializeUnoUtil

#endif
