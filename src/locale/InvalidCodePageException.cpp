
#include "DocxFactory/locale/InvalidCodePageException.h"

using namespace DocxFactory;
using namespace std;



InvalidCodePageException::InvalidCodePageException(
	const string&	p_name,
	const string&	p_file,
	const int		p_line ) : LocaleException( p_file, p_line )
{
	m_what	= "Invalid codepage (" + p_name + ").";
} // c'tor

InvalidCodePageException::~InvalidCodePageException() throw()
{

} // d'tor
