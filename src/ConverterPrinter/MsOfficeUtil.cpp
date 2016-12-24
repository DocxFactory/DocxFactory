
#include "DocxFactory/ConverterPrinter/MsOfficeUtil.h"
#include "DocxFactory/ConverterPrinter/ConverterPrinterException.h"
#include "DocxFactory/ConverterPrinter/GradeGroup.h"
#include "DocxFactory/ConverterPrinter/UtilNotFoundException.h"

#include "DocxFactory/os/OsFunc.h"
#include "DocxFactory/win/WinFunc.h"
#include "DocxFactory/str/StrFunc.h"

#include <ShlObj.h>

using namespace DocxFactory;
using namespace std;



MsOfficeUtil::MsOfficeUtil( const string& p_appName, const string& p_preOpenMethodName )

	: ConverterPrinterUtil(), m_preOpenMethodName( NULL ), m_gradeGroup( NULL )
{
	IDispatch* l_appDispatch;
	
	try
	{
		wstring	l_preOpenMethodName( p_preOpenMethodName.begin(), p_preOpenMethodName.end() );
		size_t	l_preOpenMethodNameLen = l_preOpenMethodName.length();

		initializeAppId( p_appName );

		m_preOpenMethodName = new OLECHAR[ l_preOpenMethodNameLen + 1 ];
		l_preOpenMethodName.copy( m_preOpenMethodName, l_preOpenMethodName.length() );
		m_preOpenMethodName[ l_preOpenMethodNameLen ] = 0;

		l_appDispatch = connect();
		initializeAppVersion( l_appDispatch );
		l_appDispatch ->Release();

		m_gradeGroup = new GradeGroup();
	}

	catch ( ... )
	{
		delete[] m_preOpenMethodName;
		delete m_gradeGroup;

		throw;
	}
} // c'tor

MsOfficeUtil::~MsOfficeUtil()
{
	delete[] m_preOpenMethodName;
	delete m_gradeGroup;
} // d'tor



void MsOfficeUtil::saveAs( const string& p_importFile, const string& p_exportFile )
{
	IDispatch* l_appDispatch = NULL;
	IDispatch* l_docDispatch = NULL;

	try
	{
		l_appDispatch = connect();
		l_docDispatch = openDoc( l_appDispatch, p_importFile );
		
		saveDocAs( l_docDispatch, p_exportFile );
		closeDoc( l_docDispatch );

		l_appDispatch ->Release();
	}

	catch ( ... )
	{
		if( l_docDispatch != NULL )
			closeDoc( l_docDispatch );
		
		if( l_appDispatch != NULL )
			l_appDispatch ->Release();
		
		throw;
	}
} // saveAs

void MsOfficeUtil::print( const string& p_fileName, const string& p_printerName, int p_copyCnt )
{
	IDispatch* l_appDispatch = NULL;
	IDispatch* l_docDispatch = NULL;

	try
	{
		l_appDispatch = connect();
		l_docDispatch = openDoc( l_appDispatch, p_fileName );
		
		printDoc( l_appDispatch, l_docDispatch, p_printerName, p_copyCnt );
		closeDoc( l_docDispatch );

		l_appDispatch ->Release();
	}

	catch ( ... )
	{
		if( l_docDispatch != NULL )
			closeDoc( l_docDispatch );

		if( l_appDispatch != NULL )
			l_appDispatch ->Release();
		
		throw;
	}
} // print



double MsOfficeUtil::getConversionGrade( const string& p_importExt, const string& p_exportExt )
{
	double l_importGrade = m_gradeGroup ->getImportGrade( p_importExt );
	double l_exportGrade = m_gradeGroup ->getExportGrade( p_exportExt );

	if ( l_importGrade > 0.0f && l_exportGrade > 0.0f )
		return ( l_importGrade + l_exportGrade ) / 2.0f;

	else
		return -1.0f;
} // getConversionGrade

double MsOfficeUtil::getPrintingGrade( const string& p_ext )
{
	return m_gradeGroup ->getImportGrade( p_ext );
} // getPrintingGrade



