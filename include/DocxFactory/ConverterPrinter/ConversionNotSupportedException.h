
#ifndef __DOCXFACTORY_CONVERSION_NOT_SUPPORTED_EXCEPTION_H__
#define __DOCXFACTORY_CONVERSION_NOT_SUPPORTED_EXCEPTION_H__

#include "DocxFactory/ConverterPrinter/ConverterPrinterException.h"



namespace DocxFactory
{
	using namespace std;

	class ConversionNotSupportedException : public ConverterPrinterException
	{
	public:
		ConversionNotSupportedException(
			const string&	p_importExt,
			const string&	p_exportExt,
			const string&	p_file,
			int				p_line );

		virtual ~ConversionNotSupportedException() throw();

	protected:

	private:

	};
};

#endif
