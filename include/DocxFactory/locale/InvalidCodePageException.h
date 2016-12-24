
#ifndef __DOCXFACTORY_INVALID_CODEPAGE_EXCEPTION_H__
#define __DOCXFACTORY_INVALID_CODEPAGE_EXCEPTION_H__

#include "DocxFactory/locale/LocaleException.h"



namespace DocxFactory
{
	using namespace std;

	class InvalidCodePageException : public LocaleException
	{
	public:
		InvalidCodePageException(
			const string&	p_name,
			const string&	p_file,
			const int		p_line );

		virtual ~InvalidCodePageException() throw();

	protected:

	private:

	};
};

#endif
