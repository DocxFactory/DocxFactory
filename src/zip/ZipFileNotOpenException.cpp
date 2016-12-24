
#include "DocxFactory/zip/ZipFileNotOpenException.h"

using namespace DocxFactory;
using namespace std;



ZipFileNotOpenException::ZipFileNotOpenException(
	const string&	p_file,
	const int		p_line ) : ZipException( p_file, p_line )
{
	m_what	= "Zip file not open. Cannot write.";
} // c'tor

ZipFileNotOpenException::~ZipFileNotOpenException() throw()
{

} // d'tor
