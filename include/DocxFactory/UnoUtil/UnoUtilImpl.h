
#ifndef __DOCXFACTORY_UNO_UTIL_IMPL_H__
#define __DOCXFACTORY_UNO_UTIL_IMPL_H__

#include "DocxFactory/UnoUtil/UnoUtil.h"

#include <sal/main.h>
#include <cppuhelper/bootstrap.hxx>
#include <rtl/bootstrap.hxx>
#include <rtl/process.h>
#include <osl/process.h>
#include <osl/file.hxx>

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/view/XPrintable.hpp>
#include <com/sun/star/view/PaperFormat.hpp>
#include <com/sun/star/view/PaperOrientation.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/registry/XSimpleRegistry.hpp>

#include <string>



#if defined( __unix__ ) || defined( SWIG )
	#define DOCXFACTORY_DLL
#else
	#ifdef  DOCXFACTORY_DLL_EXPORT
	#define DOCXFACTORY_DLL __declspec(dllexport)
	#else
	#define DOCXFACTORY_DLL __declspec(dllimport)
	#endif
#endif



using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::bridge;
using namespace com::sun::star::frame;
using namespace com::sun::star::registry;
using namespace com::sun::star::view;
using namespace com::sun::star::text;
using namespace std;

using ::rtl::OUString;
using ::rtl::OUStringToOString;



namespace DocxFactory
{
	class UnoUtilImpl : public UnoUtil
	{
	public:
		DOCXFACTORY_DLL UnoUtilImpl();
		DOCXFACTORY_DLL virtual ~UnoUtilImpl();

		DOCXFACTORY_DLL virtual void saveAs	( const string& p_importFile,	const string& p_exportFile );
		DOCXFACTORY_DLL virtual void print	( const string& p_fileName,		const string& p_printerName, int p_copyCnt );

	protected:

	private:
		UnoUtilImpl( const UnoUtilImpl& p_other );
		UnoUtilImpl& operator = ( const UnoUtilImpl& p_other );

		Reference<XComponentContext> connect();
		Reference<XComponent> openDoc( Reference<XComponentContext> p_xContext, const string& p_fileName );

		void	closeDoc	( Reference<XComponent> p_xComponentDoc );
		void	saveDocAs	( Reference<XComponent> p_xComponentDoc, const string& p_fileName );
		void	printDoc	( Reference<XComponent> p_xComponentDoc, const string& p_printerName, int p_copyCnt );
		string	detectFamily( Reference<XComponent> p_xComponentDoc ); 

		OUString fileNameToAbsoluteFileUrl( const string& p_fileName );

	};
};

#ifdef DOCXFACTORY_DLL
#undef DOCXFACTORY_DLL
#endif
#endif
