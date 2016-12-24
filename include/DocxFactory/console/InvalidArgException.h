
#ifndef __DOCXFACTORY_INVALID_ARG_EXCEPTION_H__
#define __DOCXFACTORY_INVALID_ARG_EXCEPTION_H__

#include "DocxFactory/console/ConsoleException.h"

#include <string>



namespace DocxFactory
{
	using namespace std;

	class InvalidArgException : public ConsoleException
	{
	public:
		InvalidArgException(
			const string&	p_arg,
			const string&	p_file,
			const int		p_line );

		virtual ~InvalidArgException() throw();

	protected:

	private:

	};
};

#endif
