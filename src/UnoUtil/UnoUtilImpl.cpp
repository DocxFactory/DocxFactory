
#define DOCXFACTORY_DLL_EXPORT
#include "DocxFactory/UnoUtil/UnoUtilImpl.h"
#undef  DOCXFACTORY_DLL_EXPORT

#include "DocxFactory/UnoUtil/PropertyMap.h"
#include "DocxFactory/UnoUtil/UnoException.h"

#include "DocxFactory/os/OsFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

using namespace DocxFactory;
using namespace std;



static bool			s_propertiesInitialized	= false;
static PropertyMap	s_properties;
static void			initializeProperties();



extern "C"
{
	UnoUtil* createUnoUtil()
	{
		UnoUtil* l_util;

		try
		{
			l_util = new UnoUtilImpl();
		}

		catch ( ... )
		{
			l_util = NULL;
		}

		return l_util;
	} // createUnoUtil

	void destroyUnoUtil( UnoUtil* p_util )
	{
		delete p_util;
	} // destroyUnoUtil
}



UnoUtilImpl::UnoUtilImpl()
{
	Reference<XComponentContext> l_xContext = connect();
	if ( !s_propertiesInitialized )
	{
		initializeProperties();
		s_propertiesInitialized = true;
	}
} // c'tor

UnoUtilImpl::~UnoUtilImpl()
{

} // d'tor



void UnoUtilImpl::saveAs( const string& p_importFile, const string& p_exportFile )
{
	Reference<XComponentContext>	l_xContext		= connect();
	Reference<XComponent>			l_xComponentDoc	= NULL;

	try
	{
		l_xComponentDoc	= openDoc( l_xContext, p_importFile );
		saveDocAs( l_xComponentDoc, p_exportFile );
		closeDoc( l_xComponentDoc );
	}

	catch ( ... )
	{
		if ( l_xComponentDoc != NULL )
			closeDoc( l_xComponentDoc );

		throw;
	}
} // saveAs

void UnoUtilImpl::print( const string& p_fileName, const string& p_printerName, int p_copyCnt )
{
	Reference<XComponentContext>	l_xContext		= connect();
	Reference<XComponent>			l_xComponentDoc	= NULL;

	try
	{
		l_xComponentDoc	= openDoc( l_xContext, p_fileName );
		printDoc( l_xComponentDoc, p_printerName, p_copyCnt );
		closeDoc( l_xComponentDoc );
	}

	catch ( ... )
	{
		if ( l_xComponentDoc != NULL )
			closeDoc( l_xComponentDoc );

		throw;
	}
} // print



Reference<XComponentContext> UnoUtilImpl::connect()
{
	try
	{
		Reference<XComponentContext> l_xContext( ::cppu::bootstrap() );
		return l_xContext;
	}

	catch ( ... )
	{
		throw UnoException( "connect", __FILE__, __LINE__ );
	}

	return NULL;
} // connect

Reference<XComponent> UnoUtilImpl::openDoc( Reference<XComponentContext> p_xContext, const string& p_fileName )
{
	try
	{
		Reference<XMultiComponentFactory> l_xServiceManager( p_xContext ->getServiceManager() );

		Reference<XComponentLoader> l_xComponentLoader(
			l_xServiceManager ->createInstanceWithContext(
				OUString::createFromAscii( "com.sun.star.frame.Desktop" ),
				p_xContext ),
			UNO_QUERY_THROW );

		OUString l_absoluteDocUrl = fileNameToAbsoluteFileUrl( p_fileName );
		Sequence < ::com::sun::star::beans::PropertyValue > l_openProperties(1);

		l_openProperties[0].Name = OUString::createFromAscii( "Hidden" );
		l_openProperties[0].Value <<= true;
	
		Reference<XComponent> l_xComponentDoc = l_xComponentLoader ->loadComponentFromURL(
			l_absoluteDocUrl,
			OUString::createFromAscii( "_blank" ),
			0,
			l_openProperties );

		return l_xComponentDoc;
	}

	catch ( ... )
	{
		throw UnoException( "openDoc", __FILE__, __LINE__ );
	}

	return NULL;
} // openDoc

