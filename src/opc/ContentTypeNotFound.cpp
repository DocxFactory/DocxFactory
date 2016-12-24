
#include "DocxFactory/opc/ContentTypeNotFoundException.h"

using namespace DocxFactory;
using namespace std;



ContentTypeNotFoundException::ContentTypeNotFoundException(
	const string&	p_contentType,
	const string&	p_file,
	const int		p_line ) : OpcException( p_file, p_line )
{
	m_what	= "Content type not found (" + p_contentType + ").";
} // c'tor

ContentTypeNotFoundException::~ContentTypeNotFoundException() throw()
{

} // d'tor
