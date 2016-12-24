
#include "DocxFactory/util/ArgTypeMismatchException.h"

using namespace DocxFactory;
using namespace std;



ArgTypeMismatchException::ArgTypeMismatchException(
	const string&	p_expectedType,
	const string&	p_receivedType,
	const string&	p_file,
	const int		p_line ) : DocxFactoryException( p_file, p_line )
{
	m_what	= "Argument type mismatch (expected: " + p_expectedType + ", received: " + p_receivedType + ").";
} // c'tor

ArgTypeMismatchException::~ArgTypeMismatchException() throw()
{

} // d'tor
