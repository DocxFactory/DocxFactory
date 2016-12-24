
#ifndef __DOCXFACTORY_MISSING_ARG_EXCEPTION_H__
#define __DOCXFACTORY_MISSING_ARG_EXCEPTION_H__

#include "DocxFactory/console/ConsoleUsageException.h"

#include <string>



namespace DocxFactory
{
	using namespace std;

	class MissingArgException : public ConsoleUsageException
	{
	public:
		MissingArgException(
			const string&	p_arg,
			const string&	p_file,
			const int		p_line );

		virtual ~MissingArgException() throw();

	protected:

	private:

	};
};

#endif
