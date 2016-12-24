
#ifndef __DOCXFACTORY_UNMATCHED_QUOTE_EXCEPTION_
#define __DOCXFACTORY_UNMATCHED_QUOTE_EXCEPTION_

#include "DocxFactory/parser/ParserException.h"



namespace DocxFactory
{
	using namespace std;

	class UnmatchedQuoteException : public ParserException
	{
	
	public:
		UnmatchedQuoteException(
			const string&	p_str,
			const string&	p_file,
			const int		p_line );

		virtual ~UnmatchedQuoteException() throw();

	protected:

	private:

	};
};

#endif
