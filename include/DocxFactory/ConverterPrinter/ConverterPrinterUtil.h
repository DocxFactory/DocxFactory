
#ifndef __DOCXFACTORY_CONVERTER_PRINTER_UTIL_H__
#define __DOCXFACTORY_CONVERTER_PRINTER_UTIL_H__

#include <string>



namespace DocxFactory
{
	using namespace std;

	class ConverterPrinterUtil
	{
	public:
		ConverterPrinterUtil() {}
		virtual ~ConverterPrinterUtil() {}
		
		virtual void	saveAs	( const string& p_importFile,	const string& p_exportFile ) = 0;
		virtual void	print	( const string& p_fileName,		const string& p_printerName, int p_copyCnt ) = 0;

		virtual double	getConversionGrade	( const string& p_importExt, const string& p_exportExt ) = 0;
		virtual double	getPrintingGrade	( const string& p_ext ) = 0;

	protected:
		ConverterPrinterUtil( const ConverterPrinterUtil& p_other );
		ConverterPrinterUtil& operator = ( const ConverterPrinterUtil& p_other );

	private:

	};
};

#endif
