
#include "DocxFactory/locale/InvalidDateFormatException.h"

using namespace DocxFactory;
using namespace std;



InvalidDateFormatException::InvalidDateFormatException(
	const string&	p_dateFormat,
	const string&	p_file,
	const int		p_line ) : LocaleException( p_file, p_line )
{
	m_what	= "Invalid date format (" + p_dateFormat + "). Must be dmy or mdy or ymd or ydm.";
} // c'tor

InvalidDateFormatException::~InvalidDateFormatException() throw()
{

} // d'tor
