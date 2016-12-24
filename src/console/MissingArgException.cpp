
#include "DocxFactory/console/MissingArgException.h"

using namespace DocxFactory;
using namespace std;



MissingArgException::MissingArgException(
	const string&	p_arg,
	const string&	p_file,
	const int		p_line ): ConsoleUsageException( p_file, p_line )
{
	m_what	= "Missing argument (" + p_arg + ").";
} // c'tor

MissingArgException::~MissingArgException() throw()
{

} // d'tor
