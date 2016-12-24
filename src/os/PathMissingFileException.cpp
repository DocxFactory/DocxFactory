
#include "DocxFactory/os/PathMissingFileException.h"

using namespace DocxFactory;
using namespace std;



PathMissingFileException::PathMissingFileException(
	const string&	p_path,
	const string&	p_file,
	const int		p_line ) : OsException( p_file, p_line )
{
	m_what	= "Path missing file (" + p_path + ").";
} // c'tor

PathMissingFileException::~PathMissingFileException() throw()
{

} // d'tor
