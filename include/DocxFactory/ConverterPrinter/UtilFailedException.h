
#ifndef __DOCXFACTORY_UTIL_FAILED_EXCEPTION_H__
#define __DOCXFACTORY_UTIL_FAILED_EXCEPTION_H__

#include "DocxFactory/ConverterPrinter/ConverterPrinterException.h"



namespace DocxFactory
{
	using namespace std;

	class UtilFailedException : public ConverterPrinterException
	{
	public:
		UtilFailedException(
			const string&	p_util,
			const string&	p_file,
			int				p_line );

		virtual ~UtilFailedException() throw();

	protected:

	private:

	};
};

#endif
