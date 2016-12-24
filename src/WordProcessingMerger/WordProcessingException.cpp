
#include "DocxFactory/WordProcessingMerger/WordProcessingException.h"

using namespace DocxFactory;
using namespace std;



WordProcessingException::WordProcessingException( const string& p_file, int p_line ) : DocxFactoryException( p_file, p_line )
{

} // c'tor

WordProcessingException::~WordProcessingException() throw()
{

} // d'tor
