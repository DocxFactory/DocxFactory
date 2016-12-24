
#include "DocxFactory/opc/PartTypeNotFoundException.h"

using namespace DocxFactory;
using namespace std;



PartTypeNotFoundException::PartTypeNotFoundException(
	const string&	p_type,
	const string&	p_file,
	const int		p_line ) : OpcException( p_file, p_line )
{
	m_what	= "Part type not found (" + p_type + ").";
} // c'tor

PartTypeNotFoundException::~PartTypeNotFoundException() throw()
{

} // d'tor
