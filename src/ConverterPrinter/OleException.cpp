
#include "DocxFactory/ConverterPrinter/OleException.h"

using namespace DocxFactory;
using namespace std;



OleException::OleException( const string& p_funcName, const string& p_file, int p_line )

	: ConverterPrinterException( p_file, p_line )
{
	m_what = "Error in OLE function (" + p_funcName + ").";
} // c'tor

OleException::~OleException() throw()
{

} // d'tor
