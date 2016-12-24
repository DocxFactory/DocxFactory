
#include "DocxFactory/xml/XmlException.h"

using namespace DocxFactory;
using namespace std;



XmlException::XmlException( const string& p_file, int p_line ) : DocxFactoryException( p_file, p_line )
{

}

XmlException::~XmlException() throw()
{

}
