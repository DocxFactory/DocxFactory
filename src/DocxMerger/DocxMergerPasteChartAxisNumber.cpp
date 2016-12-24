
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisNumber.h"

using namespace DocxFactory;
using namespace std;



DocxMergerPasteChartAxisNumber::DocxMergerPasteChartAxisNumber() : DocxMergerPasteChartAxis()
{

} // c'tor

DocxMergerPasteChartAxisNumber::~DocxMergerPasteChartAxisNumber()
{

} // d'tor



size_t DocxMergerPasteChartAxisNumber::setValue( double p_value )
{
	map<double, size_t>::iterator l_iterator = m_index.find( p_value );
	if ( l_iterator != m_index.end() )
		return l_iterator ->second;

	size_t l_index = m_values.size();
	m_values.push_back( p_value );
	m_index.insert( make_pair( p_value, l_index ) );

	return l_index;
} // setValue

const vector<double>* DocxMergerPasteChartAxisNumber::getValues() const
{
	return &m_values;
} // getValues
