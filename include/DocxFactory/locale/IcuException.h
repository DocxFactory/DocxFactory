
#ifndef __DOCXFACTORY_ICU_EXCEPTION_H__
#define __DOCXFACTORY_ICU_EXCEPTION_H__

#include "DocxFactory/locale//LocaleException.h"



namespace DocxFactory
{
	using namespace std;

	class IcuException : public LocaleException
	{
	public:
		IcuException(
			const string&	p_funcName,
			const int		p_errorCode,
			const string&	p_file,
			const int		p_line );

		virtual ~IcuException() throw();

	protected:

	private:

	};
};

#endif
