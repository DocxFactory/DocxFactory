
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartFieldSingleSeries.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxis.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxisText.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxisNumber.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxisDateTime.h"
#include "DocxFactory/DocxMerger/DocxMergerChartValues.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartFieldSingleSeries.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxis.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisText.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisNumber.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisDateTime.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartValues.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/UnzipFile.h"

using namespace DocxFactory;
using namespace std;



DocxMergerChartFieldSingleSeries::DocxMergerChartFieldSingleSeries() : DocxMergerChartField( DocxMergerChartField::CHART_SINGLE_SERIES )
{
	m_val = new DocxMergerChartValues();
} // c'tor

DocxMergerChartFieldSingleSeries::~DocxMergerChartFieldSingleSeries()
{
	delete m_catAxis;
	delete m_val;
} // d'tor



void DocxMergerChartFieldSingleSeries::setChartValue( const string& p_series, double p_category, double p_value )
{
	setPasteChartValue( p_category, p_value );
} // setChartValue

void DocxMergerChartFieldSingleSeries::setChartValue( const string& p_series, const string& p_category, double p_value )
{
	setPasteChartValue( p_category, p_value );
} // setChartValue

void DocxMergerChartFieldSingleSeries::setChartValue( double p_series, double p_category, double p_value )
{
	setPasteChartValue( p_category, p_value );
} // setChartValue

template <class Category_T>
void DocxMergerChartFieldSingleSeries::setPasteChartValue( Category_T p_category, double p_value )
{
	DocxMergerPasteChartFieldSingleSeries* l_pasteField = createPasteChartField<DocxMergerPasteChartFieldSingleSeries>();

	size_t l_category = m_catAxis ->setValue(
		( DocxMergerPasteChartAxis* ) l_pasteField ->getPasteCatAxis(),
		p_category );

	m_val ->setValue(
		( DocxMergerPasteChartValues* ) l_pasteField ->getPasteVal(),
		0,
		l_category,
		p_value );
} // setPasteChartValue



void DocxMergerChartFieldSingleSeries::deserialize( UnzipFile* p_unzipFile )
{
	string	l_str;
	char	l_ch;

	uint32	l_size;
	uint32	i;

	DocxMergerChartField::deserialize( p_unzipFile );

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_str	=			p_unzipFile ->readStr();
		l_ch	= ( char )	p_unzipFile ->readNum<uint8>();

		m_chartStrings.push_back( make_pair( l_str, l_ch ) );
	}

	m_catType = ( DocxMergerField::FieldType ) p_unzipFile ->readNum<int16>();
	switch ( m_catType )
	{
	case DocxMergerField::TYPE_TEXT:		m_catAxis = new DocxMergerChartAxisText(); break;
	case DocxMergerField::TYPE_NUMBER:		m_catAxis = new DocxMergerChartAxisNumber(); break;
	case DocxMergerField::TYPE_DATE_TIME:	m_catAxis = new DocxMergerChartAxisDateTime(); break;
	}
} // deserialize



const list<pair<string, char>>*	DocxMergerChartFieldSingleSeries::getChartStrings() const
{
	return &m_chartStrings;
} // getChartStrings

const DocxMergerChartAxis* DocxMergerChartFieldSingleSeries::getCatAxis() const
{
	return m_catAxis;
} // getCatAxis

DocxMergerField::FieldType DocxMergerChartFieldSingleSeries::getCatType() const
{
	return m_catType;
} // getCatType

const DocxMergerChartValues* DocxMergerChartFieldSingleSeries::getVal() const
{
	return m_val;
} // getVal
