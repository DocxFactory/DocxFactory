
#include "DocxFactory/os/OsException.h"

using namespace DocxFactory;
using namespace std;



OsException::OsException( const string& p_file, int p_line ) : DocxFactoryException( p_file, p_line )
{

} // c'tor

OsException::~OsException() throw()
{

} // d'tor
