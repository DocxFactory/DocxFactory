
#include "DocxFactory/xml/Utf8ToXMLCh.h"
#include "DocxFactory/xml/XmlFunc.h"

using namespace DocxFactory;
using namespace std;



Utf8ToXMLCh::Utf8ToXMLCh( const string& p_str )
{
	m_xmlCh = XmlFunc::utf8ToXMLCh( p_str );
} // c'tor

Utf8ToXMLCh::operator XMLCh*()
{
	return m_xmlCh;
} // operator XMLCh*

Utf8ToXMLCh::~Utf8ToXMLCh()
{
	delete[] m_xmlCh;
} // d'tor
