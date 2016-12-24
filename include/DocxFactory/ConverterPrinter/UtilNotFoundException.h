
#ifndef __DOCXFACTORY_UTIL_NOT_FOUND_EXCEPTION_H__
#define __DOCXFACTORY_UTIL_NOT_FOUND_EXCEPTION_H__

#include "DocxFactory/ConverterPrinter/ConverterPrinterException.h"



namespace DocxFactory
{
	using namespace std;

	class UtilNotFoundException : public ConverterPrinterException
	{
	public:
		UtilNotFoundException( const string& p_file, int p_line );
		virtual ~UtilNotFoundException() throw();

	protected:

	private:

	};
};

#endif
