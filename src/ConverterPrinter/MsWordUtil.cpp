
#include "DocxFactory/ConverterPrinter/MsOfficeUtil.h"
#include "DocxFactory/ConverterPrinter/MsWordUtil.h"
#include "DocxFactory/ConverterPrinter/ConverterPrinter.h"
#include "DocxFactory/ConverterPrinter/ConverterPrinterException.h"
#include "DocxFactory/ConverterPrinter/ExportFormatNotSupportedException.h"
#include "DocxFactory/ConverterPrinter/UtilNotFoundException.h"

#include "DocxFactory/os/OsFunc.h"
#include "DocxFactory/math/MathFunc.h"

using namespace DocxFactory;
using namespace std;



MsWordUtil::MsWordUtil() : MsOfficeUtil( "Word.Application", "Documents" )
{
	initializeGrades();
	initializeSaveProperties();
} // c'tor

MsWordUtil::~MsWordUtil()
{

} // d'tor



IDispatch* MsWordUtil::openDoc( IDispatch* p_appDispatch, const string& p_fileName )
{
	IDispatch* l_docDispatch = MsOfficeUtil::openDoc( p_appDispatch, p_fileName );

	if ( ConverterPrinter::getUpdateToc() )
		updateToc( l_docDispatch );

	return l_docDispatch;
} // openDoc

void MsWordUtil::saveDocAs( IDispatch* p_docDispatch, const string& p_fileName )
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

		if( l_saveProperties ->m_methodName == "SaveAs" )
			l_methodName = L"SaveAs";

		else
		if( l_saveProperties ->m_methodName == "ExportAsFixedFormat" )
			l_methodName = L"ExportAsFixedFormat";
	
		l_result = p_docDispatch ->GetIDsOfNames(
			IID_NULL, &l_methodName, 1, LOCALE_USER_DEFAULT, &l_methodId );

		THROW_IF_FAILED( l_result, OleException( "GetIDsOfNames", __FILE__, __LINE__ ) );
		
		l_fileName			= SysAllocString( wstring( p_fileName.begin(), p_fileName.end() ).c_str() );
		l_args[0].vt		= VT_I4;
		l_args[0].intVal	= l_saveProperties ->m_format;
		l_args[1].vt		= VT_BSTR;
		l_args[1].bstrVal	= l_fileName;

		l_result = p_docDispatch ->Invoke(
			l_methodId,
			IID_NULL,
			LOCALE_USER_DEFAULT,
			DISPATCH_METHOD,
			&l_dispParams,
			NULL,
			NULL,
			NULL );

		THROW_IF_FAILED( l_result, OleException( "ExportAsFixedFormat / Save", __FILE__, __LINE__ ) );

		SysFreeString( l_fileName );
	}

	catch ( ... )
	{
		if ( l_fileName != NULL )
			SysFreeString( l_fileName );

		throw;
	}
} // saveDocAs

void MsWordUtil::printDoc( IDispatch* p_appDispatch, IDispatch* p_docDispatch, const string& p_printerName, int p_copyCnt )
{
	VARIANT		l_setPrinterArgs;
	VARIANT		l_printArgs[8];
	DISPPARAMS	l_dispParams	= { NULL, NULL, 0, 0 };
	BSTR		l_printerName	= NULL;
	OLECHAR*	l_methodName;
	DISPID		l_methodId;
	HRESULT		l_result;

	try
	{
		l_methodName = L"ActivePrinter";
		l_result	 = p_appDispatch ->GetIDsOfNames(
			IID_NULL, &l_methodName, 1, LOCALE_USER_DEFAULT, &l_methodId );

		THROW_IF_FAILED( l_result, OleException( "GetIDsOfNames", __FILE__, __LINE__ ) );

		l_printerName				= SysAllocString( wstring( p_printerName.begin(), p_printerName.end() ).c_str() );
		l_setPrinterArgs.vt			= VT_BSTR;
		l_setPrinterArgs.bstrVal	= l_printerName;
		l_dispParams.rgvarg			= &l_setPrinterArgs;
		l_dispParams.cArgs			= 1;

		l_result = p_appDispatch ->Invoke(
			l_methodId,
			IID_NULL,
			LOCALE_USER_DEFAULT,
			DISPATCH_PROPERTYPUT,
			&l_dispParams,
			NULL,
			NULL,
			NULL );

		THROW_IF_FAILED( l_result, OleException( "ActivePrinter", __FILE__, __LINE__ ) );

		l_methodName = L"PrintOut";
		l_result	 = p_docDispatch ->GetIDsOfNames(
			IID_NULL, &l_methodName, 1, LOCALE_USER_DEFAULT, &l_methodId );

		THROW_IF_FAILED( l_result, OleException( "GetIDsOfNames", __FILE__, __LINE__ ) );

		l_printArgs[0].vt		= VT_I4;
		l_printArgs[0].intVal	= p_copyCnt;

		l_printArgs[1].vt		= VT_I4;
		l_printArgs[1].intVal	= 0;

		l_printArgs[2].vt		= VT_BSTR;
		l_printArgs[2].bstrVal	= NULL;

		l_printArgs[3].vt		= VT_BSTR;
		l_printArgs[3].bstrVal	= NULL;

		l_printArgs[4].vt		= VT_BSTR;
		l_printArgs[4].bstrVal	= NULL;

		l_printArgs[5].vt		= VT_I4;
		l_printArgs[5].intVal	= 0;

		l_printArgs[6].vt		= VT_BOOL;
		l_printArgs[6].boolVal	= true;

		l_printArgs[7].vt		= VT_BOOL;
		l_printArgs[7].boolVal	= false;

		l_dispParams.rgvarg	= l_printArgs;
		l_dispParams.cArgs	= 8;

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
		// restore active printer ?
	}

	catch ( ... )
	{
		if ( l_printerName != NULL )
			SysFreeString( l_printerName );

		throw;
	}
} // printDoc



