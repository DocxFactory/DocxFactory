
#include "DocxFactory/xml/NodeNotFoundException.h"

using namespace DocxFactory;
using namespace std;



NodeNotFoundException::NodeNotFoundException(
	const string&	p_name,
	const string&	p_file,
	const int		p_line ) : XmlException( p_file, p_line )
{
	m_what	= "Node not found (" + p_name + ").";
} // c'tor

NodeNotFoundException::~NodeNotFoundException() throw()
{

} // d'tor
