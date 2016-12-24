
#include "DocxFactory/util/DocxFactoryException.h"

#include "DocxFactory/str/StrFunc.h"

#include <string>

using namespace DocxFactory;
using namespace std;



DocxFactoryException::DocxFactoryException( const string& p_file, int p_line )
{
	m_file	= p_file;
	m_line	= p_line;
	m_where	= "File: " + m_file + " Line: " + StrFunc::intToStr( m_line );
} // c'tor

DocxFactoryException::~DocxFactoryException() throw()
{

} // d'tor



const char* DocxFactoryException::what() const throw()
{
	return m_what.c_str();
} // what

const char* DocxFactoryException::where() const throw()
{
	return m_where.c_str();
} // where
