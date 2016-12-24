
#include "DocxFactory/locale/InvalidYearOffsetException.h"

#include "DocxFactory/str/StrFunc.h"

using namespace DocxFactory;
using namespace std;



InvalidYearOffsetException::InvalidYearOffsetException(
	const unsigned short	p_yearOffset,
	const string&			p_file,
	const int				p_line ) : LocaleException( p_file, p_line )
{
	m_what	= "Invalid year offset (" + StrFunc::intToStr( p_yearOffset ) + "). Must be between 1900 to 1999.";
} // c'tor

InvalidYearOffsetException::~InvalidYearOffsetException() throw()
{

} // d'tor
