
#include "DocxFactory/util/GeneralException.h"

using namespace DocxFactory;
using namespace std;



GeneralException::GeneralException(
	const string&	p_msg,
	const string&	p_file,
	const int		p_line ) : DocxFactoryException( p_file, p_line )
{
	m_what	= p_msg;
} // c'tor

GeneralException::~GeneralException() throw()
{

} // d'tor
