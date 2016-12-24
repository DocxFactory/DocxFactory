
#ifndef __DOCXFACTORY_UNO_EXCEPTION_H__
#define __DOCXFACTORY_UNO_EXCEPTION_H__

#include "DocxFactory/util/DocxFactoryException.h"



namespace DocxFactory
{
	using namespace std;

	class UnoException : public DocxFactoryException
	{
	public:
		UnoException( const string& p_funcName, const string& p_file, int p_line );
		virtual ~UnoException() throw();

	protected:

	private:

	};
};

#endif
