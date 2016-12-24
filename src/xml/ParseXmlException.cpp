
#include "DocxFactory/xml/ParseXmlException.h"

using namespace DocxFactory;
using namespace std;



ParseXmlException::ParseXmlException(
	const string&	p_file,
	const int		p_line ) : XmlException( p_file, p_line )
{
	m_what	= "Parse XML exception.";
} // c'tor

ParseXmlException::~ParseXmlException() throw()
{

} // d'tor
