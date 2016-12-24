
#ifndef __DOCXFACTORY_CONSOLE_USAGE_EXCEPTION_H__
#define __DOCXFACTORY_CONSOLE_USAGE_EXCEPTION_H__

#include "DocxFactory/console/ConsoleException.h"



namespace DocxFactory
{
	using namespace std;

	class ConsoleUsageException : public ConsoleException
	{
	public:
		virtual ~ConsoleUsageException() throw();

	protected:
		ConsoleUsageException( const string& p_file, int p_line );

	private:

	};
};

#endif
