
#include "DocxFactory/os/UnsupportedFileTypeException.h"

using namespace DocxFactory;
using namespace std;



UnsupportedFileTypeException::UnsupportedFileTypeException(
	const string&	p_fileName,
	const string&	p_fileTypes,
	const string&	p_file,
	const int		p_line ): OsException( p_file, p_line )
{
	m_what	= "Unsupported file type (" + p_fileName + "). Must be a " + p_fileTypes + " file.";
} // c'tor

UnsupportedFileTypeException::~UnsupportedFileTypeException() throw()
{

} // d'tor
