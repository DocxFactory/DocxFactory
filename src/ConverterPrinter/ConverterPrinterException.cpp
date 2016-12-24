
#include "DocxFactory/ConverterPrinter/ConverterPrinterException.h"

using namespace DocxFactory;
using namespace std;



ConverterPrinterException::ConverterPrinterException( const string& p_file, int p_line ) : DocxFactoryException( p_file, p_line )
{

} // c'tor

ConverterPrinterException::~ConverterPrinterException() throw()
{

} // d'tor