void UnoUtilImpl::closeDoc( Reference<XComponent> p_xComponentDoc )
{
	try
	{
		p_xComponentDoc ->dispose();
	}

	catch ( ... )
	{
		throw UnoException( "closeDoc", __FILE__, __LINE__ );
	}
} // closeDoc

void UnoUtilImpl::saveDocAs( Reference<XComponent> p_xComponentDoc, const string& p_fileName )
{
	try
	{
		string						l_ext			= OsFunc::getSubPath( p_fileName, OsFunc::SUBPATH_EXT, OsFunc::SUBPATH_EXT );
		string						l_family		= detectFamily( p_xComponentDoc );
		const PropertyValueList*	l_properties	= s_properties.getProperties( l_family, l_ext );

		if ( l_properties == NULL )
			throw UnoException( "saveDocAs", __FILE__, __LINE__ );

		Reference<XStorable> l_xStorable( p_xComponentDoc, UNO_QUERY );
		Sequence < ::com::sun::star::beans::PropertyValue > l_convertProperties( l_properties ->size() );
		PropertyValueList::const_iterator l_propertyIterator;
		int i = 0;

		FOR_EACH( l_propertyIterator, l_properties )
		{
			l_convertProperties[i].Name = OUString::createFromAscii( l_propertyIterator ->first.c_str() );
			l_convertProperties[i].Value <<= OUString::createFromAscii( l_propertyIterator ->second.c_str() );
			++i;
		}

		OUString l_absoluteDocUrl = fileNameToAbsoluteFileUrl( p_fileName );
		l_xStorable ->storeToURL( l_absoluteDocUrl, l_convertProperties );
	}

	catch ( const DocxFactoryException& )
	{
		throw;
	}

	catch ( ... )
	{
		throw UnoException( "saveDocAs", __FILE__, __LINE__ );
	}
} // saveDocAs

void UnoUtilImpl::printDoc( Reference<XComponent> p_xComponentDoc, const string& p_printerName, int p_copyCnt )
{
	try
	{
		Reference<XPrintable> l_xPrintable( p_xComponentDoc, UNO_QUERY );
		Sequence < ::com::sun::star::beans::PropertyValue > l_printerProperties(1);
		Sequence < ::com::sun::star::beans::PropertyValue > l_printOptions(2);
	
		l_printerProperties[0].Name = OUString::createFromAscii( "Name" );
		l_printerProperties[0].Value <<= OUString::createFromAscii( p_printerName.c_str() );
		l_xPrintable ->setPrinter( l_printerProperties );
	
		l_printOptions[0].Name = OUString::createFromAscii( "Wait" );
		l_printOptions[0].Value <<= true;
		l_printOptions[1].Name = OUString::createFromAscii( "CopyCount" );
		l_printOptions[1].Value <<= ( sal_Int32 ) p_copyCnt;
	
		l_xPrintable->print( l_printOptions );
	}

	catch ( ... )
	{
		throw UnoException( "printDoc", __FILE__, __LINE__ );
	}
} // printDoc

string UnoUtilImpl::detectFamily( Reference<XComponent> p_xComponentDoc )
{
	Reference<XServiceInfo> l_xServiceInfo( p_xComponentDoc, UNO_QUERY );
	string					l_family;
	
	if ( l_xServiceInfo ->supportsService( OUString::createFromAscii( "com.sun.star.text.GenericTextDocument" ) ) )
		l_family = "Text";
	
	else if ( l_xServiceInfo ->supportsService( OUString::createFromAscii( "com.sun.star.sheet.SpreadsheetDocument" ) ) )
		l_family = "Spreadsheet";

	else
		throw UnoException( "detectFamily", __FILE__, __LINE__ );
	
	return l_family;
} // detectFamily

