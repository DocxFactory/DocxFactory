
#include "DocxFactory/UnoUtil/UnoException.h"

using namespace DocxFactory;
using namespace std;



UnoException::UnoException( const string& p_funcName, const string& p_file, int p_line )

	: DocxFactoryException( p_file, p_line )
{
	m_what = "Error in UnoUtil function (" + p_funcName + ").";
} // c'tor

UnoException::~UnoException() throw()
{

} // d'tor
