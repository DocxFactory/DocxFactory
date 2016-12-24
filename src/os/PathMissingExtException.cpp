
#include "DocxFactory/os/PathMissingExtException.h"

using namespace DocxFactory;
using namespace std;



PathMissingExtException::PathMissingExtException(
	const string&	p_path,
	const string&	p_file,
	const int		p_line ) : OsException( p_file, p_line )
{
	m_what	= "Path missing ext (" + p_path + ").";
} // c'tor

PathMissingExtException::~PathMissingExtException() throw()
{

} // d'tor
