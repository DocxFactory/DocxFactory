
#include "DocxFactory/locale/InvalidDateTimeStrException.h"

using namespace DocxFactory;
using namespace std;



InvalidDateTimeStrException::InvalidDateTimeStrException(
	const string&	p_name,
	const string&	p_file,
	const int		p_line ) : LocaleException( p_file, p_line )
{
	m_what	= "Invalid datetime string (" + p_name + ").";
} // c'tor

InvalidDateTimeStrException::~InvalidDateTimeStrException() throw()
{

} // d'tor
