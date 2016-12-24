
#include "DocxFactory/ConverterPrinter/MsExcelUtil.h"
#include "DocxFactory/ConverterPrinter/ConverterPrinterException.h"
#include "DocxFactory/ConverterPrinter/ExportFormatNotSupportedException.h"
#include "DocxFactory/ConverterPrinter/UtilNotFoundException.h"

#include "DocxFactory/os/OsFunc.h"
#include "DocxFactory/math/MathFunc.h"

using namespace DocxFactory;
using namespace std;



MsExcelUtil::MsExcelUtil() : MsOfficeUtil( "Excel.Application", "Workbooks" )
{
	initializeGrades();
	initializeSaveProperties();
} // c'tor

MsExcelUtil::~MsExcelUtil()
{

} // d'tor



void MsExcelUtil::saveDocAs( IDispatch* p_docDispatch, const string& p_fileName )
{
	const SaveProperties* l_saveProperties;

	string		l_ext;
	VARIANT		l_args[2];
	DISPPARAMS	l_dispParams	= { l_args, NULL, 2, 0 };
	BSTR		l_fileName		= NULL;
	OLECHAR*	l_methodName;
	DISPID		l_methodId;
	HRESULT		l_result;

	try
	{
		l_ext				= OsFunc::getSubPath( p_fileName, OsFunc::SUBPATH_EXT, OsFunc::SUBPATH_EXT );
		l_saveProperties	= getSavePropertiesByExt( l_ext );

		if ( l_saveProperties == NULL )
			throw ExportFormatNotSupportedException( l_ext, __FILE__, __LINE__ );

		l_fileName = SysAllocString( wstring( p_fileName.begin(), p_fileName.end() ).c_str() );
	
		if ( l_saveProperties ->m_methodName == "SaveAs" )
		{
			l_methodName		= L"SaveAs";
			l_args[0].vt		= VT_I4;
			l_args[0].intVal	= l_saveProperties ->m_format;
			l_args[1].vt		= VT_BSTR;
			l_args[1].bstrVal	= l_fileName;
		}

		else
		if ( l_saveProperties ->m_methodName == "ExportAsFixedFormat" )
		{
			l_methodName		= L"ExportAsFixedFormat";
			l_args[0].vt		= VT_BSTR;
			l_args[0].bstrVal	= l_fileName;
			l_args[1].vt		= VT_I4;
			l_args[1].intVal	= l_saveProperties ->m_format;
		}
	
		l_result = p_docDispatch ->GetIDsOfNames(
			IID_NULL, &l_methodName, 1, LOCALE_USER_DEFAULT, &l_methodId );

		THROW_IF_FAILED( l_result, OleException( "GetIDsOfNames", __FILE__, __LINE__ ) );
		
		l_result = p_docDispatch ->Invoke(
			l_methodId,
			IID_NULL,
			LOCALE_USER_DEFAULT,
			DISPATCH_METHOD,
			&l_dispParams,
			NULL,
			NULL,
			NULL );

		THROW_IF_FAILED( l_result, OleException( "ExportAsFixedFormat", __FILE__, __LINE__ ) );

		SysFreeString( l_fileName );
	}

	catch ( ... )
	{
		if ( l_fileName != NULL )
			SysFreeString( l_fileName );

		throw;
	}
} // saveDocAs

void MsExcelUtil::printDoc( IDispatch* p_appDispatch, IDispatch* p_docDispatch, const string& p_printerName, int p_copyCnt )
{
	VARIANT		l_args[5];
	DISPPARAMS	l_dispParams	= { l_args, NULL, 5, 0 };
	BSTR		l_printerName	= NULL;
	OLECHAR*	l_methodName;
	DISPID		l_methodId;
	HRESULT		l_result;

	try
	{
		l_methodName = L"PrintOut";
		l_result	 = p_docDispatch ->GetIDsOfNames(
			IID_NULL, &l_methodName, 1, LOCALE_USER_DEFAULT, &l_methodId );

		THROW_IF_FAILED( l_result, OleException( "GetIDsOfNames", __FILE__, __LINE__ ) );

		l_printerName		= SysAllocString( wstring( p_printerName.begin(), p_printerName.end() ).c_str() );
		l_args[0].vt		= VT_BSTR;
		l_args[0].bstrVal	= l_printerName;

		l_args[1].vt		= VT_BOOL;
		l_args[1].boolVal	= false;

		l_args[2].vt		= VT_I4;
		l_args[2].intVal	= p_copyCnt;

		l_args[3].vt		= VT_NULL;
		l_args[4].vt		= VT_NULL;
	
		l_result = p_docDispatch ->Invoke(
			l_methodId,
			IID_NULL,
			LOCALE_USER_DEFAULT,
			DISPATCH_METHOD,
			&l_dispParams,
			NULL,
			NULL,
			NULL );

		THROW_IF_FAILED( l_result, OleException( "PrintOut", __FILE__, __LINE__ ) );
		SysFreeString( l_printerName );
	}

	catch ( ... )
	{
		if ( l_printerName != NULL )
			SysFreeString( l_printerName );

		throw;
	}
} // printDoc



