
#include "DocxFactory/console/UnexpectedArgException.h"

using namespace DocxFactory;
using namespace std;



UnexpectedArgException::UnexpectedArgException(
	const string&	p_arg,
	const string&	p_file,
	const int		p_line ): ConsoleUsageException( p_file, p_line )
{
	m_what	= "Unexpected argument (" + p_arg + ").";
} // c'tor

UnexpectedArgException::~UnexpectedArgException() throw()
{

} // d'tor
