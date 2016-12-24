
#ifndef __DOCXFACTORY_UNCLOSED_BRACKET_EXCEPTION_H__
#define __DOCXFACTORY_UNCLOSED_BRACKET_EXCEPTION_H__

#include "DocxFactory/parser/ParserException.h"

namespace DocxFactory
{
	using namespace std;

	class UnclosedBracketException : public ParserException
	{
	public:
		UnclosedBracketException(
			const string&	p_str,
			const string&	p_file,
			const int		p_line );

		virtual ~UnclosedBracketException() throw();

	protected:

	private:

	};
};

#endif
