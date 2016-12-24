
#include "DocxFactory/parser/UnmatchedQuoteException.h"

using namespace DocxFactory;
using namespace std;



UnmatchedQuoteException::UnmatchedQuoteException(
	const string&	p_str,
	const string&	p_file,
	const int		p_line ) : ParserException( p_file, p_line )
{
	m_what	= "Unmatched quote found (" + p_str + ").";
} // c'tor

UnmatchedQuoteException::~UnmatchedQuoteException() throw()
{

} // d'tor
