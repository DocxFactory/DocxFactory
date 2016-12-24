
#include "DocxFactory/console/ConsoleException.h"

using namespace DocxFactory;
using namespace std;



ConsoleException::ConsoleException( const string& p_file, int p_line ) : DocxFactoryException( p_file, p_line )
{

} // c'tor

ConsoleException::~ConsoleException() throw()
{

} // d'tor
