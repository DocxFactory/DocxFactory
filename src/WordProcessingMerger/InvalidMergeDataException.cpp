
#include "DocxFactory/WordProcessingMerger/InvalidMergeDataException.h"

using namespace DocxFactory;
using namespace std;



InvalidMergeDataException::InvalidMergeDataException(
	const string&	p_file,
	const int		p_line ) : WordProcessingException( p_file, p_line )
{
	m_what	= "Invalid merge data. Only XML or JSON data is supported.";
} // c'tor

InvalidMergeDataException::~InvalidMergeDataException() throw()
{

} // d'tor
