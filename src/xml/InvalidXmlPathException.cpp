
#include "DocxFactory/xml/InvalidXmlPathException.h"

using namespace DocxFactory;
using namespace std;



InvalidXmlPathException::InvalidXmlPathException(
	const string&	p_file,
	int				p_line ) : XmlException( p_file, p_line )
{
	m_what	= "Invalid XML path exception.";
} // c'tor

InvalidXmlPathException::~InvalidXmlPathException() throw()
{

} // d'tor
