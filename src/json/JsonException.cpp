
#include "DocxFactory/json/JsonException.h"

using namespace DocxFactory;
using namespace std;



JsonException::JsonException( const string& p_file, int p_line ) : DocxFactoryException( p_file, p_line )
{

} // c'tor

JsonException::~JsonException() throw()
{

} // d'tor
