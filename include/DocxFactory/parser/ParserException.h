
#ifndef __DOCXFACTORY_PARSER_EXCEPTION_H__
#define __DOCXFACTORY_PARSER_EXCEPTION_H__

#include "DocxFactory/util/DocxFactoryException.h"



namespace DocxFactory
{
	using namespace std;

	class ParserException : public DocxFactoryException
	{
	public:
		ParserException(
			const string&	p_file,
			const int		p_line );

		virtual ~ParserException() throw();

	protected:

	private:

	};
};

#endif
