
#ifndef __DOCXFACTORY_INVALID_DATE_TIME_STR_EXCEPTION_H__
#define __DOCXFACTORY_INVALID_DATE_TIME_STR_EXCEPTION_H__

#include "DocxFactory/locale/LocaleException.h"



namespace DocxFactory
{
	using namespace std;

	class InvalidDateTimeStrException : public LocaleException
	{
	public:
		InvalidDateTimeStrException(
			const string&	p_name,
			const string&	p_file,
			const int		p_line );

		virtual ~InvalidDateTimeStrException() throw();

	protected:

	private:

	};
};

#endif
