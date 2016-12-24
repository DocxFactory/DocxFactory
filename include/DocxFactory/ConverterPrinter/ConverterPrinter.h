
#ifndef __DOCXFACTORY_CONVERTER_PRINTER_H__
#define __DOCXFACTORY_CONVERTER_PRINTER_H__

#include <list>
#include <string>



namespace DocxFactory
{
	using namespace std;

	class ConverterPrinterUtil;
	class ConverterPrinter
	{
	public:
		static void	setUpdateToc( bool p_update );
		static bool	getUpdateToc();

		static ConverterPrinter& getInstance();
		virtual ~ConverterPrinter();

		void saveAs	( const string& p_importFile,	const string& p_exportFile );
		void print	( const string& p_fileName,		const string& p_printerName, int p_copyCnt );

	protected:

	private:
		static bool m_updateToc;

		ConverterPrinter();
		ConverterPrinter( const ConverterPrinter& p_other );
		ConverterPrinter& operator = ( const ConverterPrinter& p_other );

	#ifndef SWIG
		void initializeUtils();

		list<ConverterPrinterUtil*> m_utils;
	#endif

	};
};

#ifdef DOCXFACTORY_DLL
#undef DOCXFACTORY_DLL
#endif
#endif