void MsExcelUtil::initializeGrades()
{
	double l_appVersion = getAppVersion();
	double l_appBuild	= getAppBuild();

	if ( MathFunc::cmpGe( l_appVersion, 14.0 ) )
	{
		insertImportGrade( ".xlsx", 1 );
		insertImportGrade( ".xlsm", 1 );
		insertImportGrade( ".xltx", 1 );
		insertImportGrade( ".xltm", 1 );

		insertImportGrade( ".odf", 3 );
		insertImportGrade( ".ods", 3 );

		insertExportGrade( ".xlsx", 1 );
		insertExportGrade( ".xlsm", 1 );
		insertExportGrade( ".xltx", 1 );
		insertExportGrade( ".xltm", 1 );

		insertExportGrade( ".pdf", 1 );
		insertExportGrade( ".xps", 1 );
	}

	else
	if ( MathFunc::cmpGe( l_appVersion, 12.0 ) )
	{
		insertImportGrade( ".xlsx", 1 );
		insertImportGrade( ".xlsm", 1 );
		insertImportGrade( ".xltx", 1 );
		insertImportGrade( ".xltm", 1 );

		insertImportGrade( ".odf", 3 );
		insertImportGrade( ".ods", 3 );

		insertExportGrade( ".xlsx", 1 );
		insertExportGrade( ".xlsm", 1 );
		insertExportGrade( ".xltx", 1 );
		insertExportGrade( ".xltm", 1 );

		if ( MathFunc::cmpGe( l_appBuild, 6425.0 ) || isPdfExportInstalled() )
		{
			insertExportGrade( ".pdf", 1 );
			insertExportGrade( ".xps", 1 );
		}
	}

	else
	if ( MathFunc::cmpGe( l_appVersion, 9.0 ) )
	{
		if ( isCompatibilityPackInstalled() )
		{
			insertImportGrade( ".xlsx", 1 );
			insertImportGrade( ".xlsm", 1 );
			insertImportGrade( ".xltx", 1 );
			insertImportGrade( ".xltm", 1 );

			insertExportGrade( ".xlsx", 1 );
			insertExportGrade( ".xlsm", 1 );
			insertExportGrade( ".xltx", 1 );
			insertExportGrade( ".xltm", 1 );
		}
	}

	else
	{
		throw UtilNotFoundException( __FILE__, __LINE__ );
	}

	insertImportGrade( ".xls", 1 );
	insertImportGrade( ".xlt", 1 );
	insertImportGrade( ".htm", 1 );
	insertImportGrade( ".html", 1 );
	insertImportGrade( ".mht", 1 );
	insertImportGrade( ".mhtml", 1 );
	insertImportGrade( ".csv", 1 );
	insertImportGrade( ".slk", 1 );
	insertImportGrade( ".dbf", 1 );
	insertImportGrade( ".dif", 1 );

	insertExportGrade( ".xls", 1 );
	insertExportGrade( ".xlt", 1 );
	insertExportGrade( ".htm", 1 );
	insertExportGrade( ".html", 1 );
	insertExportGrade( ".csv", 1 );
	insertExportGrade( ".slk", 1 );
	insertExportGrade( ".dif", 1 );
} // initializeGrades

void MsExcelUtil::initializeSaveProperties()
{
	insertSaveExt( ".xlsx", "SaveAs", 51 );
	insertSaveExt( ".xlsm", "SaveAs", 52 );
	insertSaveExt( ".xltx", "SaveAs", 54 );
	insertSaveExt( ".xltm", "SaveAs", 53 );

	insertSaveExt( ".xls", "SaveAs", 56 );
	insertSaveExt( ".xlt", "SaveAs", 17 );
	insertSaveExt( ".htm", "SaveAs", 44 );
	insertSaveExt( ".html", "SaveAs", 44 );
	insertSaveExt( ".csv", "SaveAs", 23 );
	insertSaveExt( ".slk", "SaveAs", 2 );
	insertSaveExt( ".dif", "SaveAs", 9 );

	insertSaveExt( ".pdf", "ExportAsFixedFormat", 0 );
	insertSaveExt( ".xps", "ExportAsFixedFormat", 1 );
} // initializeSaveProperties
