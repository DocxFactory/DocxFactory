
#include "DocxFactory/zip/ZipStreamOpenException.h"

using namespace DocxFactory;
using namespace std;



ZipStreamOpenException::ZipStreamOpenException(
	const string&	p_file,
	const int		p_line ) : ZipException( p_file, p_line )
{
	m_what	= "Zip stream already open. Cannot open more than one stream at the same time.";
} // c'tor

ZipStreamOpenException::~ZipStreamOpenException() throw()
{

} // d'tor
