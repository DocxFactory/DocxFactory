
#include "DocxFactory/parser/ParserException.h"

using namespace DocxFactory;
using namespace std;



ParserException::ParserException( const string& p_file, int p_line ) : DocxFactoryException( p_file, p_line )
{

} // c'tor

ParserException::~ParserException() throw()
{

} // d'tor
