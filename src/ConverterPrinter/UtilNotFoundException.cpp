
#include "DocxFactory/ConverterPrinter/UtilNotFoundException.h"

using namespace DocxFactory;
using namespace std;



UtilNotFoundException::UtilNotFoundException( const string& p_file, int p_line )
	: ConverterPrinterException( p_file, p_line )
{
	m_what = "Util not found.";
} // c'tor

UtilNotFoundException::~UtilNotFoundException() throw()
{

} // d'tor
