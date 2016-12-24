
#ifndef __DOCXFACTORY_INVALID_DATE_FORMAT_EXCEPTION_H__
#define __DOCXFACTORY_INVALID_DATE_FORMAT_EXCEPTION_H__

#include "DocxFactory/locale/LocaleException.h"



namespace DocxFactory
{
	using namespace std;

	class InvalidDateFormatException : public LocaleException
	{
	public:
		InvalidDateFormatException(
			const string&	p_dateFormat,
			const string&	p_file,
			const int		p_line );

		virtual ~InvalidDateFormatException() throw();

	protected:

	private:

	};
};

#endif