OUString UnoUtilImpl::fileNameToAbsoluteFileUrl( const string& p_fileName )
{
	OUString l_fileName = OUString::createFromAscii( p_fileName.c_str() );
	OUString l_absoluteDocUrl;
	OUString l_workingDir;
	OUString l_docPathUrl;
	
	osl_getProcessWorkingDir( &l_workingDir.pData );
	osl::FileBase::getFileURLFromSystemPath( l_fileName, l_docPathUrl );
	osl::FileBase::getAbsoluteFileURL( l_workingDir, l_docPathUrl, l_absoluteDocUrl );

	return l_absoluteDocUrl;
} // fileNameToAbsoluteFileUrl



static void initializeProperties()
{
	// Spreadsheet
	s_properties.setProperty( "Spreadsheet", ".xls", "FilterName", "MS Excel 97" );
	s_properties.setProperty( "Spreadsheet", ".xlt", "FilterName", "MS Excel 97 Vorlage/Template" );

	s_properties.setProperty( "Spreadsheet", ".ods", "FilterName", "calc8" );
	s_properties.setProperty( "Spreadsheet", ".sdc", "FilterName", "StarCalc 5.0" );
	s_properties.setProperty( "Spreadsheet", ".sxc", "FilterName", "StarOffice XML (Calc)" );
	s_properties.setProperty( "Spreadsheet", ".uos", "FilterName", "UOF spreadsheet" );

	s_properties.setProperty( "Spreadsheet", ".pdf", "FilterName", "calc_pdf_Export" );
	s_properties.setProperty( "Spreadsheet", ".htm", "FilterName", "HTML (StarCalc)" );
	s_properties.setProperty( "Spreadsheet", ".html", "FilterName", "HTML (StarCalc)" );
	s_properties.setProperty( "Spreadsheet", ".xhtml", "FilterName", "XHTML Calc File" );

	s_properties.setProperty( "Spreadsheet", ".dbf", "FilterName", "dBase" );
	s_properties.setProperty( "Spreadsheet", ".dif", "FilterName", "DIF" );
	s_properties.setProperty( "Spreadsheet", ".slk", "FilterName", "SYLK" );
	s_properties.setProperty( "Spreadsheet", ".csv", "FilterName", "Text - txt - csv (StarCalc)" );
	s_properties.setProperty( "Spreadsheet", ".csv", "FilterOptions", "44,34,0" );
	s_properties.setProperty( "Spreadsheet", ".tsv", "FilterName", "Text - txt - csv (StarCalc)" );
	s_properties.setProperty( "Spreadsheet", ".tsv", "FilterOptions", "9,34,0" );

	// Text
	s_properties.setProperty( "Text", ".doc", "FilterName", "MS Word 97" );
	s_properties.setProperty( "Text", ".dot", "FilterName", "MS Word 97" );

	s_properties.setProperty( "Text", ".odt", "FilterName", "writer8" );
	s_properties.setProperty( "Text", ".sdw", "FilterName", "StarWriter 5.0" );
	s_properties.setProperty( "Text", ".sxw", "FilterName", "StarOffice XML (Writer)" );
	s_properties.setProperty( "Text", ".uot", "FilterName", "UOF text" );

	s_properties.setProperty( "Text", ".pdf", "FilterName", "writer_pdf_Export" );
	s_properties.setProperty( "Text", ".htm", "FilterName", "HTML (StarWriter)" );
	s_properties.setProperty( "Text", ".html", "FilterName", "HTML (StarWriter)" );
	s_properties.setProperty( "Text", ".xhtml", "FilterName", "XHTML Writer File" );

	s_properties.setProperty( "Text", ".ltx", "FilterName", "LaTeX_Writer" );
	s_properties.setProperty( "Text", ".bib", "FilterName", "BibTeX_Writer" );
	s_properties.setProperty( "Text", ".rtf", "FilterName", "Rich Text Format" );
	s_properties.setProperty( "Text", ".txt", "FilterName", "Text" );
} // initializeProperties
