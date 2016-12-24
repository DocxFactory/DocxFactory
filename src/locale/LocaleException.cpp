
#include "DocxFactory/locale/LocaleException.h"

using namespace DocxFactory;
using namespace std;



LocaleException::LocaleException( const string& p_file, int p_line ) : DocxFactoryException( p_file, p_line )
{

} // c'tor

LocaleException::~LocaleException() throw()
{

} // d'tor
