
#ifndef __DOCXFACTORY_UNEXPECTED_OPT_EXCEPTION_H__
#define __DOCXFACTORY_UNEXPECTED_OPT_EXCEPTION_H__

#include "DocxFactory/console/ConsoleUsageException.h"

#include <string>



namespace DocxFactory
{
	using namespace std;

	class UnexpectedOptException : public ConsoleUsageException
	{
	public:
		UnexpectedOptException(
			const string&	p_opt,
			const string&	p_file,
			const int		p_line );

		virtual ~UnexpectedOptException() throw();

	protected:

	private:

	};
};

#endif
