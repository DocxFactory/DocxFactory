
#include "DocxFactory/locale/InvalidFirstWeekDayException.h"

#include "DocxFactory/str/StrFunc.h"

using namespace DocxFactory;
using namespace std;



InvalidFirstWeekDayException::InvalidFirstWeekDayException(
	unsigned short	p_weekDayNum,
	const string&	p_file,
	const int		p_line ) : LocaleException( p_file, p_line )
{
	m_what	= "Invalid weekday number (" + StrFunc::intToStr( p_weekDayNum ) + "). Must be between 1 or 2.";
} // c'tor

InvalidFirstWeekDayException::~InvalidFirstWeekDayException() throw()
{

} // d'tor
