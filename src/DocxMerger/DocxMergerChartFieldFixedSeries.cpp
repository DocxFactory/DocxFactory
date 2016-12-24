
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartFieldFixedSeries.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxis.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxisText.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxisNumber.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxisDateTime.h"
#include "DocxFactory/DocxMerger/DocxMergerChartValues.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartFieldFixedSeries.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxis.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisText.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisNumber.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisDateTime.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartValues.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/UnzipFile.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/str/StrFunc.h"

using namespace DocxFactory;
using namespace std;



DocxMergerChartFieldFixedSeries::DocxMergerChartFieldFixedSeries() : DocxMergerChartField( DocxMergerChartField::CHART_FIXED_SERIES )
{
	m_val = new DocxMergerChartValues();
} // c'tor

DocxMergerChartFieldFixedSeries::~DocxMergerChartFieldFixedSeries()
{
	delete m_catAxis;
	delete m_val;
} // d'tor



void DocxMergerChartFieldFixedSeries::setChartValue( const string& p_series, double p_category, double p_value )
{
	setPasteChartValue( p_series, p_category, p_value );
} // setChartValue

void DocxMergerChartFieldFixedSeries::setChartValue( const string& p_series, const string& p_category, double p_value )
{
	setPasteChartValue( p_series, p_category, p_value );
} // setChartValue

void DocxMergerChartFieldFixedSeries::setChartValue( double p_series, double p_category, double p_value )
{
	setPasteChartValue( LocaleFunc::numToStr( p_series ), p_category, p_value );
} // setChartValue

template <class Category_T>
void DocxMergerChartFieldFixedSeries::setPasteChartValue( const string& p_series, Category_T p_category, double p_value )
{
	DocxMergerPasteChartFieldFixedSeries*	l_pasteField = createPasteChartField();
	map<string, char>::iterator				l_seriesIdxIterator;

	l_seriesIdxIterator = m_seriesIdx.find( StrFunc::lc( p_series ) );
	if ( l_seriesIdxIterator == m_seriesIdx.end() )
		return;

	size_t	l_series = ( size_t ) l_seriesIdxIterator ->second;
	size_t	l_category;

	l_category = m_catAxis ->setValue(
		( DocxMergerPasteChartAxis* ) l_pasteField ->getPasteCatAxis(),
		p_category );

	m_val ->setValue(
		( DocxMergerPasteChartValues* ) l_pasteField ->getPasteVal(),
		l_series,
		l_category,
		p_value );
} // setPasteChartValue

DocxMergerPasteChartFieldFixedSeries* DocxMergerChartFieldFixedSeries::createPasteChartField()
{
	DocxMergerPasteFieldGroup*										l_group;
	map<DocxMergerField*, DocxMergerPasteField*>::const_iterator	l_iterator;
	DocxMergerPasteChartFieldFixedSeries*							l_pasteField;

	l_group		= m_itemFile ->getPasteFieldGroup();
	l_iterator	= l_group ->getPasteFieldsByField() ->find( this );

	if( l_iterator != l_group ->getPasteFieldsByField() ->cend() )
		l_pasteField = ( DocxMergerPasteChartFieldFixedSeries* ) l_iterator ->second;

	else
	{
		l_pasteField = new DocxMergerPasteChartFieldFixedSeries( this, m_seriesIdx.size() );
		l_group ->insertPasteField( this, l_pasteField );
	}

	return l_pasteField;
} // createPasteChartField



void DocxMergerChartFieldFixedSeries::deserialize( UnzipFile* p_unzipFile )
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

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_str	=			p_unzipFile ->readStr();
		l_ch	= ( char )	p_unzipFile ->readNum<uint8>();

		m_seriesIdx.insert( make_pair( l_str, l_ch ) );
	}

	m_catType = ( DocxMergerField::FieldType ) p_unzipFile ->readNum<int16>();
	switch ( m_catType )
	{
	case DocxMergerField::TYPE_TEXT:		m_catAxis = new DocxMergerChartAxisText(); break;
	case DocxMergerField::TYPE_NUMBER:		m_catAxis = new DocxMergerChartAxisNumber(); break;
	case DocxMergerField::TYPE_DATE_TIME:	m_catAxis = new DocxMergerChartAxisDateTime(); break;
	}
} // deserialize



const list<pair<string, char>>* DocxMergerChartFieldFixedSeries::getChartStrings() const
{
	return &m_chartStrings;
} // getChartStrings

const vector<list<pair<string, char>>>*	DocxMergerChartFieldFixedSeries::getSeriesStrings() const
{
	return &m_seriesStrings;
} // getSeriesStrings

const map<string, char>* DocxMergerChartFieldFixedSeries::getSeriesIdx() const
{
	return &m_seriesIdx;
} // getSeriesIdx

const DocxMergerChartAxis* DocxMergerChartFieldFixedSeries::getCatAxis() const
{
	return m_catAxis;
} // getCatAxis

DocxMergerField::FieldType DocxMergerChartFieldFixedSeries::getCatType() const
{
	return m_catType;
} // getCatType

const DocxMergerChartValues* DocxMergerChartFieldFixedSeries::getVal() const
{
	return m_val;
} // getVal
