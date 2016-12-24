
#ifndef __DOCXFACTORY_INVALID_YEAR_OFFSET_EXCEPTION_H__
#define __DOCXFACTORY_INVALID_YEAR_OFFSET_EXCEPTION_H__

#include "DocxFactory/locale/LocaleException.h"



namespace DocxFactory
{
	using namespace std;

	class InvalidYearOffsetException : public LocaleException
	{
	public:
		InvalidYearOffsetException(
			const unsigned short	p_yearOffset,
			const string&			p_file,
			const int				p_line );

		virtual ~InvalidYearOffsetException() throw();

	protected:

	private:

	};
};

#endif
