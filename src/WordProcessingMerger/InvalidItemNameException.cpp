
#include "DocxFactory/WordProcessingMerger/InvalidItemNameException.h"

using namespace DocxFactory;
using namespace std;



InvalidItemNameException::InvalidItemNameException(
	const string&	p_itemName,
	const string&	p_file,
	const int		p_line ) : WordProcessingException( p_file, p_line )
{
	m_what	= "Invalid item name (" + p_itemName + ").";
} // c'tor

InvalidItemNameException::~InvalidItemNameException() throw()
{

} // d'tor
