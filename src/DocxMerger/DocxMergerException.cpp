
#include "DocxFactory/DocxMerger/DocxMergerException.h"

using namespace DocxFactory;
using namespace std;



DocxMergerException::DocxMergerException( const string& p_file, int p_line ) : DocxFactoryException( p_file, p_line )
{

} // c'tor

DocxMergerException::~DocxMergerException() throw()
{

} // d'tor
