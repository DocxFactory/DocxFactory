
#include "DocxFactory/console/GetOptException.h"

using namespace DocxFactory;
using namespace std;



GetOptException::GetOptException(
	const string&	p_file,
	const int		p_line ): ConsoleUsageException( p_file, p_line )
{
	m_what	= "getopt() exception.";
} // c'tor

GetOptException::~GetOptException() throw()
{

} // d'tor
