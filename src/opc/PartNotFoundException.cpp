
#include "DocxFactory/opc/PartNotFoundException.h"

using namespace DocxFactory;
using namespace std;



PartNotFoundException::PartNotFoundException(
	const string&	p_fullPath,
	const string&	p_file,
	const int		p_line ) : OpcException( p_file, p_line )
{
	m_what	= "Part not found (" + p_fullPath + ").";
} // c'tor

PartNotFoundException::~PartNotFoundException() throw()
{

} // d'tor