void MsWordUtil::updateToc( IDispatch* p_docDispatch )
{
	IDispatch*	l_tablesDispatch	= NULL;
	IDispatch*	l_tableDispatch		= NULL;

	DISPPARAMS	l_dispParams = { NULL, NULL, 0, 0 };
	VARIANT		l_args;
	VARIANT		l_argsOut;
	OLECHAR*	l_methodName;
	DISPID		l_methodId;
	HRESULT		l_result;
	int			l_count;

	try
	{
		l_methodName = L"TablesOfContents";
		l_result	 = p_docDispatch ->GetIDsOfNames(
			IID_NULL,
			&l_methodName,
			1,
			LOCALE_USER_DEFAULT,
			&l_methodId );

		THROW_IF_FAILED( l_result, OleException( "GetIDsOfNames", __FILE__, __LINE__ ) );

		l_result = p_docDispatch ->Invoke(
			l_methodId,
			IID_NULL,
			LOCALE_SYSTEM_DEFAULT,
			DISPATCH_PROPERTYGET,
			&l_dispParams,
			&l_argsOut,
			NULL,
			NULL );

		THROW_IF_FAILED( l_result, OleException( "TablesOfContents", __FILE__, __LINE__ ) );
		l_tablesDispatch = l_argsOut.pdispVal;

		l_methodName = L"Count";
		l_result	 = l_tablesDispatch ->GetIDsOfNames(
			IID_NULL,
			&l_methodName,
			1,
			LOCALE_USER_DEFAULT,
			&l_methodId );

		THROW_IF_FAILED( l_result, OleException( "GetIDsOfNames", __FILE__, __LINE__ ) );

		l_result = l_tablesDispatch ->Invoke(
			l_methodId,
			IID_NULL,
			LOCALE_SYSTEM_DEFAULT,
			DISPATCH_PROPERTYGET,
			&l_dispParams,
			&l_argsOut,
			NULL,
			NULL );

		THROW_IF_FAILED( l_result, OleException( "Count", __FILE__, __LINE__ ) );

		l_count = l_argsOut.iVal;
		if ( l_count > 0 )
		{
			l_methodName = L"Item";
			l_result	 = l_tablesDispatch ->GetIDsOfNames(
				IID_NULL,
				&l_methodName,
				1,
				LOCALE_USER_DEFAULT,
				&l_methodId );

			THROW_IF_FAILED( l_result, OleException( "GetIDsOfNames", __FILE__, __LINE__ ) );

			l_args.vt			= VT_I4;
			l_args.intVal		= 1;
			l_dispParams.rgvarg	= &l_args;
			l_dispParams.cArgs	= 1;

			l_result = l_tablesDispatch ->Invoke(
				l_methodId,
				IID_NULL,
				LOCALE_SYSTEM_DEFAULT,
				DISPATCH_METHOD,
				&l_dispParams,
				&l_argsOut,
				NULL,
				NULL );

			THROW_IF_FAILED( l_result, OleException( "Item", __FILE__, __LINE__ ) );
			l_tableDispatch = l_argsOut.pdispVal;

			l_methodName = L"Update";
			l_result	 = l_tableDispatch ->GetIDsOfNames(
				IID_NULL,
				&l_methodName,
				1,
				LOCALE_USER_DEFAULT,
				&l_methodId );

			THROW_IF_FAILED( l_result, OleException( "GetIDsOfNames", __FILE__, __LINE__ ) );

			l_dispParams.rgvarg	= NULL;
			l_dispParams.cArgs	= 0;

			l_result = l_tableDispatch ->Invoke(
				l_methodId,
				IID_NULL,
				LOCALE_SYSTEM_DEFAULT,
				DISPATCH_METHOD,
				&l_dispParams,
				NULL,
				NULL,
				NULL );

			THROW_IF_FAILED( l_result, OleException( "Update", __FILE__, __LINE__ ) );

			l_tableDispatch ->Release();
		}

		l_tablesDispatch ->Release();
	}

	catch ( ... )
	{
		if( l_tableDispatch )
			l_tableDispatch ->Release();

		if( l_tablesDispatch )
			l_tablesDispatch ->Release();

		throw;
	}
} // updateToc



