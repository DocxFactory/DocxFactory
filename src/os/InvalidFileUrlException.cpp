
#include "DocxFactory/os/InvalidFileUrlException.h"

using namespace DocxFactory;
using namespace std;



InvalidFileUrlException::InvalidFileUrlException(
	const string&	p_url,
	const string&	p_file,
	const int		p_line ) : OsException( p_file, p_line )
{
	m_what	= "Invalid file URL (" + p_url + ").";
} // c'tor

InvalidFileUrlException::~InvalidFileUrlException() throw()
{

} // d'tor