IDispatch* MsOfficeUtil::connect()
{
	IDispatch*	l_appDispatch	= NULL;
	IUnknown*	l_unknown		= NULL;
	HRESULT		l_result;

	try
	{
		// connect to an existing instance
		l_result = GetActiveObject( m_appId, NULL, ( IUnknown** ) &l_unknown );
		THROW_IF_FAILED( l_result, OleException( "GetActiveObject", __FILE__, __LINE__ ) );

		l_result = l_unknown ->QueryInterface( IID_IDispatch, ( void ** ) &l_appDispatch );
		THROW_IF_FAILED( l_result, OleException( "QueryInterface", __FILE__, __LINE__ ) );

		l_unknown ->Release();
	}

	catch ( ... )
	{
		// create a new instance
		if( l_appDispatch != NULL )
			l_appDispatch ->Release();
		
		if( l_unknown != NULL )
			l_unknown ->Release();

		l_result = CoCreateInstance(
			m_appId,
			NULL,
			CLSCTX_LOCAL_SERVER,
			IID_IDispatch,
			( void ** ) &l_appDispatch );

		THROW_IF_FAILED( l_result, OleException( "CoCreateInstance", __FILE__, __LINE__ ) );

		setVisible( l_appDispatch, false );
	}

	return l_appDispatch;
} // connect



IDispatch* MsOfficeUtil::openDoc( IDispatch* p_appDispatch, const string& p_fileName )
{
	IDispatch*	l_docDispatch;
	IDispatch*	l_docsDispatch;
	OLECHAR*	l_methodName;
	DISPID		l_methodId;
	VARIANT		l_args;
	VARIANT		l_argsOut;
	DISPPARAMS	l_dispParams	= { NULL, NULL, 0, 0 };
	BSTR		l_fileName		= NULL;
	HRESULT		l_result;

	try
	{
		l_result = p_appDispatch ->GetIDsOfNames(
			IID_NULL,
			&m_preOpenMethodName,
			1,
			LOCALE_USER_DEFAULT,
			&l_methodId );

		THROW_IF_FAILED( l_result, OleException( "GetIDsOfNames", __FILE__, __LINE__ ) );

		l_result = p_appDispatch ->Invoke(
			l_methodId,
			IID_NULL,
			LOCALE_SYSTEM_DEFAULT,
			DISPATCH_PROPERTYGET,
			&l_dispParams,
			&l_argsOut,
			NULL,
			NULL );

		THROW_IF_FAILED( l_result, OleException( "pre open method", __FILE__, __LINE__ ) );
		l_docsDispatch = l_argsOut.pdispVal;
	
		l_methodName = L"Open";
		l_result	 = l_docsDispatch ->GetIDsOfNames(
			IID_NULL,
			&l_methodName,
			1,
			LOCALE_USER_DEFAULT,
			&l_methodId );

		THROW_IF_FAILED( l_result, OleException( "GetIDsOfNames", __FILE__, __LINE__ ) );

		l_fileName			= SysAllocString( wstring( p_fileName.begin(), p_fileName.end() ).c_str() );
		l_args.vt			= VT_BSTR;
		l_args.bstrVal		= l_fileName;
		l_dispParams.rgvarg	= &l_args;
		l_dispParams.cArgs	= 1;

		l_result = l_docsDispatch ->Invoke(
			l_methodId,
			IID_NULL,
			LOCALE_SYSTEM_DEFAULT,
			DISPATCH_METHOD,
			&l_dispParams,
			&l_argsOut,
			NULL,
			NULL );

		THROW_IF_FAILED( l_result, OleException( "Open", __FILE__, __LINE__ ) );

		l_docDispatch = l_argsOut.pdispVal;
		l_docsDispatch ->Release();
		SysFreeString( l_fileName );
	}
	
	catch ( ... )
	{
		if ( l_fileName != NULL )
			SysFreeString( l_fileName );

		throw;
	}

	return l_docDispatch;
} // openDoc

