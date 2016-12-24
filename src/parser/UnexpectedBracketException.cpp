
#include "DocxFactory/parser/UnexpectedBracketException.h"

using namespace DocxFactory;
using namespace std;



UnexpectedBracketException::UnexpectedBracketException(
	const string&	p_str,
	const string&	p_file,
	const int		p_line ) : ParserException( p_file, p_line )
{
	m_what	= "Unexpected bracket found (" + p_str + ").";
} // c'tor

UnexpectedBracketException::~UnexpectedBracketException() throw()
{

} // d'tor
