
#include "DocxFactory/console/UnexpectedOptException.h"

using namespace DocxFactory;
using namespace std;



UnexpectedOptException::UnexpectedOptException(
	const string&	p_opt,
	const string&	p_file,
	const int		p_line ): ConsoleUsageException( p_file, p_line )
{
	m_what	= "Unexpected option (" + p_opt + ").";
} // c'tor

UnexpectedOptException::~UnexpectedOptException() throw()
{

} // d'tor
