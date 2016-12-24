
#include "DocxFactory/ConverterPrinter/ConversionNotSupportedException.h"

using namespace DocxFactory;
using namespace std;



ConversionNotSupportedException::ConversionNotSupportedException(
	const string&	p_importExt,
	const string&	p_exportExt,
	const string&	p_file,
	int				p_line )

	: ConverterPrinterException( p_file, p_line )
{
	m_what = "Conversion not supported (" + p_importExt + " to " + p_exportExt + ").";
} // c'tor

ConversionNotSupportedException::~ConversionNotSupportedException() throw()
{

} // d'tor
