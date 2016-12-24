
#include "DocxFactory/os/FileNotFoundException.h"

using namespace DocxFactory;
using namespace std;



FileNotFoundException::FileNotFoundException(
	const string&	p_fileName,
	const string&	p_file,
	const int		p_line ) : OsException( p_file, p_line )
{
	m_what	= "File not found (" + p_fileName + ").";
} // c'tor

FileNotFoundException::~FileNotFoundException() throw()
{

} // d'tor
