
#include "DocxFactory/console/ConsoleUsageException.h"

using namespace DocxFactory;
using namespace std;



ConsoleUsageException::ConsoleUsageException( const string& p_file, int p_line ) : ConsoleException( p_file, p_line )
{

} // c'tor

ConsoleUsageException::~ConsoleUsageException() throw()
{

} // d'tor
