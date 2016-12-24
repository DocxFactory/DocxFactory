
#ifndef __DOCXFACTORY_EXPORT_FORMAT_NOT_SUPPORTED_EXCEPTION_H__
#define __DOCXFACTORY_EXPORT_FORMAT_NOT_SUPPORTED_EXCEPTION_H__

#include "DocxFactory/ConverterPrinter/ConverterPrinterException.h"



namespace DocxFactory
{
	using namespace std;

	class ExportFormatNotSupportedException : public ConverterPrinterException
	{
	public:
		ExportFormatNotSupportedException(
			const string&	p_ext,
			const string&	p_file,
			int				p_line );

		virtual ~ExportFormatNotSupportedException() throw();

	protected:

	private:

	};
};

#endif
