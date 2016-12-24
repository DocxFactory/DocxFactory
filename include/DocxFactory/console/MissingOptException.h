
#ifndef __DOCXFACTORY_MISSING_OPT_EXCEPTION_H__
#define __DOCXFACTORY_MISSING_OPT_EXCEPTION_H__

#include "DocxFactory/console/ConsoleUsageException.h"

#include <string>



namespace DocxFactory
{
	using namespace std;

	class MissingOptException : public ConsoleUsageException
	{
	public:
		MissingOptException(
			const string&	p_opt,
			const string&	p_file,
			const int		p_line );

		virtual ~MissingOptException() throw();

	protected:

	private:

	};
};

#endif
