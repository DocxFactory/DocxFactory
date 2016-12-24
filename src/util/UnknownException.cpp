
#include "DocxFactory/util/UnknownException.h"

using namespace DocxFactory;
using namespace std;



UnknownException::UnknownException(
	const string&	p_file,
	const int		p_line ) : DocxFactoryException( p_file, p_line )
{

} // c'tor

UnknownException::~UnknownException() throw()
{

} // d'tor
