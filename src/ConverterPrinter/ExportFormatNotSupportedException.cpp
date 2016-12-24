
#include "DocxFactory/ConverterPrinter/ExportFormatNotSupportedException.h"

using namespace DocxFactory;
using namespace std;



ExportFormatNotSupportedException::ExportFormatNotSupportedException( const string& p_ext, const string& p_file, int p_line )

	: ConverterPrinterException( p_file, p_line )
{
	m_what = "Export format not supported (" + p_ext + ").";
} // c'tor


ExportFormatNotSupportedException::~ExportFormatNotSupportedException() throw()
{

} // d'tor
