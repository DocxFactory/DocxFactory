
#include "DocxFactory/xml/StrToXMLCh.h"
#include "DocxFactory/xml/XmlFunc.h"

using namespace DocxFactory;
using namespace std;



StrToXMLCh::StrToXMLCh( const string& p_str )
{
	m_xmlCh = XmlFunc::strToXMLCh( p_str );
} // c'tor

StrToXMLCh::operator XMLCh*()
{
	return m_xmlCh;
} // operator XMLCh*

StrToXMLCh::~StrToXMLCh()
{
	delete[] m_xmlCh;
} // d'tor
