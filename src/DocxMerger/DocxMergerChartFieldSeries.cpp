
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartFieldSeries.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxis.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxisText.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxisNumber.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxisDateTime.h"
#include "DocxFactory/DocxMerger/DocxMergerChartValues.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartFieldSeries.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxis.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisText.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisNumber.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisDateTime.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartValues.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/UnzipFile.h"

using namespace DocxFactory;
using namespace std;



DocxMergerChartFieldSeries::DocxMergerChartFieldSeries() : DocxMergerChartField( DocxMergerChartField::CHART_SERIES )
{
	m_serAxis	= new DocxMergerChartAxisText();
	m_val		= new DocxMergerChartValues();
} // c'tor

DocxMergerChartFieldSeries::~DocxMergerChartFieldSeries()
{
	delete m_serAxis;
	delete m_catAxis;
	delete m_val;
} // d'tor



void DocxMergerChartFieldSeries::setChartValue( const string& p_series, double p_category, double p_value )
{
	setPasteChartValue( p_series, p_category, p_value );
} // setChartValue

void DocxMergerChartFieldSeries::setChartValue( const string& p_series, const string& p_category, double p_value )
{
	setPasteChartValue( p_series, p_category, p_value );
} // setChartValue

void DocxMergerChartFieldSeries::setChartValue( double p_series, double p_category, double p_value )
{
	setPasteChartValue( p_series, p_category, p_value );
} // setChartValue

template <class Series_T, class Category_T>
void DocxMergerChartFieldSeries::setPasteChartValue( Series_T p_series, Category_T p_category, double p_value )
{
	DocxMergerPasteChartFieldSeries* l_pasteField = createPasteChartField<DocxMergerPasteChartFieldSeries>();

	size_t l_series		= m_serAxis ->setValue( ( DocxMergerPasteChartAxis* ) l_pasteField ->getPasteSerAxis(), p_series );
	size_t l_category	= m_catAxis ->setValue(	( DocxMergerPasteChartAxis* ) l_pasteField ->getPasteCatAxis(), p_category );

	m_val ->setValue(
		( DocxMergerPasteChartValues* ) l_pasteField ->getPasteVal(),
		l_series,
		l_category,
		p_value );
} // setPasteChartValue



void DocxMergerChartFieldSeries::deserialize( UnzipFile* p_unzipFile )
{
	string	l_str;
	char	l_ch;

	uint32	l_size;
	uint32	i;

	uint32	l_size2;
	uint32	i2;

	DocxMergerChartField::deserialize( p_unzipFile );

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_str	=			p_unzipFile ->readStr();
		l_ch	= ( char )	p_unzipFile ->readNum<uint8>();

		m_chartStrings.push_back( make_pair( l_str, l_ch ) );
	}

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		m_seriesStrings.push_back( list<pair<string, char>>() );

		l_size2 = p_unzipFile ->readNum<uint32>();
		for ( i2 = 0; i2 < l_size2; ++i2 )
		{
			l_str	=			p_unzipFile ->readStr();
			l_ch	= ( char )	p_unzipFile ->readNum<uint8>();

			m_seriesStrings.back().push_back( make_pair( l_str, l_ch ) );
		}
	}

	m_catType = ( DocxMergerField::FieldType ) p_unzipFile ->readNum<int16>();
	switch ( m_catType )
	{
	case DocxMergerField::TYPE_TEXT:		m_catAxis = new DocxMergerChartAxisText(); break;
	case DocxMergerField::TYPE_NUMBER:		m_catAxis = new DocxMergerChartAxisNumber(); break;
	case DocxMergerField::TYPE_DATE_TIME:	m_catAxis = new DocxMergerChartAxisDateTime(); break;
	}
} // deserialize



const list<pair<string, char>>* DocxMergerChartFieldSeries::getChartStrings() const
{
	return &m_chartStrings;
} // getChartStrings

const vector<list<pair<string, char>>>* DocxMergerChartFieldSeries::getSeriesStrings() const
{
	return &m_seriesStrings;
} // getSeriesStrings

const DocxMergerChartAxisText* DocxMergerChartFieldSeries::getSerAxis() const
{
	return m_serAxis;
} // getSerAxis

const DocxMergerChartAxis* DocxMergerChartFieldSeries::getCatAxis() const
{
	return m_catAxis;
} // getCatAxis

DocxMergerField::FieldType DocxMergerChartFieldSeries::getCatType() const
{
	return m_catType;
} // getCatType

const DocxMergerChartValues* DocxMergerChartFieldSeries::getVal() const
{
	return m_val;
} // getVal
