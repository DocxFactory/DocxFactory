
#include "DocxFactory/xml/AsciiToXMLCh.h"

#include <cstring>

using namespace DocxFactory;
using namespace std;



AsciiToXMLCh::AsciiToXMLCh( const string& p_str )
{
	size_t	l_len = p_str.length();
	size_t	l_pos = 0;

	m_xmlCh = new XMLCh[ l_len + 1 ];

	for ( ; l_pos < l_len; l_pos++)
		m_xmlCh[ l_pos ] = ( XMLCh ) p_str[ l_pos ];

	m_xmlCh[ l_pos ] = NULL;
} // c'tor

AsciiToXMLCh::operator XMLCh*()
{
	return m_xmlCh;
} // operator XMLCh*

AsciiToXMLCh::~AsciiToXMLCh()
{
	delete[] m_xmlCh;
} // d'tor
