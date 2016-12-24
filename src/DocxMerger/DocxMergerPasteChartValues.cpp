
#include "DocxFactory/DocxMerger/DocxMergerPasteChartValues.h"

using namespace DocxFactory;
using namespace std;



DocxMergerPasteChartValues::DocxMergerPasteChartValues()
{
	m_values.push_back( vector<double>() );
	m_values.back().push_back( 0.0f );
} // c'tor

DocxMergerPasteChartValues::DocxMergerPasteChartValues( size_t p_serLength, size_t p_catLength )
{
	size_t i;
	size_t j;

	if( p_serLength == 0 )
		p_serLength = 1;

	if( p_catLength == 0 )
		p_catLength = 1;

	for ( i = 0; i < p_serLength; i++ )
	{
		m_values.push_back( vector<double>() );

		for ( j = 0; j < p_catLength; j++ )
		{
			m_values.back().push_back( 0.0f );
		}
	}
} // c'tor

DocxMergerPasteChartValues::~DocxMergerPasteChartValues()
{

} // d'tor



void DocxMergerPasteChartValues::setValue( size_t p_series, size_t p_category, double p_value )
{
	size_t i;

	if ( p_series >= m_values.size() )
	{
		m_values.push_back( vector<double>() );

		for ( i = 0; i < m_values[0].size(); i++ )
		{
			m_values.back().push_back( 0.0f );
		}
	}

	if ( p_category >= m_values[0].size() )
	{
		for ( i = 0; i < m_values.size(); i++ )
		{
			m_values[i].push_back( 0.0f );
		}
	}

	/***
	size_t len;
	size_t i;
	size_t j;

	while ( p_series > m_values.size() - 1 )
	{
		m_values.push_back( vector<double>() );

		for ( i = 0; i < m_values[0].size(); i++ )
		{
			m_values.back().push_back( 0.0f );
		}
	}

	if ( p_category > m_values[0].size() - 1 )
	{
		len = m_values[0].size();

		for ( i = 0; i < m_values.size(); i++ )
		{
			for ( j = len; j <= p_category; j++ )
			{
				m_values[i].push_back( 0.0f );
			}
		}
	}
	***/

	m_values[ p_series ][ p_category ] += p_value;
} // setValues

const vector<vector<double>>* DocxMergerPasteChartValues::getValues() const
{
	return &m_values;
} // getValues
