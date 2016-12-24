
#ifndef __DOCXFACTORY_PRINTING_NOT_SUPPORTED_EXCEPTION_H__
#define __DOCXFACTORY_PRINTING_NOT_SUPPORTED_EXCEPTION_H__

#include "DocxFactory/ConverterPrinter/ConverterPrinterException.h"



namespace DocxFactory
{
	using namespace std;

	class PrintingNotSupportedException : public ConverterPrinterException
	{
	public:
		PrintingNotSupportedException(
			const string&	p_fileName,
			const string&	p_file,
			int				p_line );

		virtual ~PrintingNotSupportedException() throw();

	protected:

	private:

	};
};

#endif
