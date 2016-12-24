
#include "DocxFactory/os/FileExistsException.h"

using namespace DocxFactory;
using namespace std;



FileExistsException::FileExistsException(
	const string&	p_fileName,
	const string&	p_file,
	const int		p_line ) : OsException( p_file, p_line )
{
	m_what	= "File already exists (" + p_fileName + "). Cannot create.";
} // c'tor

FileExistsException::~FileExistsException() throw()
{

} // d'tor
