
#include "DocxFactory/locale/IcuException.h"

#include "DocxFactory/str/StrFunc.h"

using namespace DocxFactory;
using namespace std;



IcuException::IcuException(
	const string&	p_funcName,
	const int		p_errorCode,
	const string&	p_file,
	const int		p_line ) : LocaleException( p_file, p_line )
{
	m_what	= "Error in ICU function " + p_funcName
		+ " (error code: " + StrFunc::intToStr( p_errorCode ) + ").";
} // c'tor


IcuException::~IcuException() throw()
{

} // d'tor
