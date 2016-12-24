
#include "DocxFactory/opc/OpcException.h"

using namespace DocxFactory;
using namespace std;



OpcException::OpcException( const string& p_file, int p_line ) : DocxFactoryException( p_file, p_line )
{

} // c'tor

OpcException::~OpcException() throw()
{

} // d'tor
