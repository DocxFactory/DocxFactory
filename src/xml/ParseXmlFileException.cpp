
#include "DocxFactory/xml/ParseXmlFileException.h"

using namespace DocxFactory;
using namespace std;



ParseXmlFileException::ParseXmlFileException(
	const string&	p_fileName,
	const string&	p_file,
	const int		p_line ) : ParseXmlException( p_file, p_line )
{
	m_what	= "Parse XML file exception (" + p_fileName + ").";
} // c'tor

ParseXmlFileException::~ParseXmlFileException() throw()
{

} // d'tor
