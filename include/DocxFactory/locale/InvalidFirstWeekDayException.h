
#ifndef __DOCXFACTORY_INVALID_FIRST_WEEKDAY_EXCEPTION_H__
#define __DOCXFACTORY_INVALID_FIRST_WEEKDAY_EXCEPTION_H__

#include "DocxFactory/locale/LocaleException.h"



namespace DocxFactory
{
	using namespace std;

	class InvalidFirstWeekDayException : public LocaleException
	{
	public:
		InvalidFirstWeekDayException(
			unsigned short	p_weekDayNum,
			const string&	p_file,
			const int		p_line );

		virtual ~InvalidFirstWeekDayException() throw();

	protected:

	private:

	};
};

#endif
