
#ifndef __DOCXFACTORY_OLE_EXCEPTION_H__
#define __DOCXFACTORY_OLE_EXCEPTION_H__

#include "DocxFactory/ConverterPrinter/ConverterPrinterException.h"



namespace DocxFactory
{
	using namespace std;

	class OleException : public ConverterPrinterException
	{
	public:
		OleException( const string& p_funcName, const string& p_file, int p_line );
		virtual ~OleException() throw();

	protected:

	private:

	};
};

#endif
