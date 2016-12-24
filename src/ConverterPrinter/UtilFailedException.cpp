
#include "DocxFactory/ConverterPrinter/UtilFailedException.h"

using namespace DocxFactory;
using namespace std;



UtilFailedException::UtilFailedException( const string& p_util, const string& p_file, int p_line )

	: ConverterPrinterException( p_file, p_line )
{
	m_what = "Util failed (" + p_util + ").";
} // c'tor

UtilFailedException::~UtilFailedException() throw()
{

} // d'tor
