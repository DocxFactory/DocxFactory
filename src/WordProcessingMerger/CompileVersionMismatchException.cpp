
#include "DocxFactory/WordProcessingMerger/CompileVersionMismatchException.h"

using namespace DocxFactory;
using namespace std;



CompileVersionMismatchException::CompileVersionMismatchException(
	const string&	p_file,
	const int		p_line ) : WordProcessingException( p_file, p_line )
{
	m_what	= "Template compiled with a different version. Recompile template file.";
} // c'tor

CompileVersionMismatchException::~CompileVersionMismatchException() throw()
{

} // d'tor
