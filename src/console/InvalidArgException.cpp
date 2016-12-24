
#include "DocxFactory/console/InvalidArgException.h"

using namespace DocxFactory;
using namespace std;



InvalidArgException::InvalidArgException(
	const string&	p_arg,
	const string&	p_file,
	const int		p_line ): ConsoleException( p_file, p_line )
{
	m_what	= "Invalid argument (" + p_arg + ").";
} // c'tor

InvalidArgException::~InvalidArgException() throw()
{

} // d'tor
