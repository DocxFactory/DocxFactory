
#ifndef __DOCXFACTORY_CONSOLE_EXCEPTION_H__
#define __DOCXFACTORY_CONSOLE_EXCEPTION_H__

#include "DocxFactory/util/DocxFactoryException.h"



namespace DocxFactory
{
	using namespace std;

	class ConsoleException : public DocxFactoryException
	{
	public:
		virtual ~ConsoleException() throw();

	protected:
		ConsoleException( const string& p_file, int p_line );

	private:

	};
};

#endif
