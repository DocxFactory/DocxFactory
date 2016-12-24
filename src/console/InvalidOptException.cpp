
#include "DocxFactory/console/InvalidOptException.h"

using namespace DocxFactory;
using namespace std;



InvalidOptException::InvalidOptException(
	const string&	p_opt,
	const string&	p_arg,
	const string&	p_file,
	const int		p_line ): ConsoleException( p_file, p_line )
{
	m_what	= "Invalid option argument (" + p_opt + " " + p_arg + ").";
} // c'tor

InvalidOptException::~InvalidOptException() throw()
{

} // d'tor