void MsOfficeUtil::closeDoc( IDispatch* p_docDispatch )
{
	DISPPARAMS	l_dispParams	= { NULL, NULL, 0, 0 };
	OLECHAR*	l_methodName	= L"Close";
	DISPID		l_methodId;
	HRESULT		l_result;

	try
	{
		l_result = p_docDispatch ->GetIDsOfNames(
			IID_NULL,
			&l_methodName,
			1,
			LOCALE_USER_DEFAULT,
			&l_methodId );

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

		THROW_IF_FAILED( l_result, OleException( "Close", __FILE__, __LINE__ ) );

		p_docDispatch ->Release();
	}

	catch ( ... )
	{
		p_docDispatch ->Release();
		throw;
	}
} // closeDoc

void MsOfficeUtil::setVisible( IDispatch* p_appDispatch, bool p_visible )
{
	HRESULT		l_result;
	OLECHAR*	l_methodName;
	DISPID		l_methodId;

	VARIANT		l_args;
	DISPID		l_propertyPut	= DISPID_PROPERTYPUT;
	DISPPARAMS	l_dispParams	= { &l_args, &l_propertyPut, 1, 1 };

	l_methodName	= L"Visible";
	l_result		= p_appDispatch ->GetIDsOfNames( IID_NULL, &l_methodName, 1, LOCALE_USER_DEFAULT, &l_methodId );

	THROW_IF_FAILED( l_result, OleException( "GetIDsOfNames", __FILE__, __LINE__ ) );

	l_args.vt		= VT_BOOL;
	l_args.boolVal	= p_visible;

	l_result = p_appDispatch ->Invoke(
		l_methodId,
		IID_NULL,
		LOCALE_USER_DEFAULT,
		DISPATCH_PROPERTYPUT,
		&l_dispParams,
		NULL,
		NULL,
		NULL );

	THROW_IF_FAILED( l_result, OleException( "Visible", __FILE__, __LINE__ ) );
	
	l_methodName	= L"DisplayAlerts";
	l_result		= p_appDispatch ->GetIDsOfNames( IID_NULL, &l_methodName, 1, LOCALE_USER_DEFAULT, &l_methodId );

	THROW_IF_FAILED( l_result, OleException( "GetIDsOfNames", __FILE__, __LINE__ ) );

	l_args.vt		= VT_BOOL;
	l_args.boolVal	= p_visible;

	l_result = p_appDispatch ->Invoke(
		l_methodId,
		IID_NULL,
		LOCALE_USER_DEFAULT,
		DISPATCH_PROPERTYPUT,
		&l_dispParams,
		NULL,
		NULL,
		NULL );

	THROW_IF_FAILED( l_result, OleException( "DisplayAlerts", __FILE__, __LINE__ ) );
} // setVisible



void MsOfficeUtil::insertImportGrade( const string& p_ext, double p_grade )
{
	m_gradeGroup ->insertImportGrade( p_ext, p_grade );
} // insertImportGrade

void MsOfficeUtil::insertExportGrade( const string& p_ext, double p_grade )
{
	m_gradeGroup ->insertExportGrade( p_ext, p_grade );
} // insertExportGrade

void MsOfficeUtil::insertSaveExt( const string& p_ext, const string& p_methodName, int p_format )
{
	map<string, SaveProperties*>::iterator	l_savePropertiesIterator = m_savePropertiesByExt.find( p_ext );
	SaveProperties*							l_saveProperties;

	if( l_savePropertiesIterator == m_savePropertiesByExt.end() )
	{
		l_saveProperties = new SaveProperties();
		m_savePropertiesByExt[ p_ext ] = l_saveProperties;
	}

	else
		l_saveProperties = l_savePropertiesIterator ->second;

	l_saveProperties ->m_methodName	= p_methodName;
	l_saveProperties ->m_format		= p_format;
} // insertSaveExt

