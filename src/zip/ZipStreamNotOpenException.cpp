
#include "DocxFactory/zip/ZipStreamNotOpenException.h"

using namespace DocxFactory;
using namespace std;



ZipStreamNotOpenException::ZipStreamNotOpenException(
	const string&	p_file,
	const int		p_line ) : ZipException( p_file, p_line )
{
	m_what	= "Zip stream not open. Cannot write.";
} // c'tor

ZipStreamNotOpenException::~ZipStreamNotOpenException() throw()
{

} // d'tor
