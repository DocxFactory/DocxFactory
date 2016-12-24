
#ifndef __DOCXFACTORY_GET_OPT_EXCEPTION_H__
#define __DOCXFACTORY_GET_OPT_EXCEPTION_H__

#include "DocxFactory/console/ConsoleUsageException.h"

#include <string>



namespace DocxFactory
{
	using namespace std;

	class GetOptException : public ConsoleUsageException
	{
	public:
		GetOptException(
			const string&	p_file,
			const int		p_line );

		virtual ~GetOptException() throw();

	protected:

	private:

	};
};

#endif