void MsWordUtil::initializeGrades()
{
	double l_appVersion = getAppVersion();
	double l_appBuild	= getAppBuild();

	if ( MathFunc::cmpGe( l_appVersion, 14.0 ) )
	{
		insertImportGrade( ".docx", 1 );
		insertImportGrade( ".docm", 1 );
		insertImportGrade( ".dotx", 1 );
		insertImportGrade( ".dotm", 1 );

		insertImportGrade( ".odf", 3 );
		insertImportGrade( ".odt", 3 );

		insertExportGrade( ".docx", 1 );
		insertExportGrade( ".docm", 1 );
		insertExportGrade( ".dotx", 1 );
		insertExportGrade( ".dotm", 1 );
		insertExportGrade( ".pdf", 1 );
		insertExportGrade( ".xps", 1 );
	}

	else
	if ( MathFunc::cmpGe( l_appVersion, 12.0 ) )
	{
		insertImportGrade( ".docx", 1 );
		insertImportGrade( ".docm", 1 );
		insertImportGrade( ".dotx", 1 );
		insertImportGrade( ".dotm", 1 );

		insertImportGrade( ".odf", 3 );
		insertImportGrade( ".odt", 3 );

		insertExportGrade( ".docx", 1 );
		insertExportGrade( ".docm", 1 );
		insertExportGrade( ".dotx", 1 );
		insertExportGrade( ".dotm", 1 );
		
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
			insertImportGrade( ".docx", 1 );
			insertImportGrade( ".docm", 1 );
			insertImportGrade( ".dotx", 1 );
			insertImportGrade( ".dotm", 1 );

			insertExportGrade( ".docx", 1 );
			insertExportGrade( ".docm", 1 );
			insertExportGrade( ".dotx", 1 );
			insertExportGrade( ".dotm", 1 );
		}
	}

	else
	{
		throw UtilNotFoundException( __FILE__, __LINE__ );
	}

	insertImportGrade( ".doc", 1 );
	insertImportGrade( ".dot", 1 );
	insertImportGrade( ".htm", 1 );
	insertImportGrade( ".html", 1 );
	insertImportGrade( ".mht", 1 );
	insertImportGrade( ".mhtml", 1 );
	insertImportGrade( ".rtf", 1 );
	insertImportGrade( ".txt", 1 );
	insertImportGrade( ".wpd", 1 );
	insertImportGrade( ".wps", 1 );

	insertExportGrade( ".doc", 1 );
	insertExportGrade( ".dot", 1 );
	insertExportGrade( ".htm", 1 );
	insertExportGrade( ".html", 1 );
	insertExportGrade( ".rtf", 1 );
	insertExportGrade( ".txt", 1 );
} // initializeGrades

void MsWordUtil::initializeSaveProperties()
{
	insertSaveExt( ".docx", "SaveAs", 12 );
	insertSaveExt( ".docm", "SaveAs", 13 );
	insertSaveExt( ".dotx", "SaveAs", 14 );
	insertSaveExt( ".dotm", "SaveAs", 15 );

	insertSaveExt( ".doc", "SaveAs", 0 );
	insertSaveExt( ".dot", "SaveAs", 1 );
	insertSaveExt( ".htm", "SaveAs", 8 );
	insertSaveExt( ".html", "SaveAs", 8 );
	insertSaveExt( ".rtf", "SaveAs", 6 );
	insertSaveExt( ".txt", "SaveAs", 7 );

	insertSaveExt( ".pdf", "ExportAsFixedFormat", 17 );
	insertSaveExt( ".xps", "ExportAsFixedFormat", 18 );
} // initializeSaveProperties
