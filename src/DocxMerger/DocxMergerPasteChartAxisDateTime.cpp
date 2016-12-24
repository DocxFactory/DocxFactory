
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisDateTime.h"

using namespace DocxFactory;
using namespace std;



DocxMergerPasteChartAxisDateTime::DocxMergerPasteChartAxisDateTime() : DocxMergerPasteChartAxis()
{

} // c'tor

DocxMergerPasteChartAxisDateTime::~DocxMergerPasteChartAxisDateTime()
{

} // d'tor



size_t DocxMergerPasteChartAxisDateTime::setValue( double p_value )
{
	map<double, size_t>::iterator l_iterator = m_index.find( p_value );
	if ( l_iterator != m_index.end() )
		return l_iterator ->second;

	size_t l_index = m_values.size();
	m_values.push_back( p_value );
	m_index.insert( make_pair( p_value, l_index ) );

	return l_index;
} // setValue

const vector<double>* DocxMergerPasteChartAxisDateTime::getValues() const
{
	return &m_values;
} // getValues
