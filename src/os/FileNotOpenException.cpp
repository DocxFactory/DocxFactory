
#include "DocxFactory/os/FileNotOpenException.h"

using namespace DocxFactory;
using namespace std;



FileNotOpenException::FileNotOpenException(
	const string&	p_fileName,
	const string&	p_file,
	const int		p_line ) : OsException( p_file, p_line )
{
	m_what	= "File not opened (" + p_fileName + ").";
} // c'tor

FileNotOpenException::~FileNotOpenException() throw()
{

} // d'tor
