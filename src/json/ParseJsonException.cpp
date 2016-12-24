
#include "DocxFactory/json/ParseJsonException.h"

using namespace DocxFactory;
using namespace std;



ParseJsonException::ParseJsonException(
	const string&	p_file,
	const int		p_line ): JsonException( p_file, p_line )
{
	m_what	= "Parse JSON exception.";
} // c'tor

ParseJsonException::~ParseJsonException() throw()
{

} // d'tor