
#include "DocxFactory/WordProcessingMerger/TemplateNotLoadedException.h"

using namespace DocxFactory;
using namespace std;



TemplateNotLoadedException::TemplateNotLoadedException(
	const string&	p_file,
	const int		p_line ) : WordProcessingException( p_file, p_line )
{
	m_what	= "Template not loaded.";
} // c'tor

TemplateNotLoadedException::~TemplateNotLoadedException() throw()
{

} // d'tor
