
#ifndef __DOCXFACTORY_ARG_TYPE_MISMATCH_EXCEPTION_H__
#define __DOCXFACTORY_ARG_TYPE_MISMATCH_EXCEPTION_H__

#include "DocxFactory/util/DocxFactoryException.h"



namespace DocxFactory
{
	using namespace std;

	class ArgTypeMismatchException : public DocxFactoryException
	{
	public:
		ArgTypeMismatchException(
			const string&	p_expectedType,
			const string&	p_receivedType,
			const string&	p_file,
			const int		p_line );

		virtual ~ArgTypeMismatchException() throw();

	protected:

	private:

	};
};

#endif
