
#include "DocxFactory/parser/UnclosedBracketException.h"

using namespace DocxFactory;
using namespace std;



UnclosedBracketException::UnclosedBracketException(
	const string&	p_str,
	const string&	p_file,
	const int		p_line ) : ParserException( p_file, p_line )
{
	m_what	= "Unclosed bracket found (" + p_str + ").";
} // c'tor

UnclosedBracketException::~UnclosedBracketException() throw()
{

} // d'tor