const SaveProperties* MsOfficeUtil::getSavePropertiesByExt( const string& p_ext ) const
{
	map<string, SaveProperties*>::const_iterator	l_savePropertiesIterator = m_savePropertiesByExt.find( p_ext );
	const SaveProperties*							l_saveProperties;

	if( l_savePropertiesIterator == m_savePropertiesByExt.end() )
		l_saveProperties = NULL;

	else
		l_saveProperties = l_savePropertiesIterator ->second;

	return l_saveProperties;
} // getSavePropertiesByExt



double MsOfficeUtil::getAppVersion() const
{
	return m_appVersion;
} // getAppVersion

double MsOfficeUtil::getAppBuild() const
{
	return m_appBuild;
} // getAppBuild

bool MsOfficeUtil::isCompatibilityPackInstalled() const
{
	return m_compatibilityPackInstalled;
} // isCompatibilityPackInstalled

bool MsOfficeUtil::isPdfExportInstalled() const
{
	return m_pdfExportInstalled;
} // isPdfExportInstalled



void MsOfficeUtil::initializeAppId( const string& p_appName )
{
	wstring	l_appName( p_appName.begin(), p_appName.end() );
	HRESULT l_result;
	
	CoInitialize( NULL );
	l_result = CLSIDFromProgID( l_appName.c_str(), &m_appId );

	THROW_IF_FAILED( l_result, OleException( "CLSIDFromProgID", __FILE__, __LINE__ ) );
} // initializeAppId

void MsOfficeUtil::initializeAppVersion( IDispatch* p_appDispatch )
{
	DISPPARAMS	l_dispParams = { NULL, NULL, 0, 0 };
	OLECHAR*	l_methodName;
	DISPID		l_methodId;
	VARIANT		l_outVariant;
	HRESULT		l_result;

	wstring		l_wStr;
	string		l_str;

	l_methodName = L"Version";
	l_result	 = p_appDispatch ->GetIDsOfNames(
		IID_NULL,
		&l_methodName,
		1,
		LOCALE_USER_DEFAULT,
		&l_methodId );

	THROW_IF_FAILED( l_result, OleException( "GetIDsOfNames", __FILE__, __LINE__ ) );

	l_result = p_appDispatch ->Invoke(
		l_methodId,
		IID_NULL,
		LOCALE_USER_DEFAULT,
		DISPATCH_PROPERTYGET,
		&l_dispParams,
		&l_outVariant,
		NULL,
		NULL );

	THROW_IF_FAILED( l_result, OleException( "Version", __FILE__, __LINE__ ) );

	l_wStr			= l_outVariant.bstrVal;
	l_str			= string( l_wStr.begin(), l_wStr.end() );
	m_appVersion	= StrFunc::strToInt<double>( l_str );
	
	l_methodName = L"Build";
	l_result	 = p_appDispatch ->GetIDsOfNames(
		IID_NULL,
		&l_methodName,
		1,
		LOCALE_USER_DEFAULT,
		&l_methodId );

	THROW_IF_FAILED( l_result, OleException( "GetIDsOfNames", __FILE__, __LINE__ ) );

	l_result = p_appDispatch ->Invoke(
		l_methodId,
		IID_NULL,
		LOCALE_USER_DEFAULT,
		DISPATCH_PROPERTYGET,
		&l_dispParams,
		&l_outVariant,
		NULL,
		NULL );

	THROW_IF_FAILED( l_result, OleException( "Build", __FILE__, __LINE__ ) );
	m_appBuild = l_outVariant.dblVal;

	m_compatibilityPackInstalled = OsFunc::isFileExists(
		WinFunc::getFolderPath( CSIDL_PROGRAM_FILESX86 ) + "\\moc.exe" );

	m_pdfExportInstalled = 
		OsFunc::isFileExists( WinFunc::getFolderPath( CSIDL_PROGRAM_FILES_COMMONX86 ) + "\\microsoft shared\\OFFICE12\\EXP_PDF.DLL" ) &&
		OsFunc::isFileExists( WinFunc::getFolderPath( CSIDL_PROGRAM_FILES_COMMONX86 ) + "\\microsoft shared\\OFFICE12\\EXP_XPS.DLL" );
} // initializeAppVersion
