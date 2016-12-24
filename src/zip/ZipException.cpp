
#include "DocxFactory/zip/ZipException.h"

using namespace DocxFactory;
using namespace std;



ZipException::ZipException( const string& p_file, int p_line ) : DocxFactoryException( p_file, p_line )
{

} // c'tor

ZipException::~ZipException() throw()
{

} // d'tor
