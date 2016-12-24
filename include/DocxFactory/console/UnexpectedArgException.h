
#ifndef __DOCXFACTORY_UNEXPECTED_ARG_EXCEPTION_H__
#define __DOCXFACTORY_UNEXPECTED_ARG_EXCEPTION_H__

#include "DocxFactory/console/ConsoleUsageException.h"

#include <string>



namespace DocxFactory
{
	using namespace std;

	class UnexpectedArgException : public ConsoleUsageException
	{
	public:
		UnexpectedArgException(
			const string&	p_arg,
			const string&	p_file,
			const int		p_line );

		virtual ~UnexpectedArgException() throw();

	protected:

	private:

	};
};

#endif
