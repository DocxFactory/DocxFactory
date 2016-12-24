
#include "DocxFactory/os/DirNotFoundException.h"

using namespace DocxFactory;
using namespace std;



DirNotFoundException::DirNotFoundException(
	const string&	p_dirName,
	const string&	p_file,
	const int		p_line ) : OsException( p_file, p_line )
{
	m_what	= "Directory not found (" + p_dirName + ").";
} // c'tor

DirNotFoundException::~DirNotFoundException() throw()
{

} // d'tor
