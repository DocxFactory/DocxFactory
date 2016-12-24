
#include "DocxFactory/zip/MinizipException.h"

#include "DocxFactory/str/StrFunc.h"

using namespace DocxFactory;
using namespace std;



MinizipException::MinizipException(
	const string&	p_funcName,
	const int		p_errorCode,
	const string&	p_file,
	const int		p_line ) : ZipException( p_file, p_line )
{
	m_what	= "Error in minizip function " + p_funcName
			+ " (error code: " + StrFunc::intToStr( p_errorCode ) + ")."
			+ " Minizip exceptions are usually dir and file access privileges related.";
} // c'tor

MinizipException::MinizipException(
	const string&	p_funcName,
	const string&	p_file,
	const int		p_line ) : ZipException( p_file, p_line )
{
	m_what	= "Error in minizip function " + p_funcName + "."
			+ " Minizip exceptions are usually dir and file access privileges related.";
} // c'tor

MinizipException::~MinizipException() throw()
{

} // d'tor
