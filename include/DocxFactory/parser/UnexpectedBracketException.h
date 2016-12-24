
#ifndef __DOCXFACTORY_UNEXPECTED_BRACKET_EXCEPTION_H__
#define __DOCXFACTORY_UNEXPECTED_BRACKET_EXCEPTION_H__

#include "DocxFactory/parser/ParserException.h"



namespace DocxFactory
{
	using namespace std;

	class UnexpectedBracketException : public ParserException
	{
	public:
		UnexpectedBracketException(
			const string&	p_str,
			const string&	p_file,
			const int		p_line );

		virtual ~UnexpectedBracketException() throw();

	protected:

	private:

	};
};

#endif
