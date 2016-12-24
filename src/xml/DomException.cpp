
#include "DocxFactory/xml/DomException.h"

using namespace DocxFactory;
using namespace std;



DomException::DomException(
	const string&	p_msg,
	const string&	p_file,
	const int		p_line ) : XmlException( p_file, p_line )
{
	m_what	= "DOM exception (" + p_msg + ").";
} // c'tor

DomException::~DomException() throw()
{

} // d'tor
