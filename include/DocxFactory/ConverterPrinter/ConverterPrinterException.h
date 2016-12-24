
#ifndef __DOCXFACTORY_CONVERTER_PRINTER_EXCEPTION_H__
#define __DOCXFACTORY_CONVERTER_PRINTER_EXCEPTION_H__

#include "DocxFactory/util/DocxFactoryException.h"



namespace DocxFactory
{
	using namespace std;

	class ConverterPrinterException : public DocxFactoryException
	{
	public:
		virtual ~ConverterPrinterException() throw();

	protected:
		ConverterPrinterException( const string& p_file, int p_line );

	private:

	};
};

#endif
