
#include "DocxFactory/json/ParseJsonFileException.h"

using namespace DocxFactory;
using namespace std;



ParseJsonFileException::ParseJsonFileException(
	const string&	p_fileName,
	const string&	p_file,
	const int		p_line ): ParseJsonException( p_file, p_line )
{
	m_what	= "Parse JSON file exception (" + p_fileName + ").";
} // c'tor

ParseJsonFileException::~ParseJsonFileException() throw()
{

} // d'tor