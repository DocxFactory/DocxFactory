
#ifndef __DOCXFACTORY_LOCALE_EXCEPTION_H__
#define __DOCXFACTORY_LOCALE_EXCEPTION_H__

#include "DocxFactory/util/DocxFactoryException.h"



namespace DocxFactory
{
	using namespace std;

	class LocaleException : public DocxFactoryException
	{
	public:
		virtual ~LocaleException() throw();

	protected:
		LocaleException( const string& p_file, int p_line );

	private:

	};
};

#endif
