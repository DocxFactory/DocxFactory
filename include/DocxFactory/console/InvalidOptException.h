
#ifndef __DOCXFACTORY_INVALID_OPT_EXCEPTION_H__
#define __DOCXFACTORY_INVALID_OPT_EXCEPTION_H__

#include "DocxFactory/console/ConsoleException.h"

#include <string>



namespace DocxFactory
{
	using namespace std;

	class InvalidOptException : public ConsoleException
	{
	public:
		InvalidOptException(
			const string&	p_opt,
			const string&	p_arg,
			const string&	p_file,
			const int		p_line );

		virtual ~InvalidOptException() throw();

	protected:

	private:

	};
};

#endif
