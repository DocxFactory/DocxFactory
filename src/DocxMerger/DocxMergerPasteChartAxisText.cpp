
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisText.h"

#include "DocxFactory/str/StrFunc.h"

using namespace DocxFactory;
using namespace std;



DocxMergerPasteChartAxisText::DocxMergerPasteChartAxisText() : DocxMergerPasteChartAxis()
{

} // c'tor

DocxMergerPasteChartAxisText::~DocxMergerPasteChartAxisText()
{

} // d'tor



size_t DocxMergerPasteChartAxisText::setValue( const string& p_value )
{
	string l_str = StrFunc::lc( p_value );

	map<string, size_t>::iterator l_iterator = m_index.find( l_str );
	if ( l_iterator != m_index.end() )
		return l_iterator ->second;

	size_t l_index = m_values.size();
	m_values.push_back( p_value );
	m_index.insert( make_pair( l_str, l_index ) );

	return l_index;
} // setValue

const vector<string>* DocxMergerPasteChartAxisText::getValues() const
{
	return &m_values;
} // getValues
