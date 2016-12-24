
#include "DocxFactory/os/FileNotRemovedException.h"

using namespace DocxFactory;
using namespace std;



FileNotRemovedException::FileNotRemovedException(
	const string&	p_fileName,
	const string&	p_file,
	const int		p_line ) : OsException( p_file, p_line )
{
	m_what	= "File not removed (" + p_fileName + ").";
} // c'tor

FileNotRemovedException::~FileNotRemovedException() throw()
{

} // d'tor
