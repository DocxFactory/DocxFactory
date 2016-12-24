
#include "DocxFactory/ConverterPrinter/PrintingNotSupportedException.h"

using namespace DocxFactory;
using namespace std;



PrintingNotSupportedException::PrintingNotSupportedException( const string& p_fileName, const string& p_file, int p_line )

	: ConverterPrinterException( p_file, p_line )
{
	m_what = "Printing not supported (" + p_fileName + ").";
} // c'tor

PrintingNotSupportedException::~PrintingNotSupportedException() throw()
{

} // d'tor
