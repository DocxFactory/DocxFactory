
#ifndef __DOCXFACTORY_UNKNOWN_EXCEPTION_H__
#define __DOCXFACTORY_UNKNOWN_EXCEPTION_H__

#include "DocxFactory/util/DocxFactoryException.h"

#include <string>



namespace DocxFactory
{
	using namespace std;

	class UnknownException : public DocxFactoryException
	{
	public:
		UnknownException(
			const string&	p_file,
			const int		p_line );

		virtual ~UnknownException() throw();

	protected:

	private:

	};
};

#endif
