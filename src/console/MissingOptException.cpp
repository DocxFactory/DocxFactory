
#include "DocxFactory/console/MissingOptException.h"

using namespace DocxFactory;
using namespace std;



MissingOptException::MissingOptException(
	const string&	p_opt,
	const string&	p_file,
	const int		p_line ): ConsoleUsageException( p_file, p_line )
{
	m_what	= "Missing option (" + p_opt + ").";
} // c'tor

MissingOptException::~MissingOptException() throw()
{

} // d'tor
