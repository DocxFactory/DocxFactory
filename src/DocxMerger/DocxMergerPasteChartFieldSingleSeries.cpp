
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartFieldSingleSeries.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxis.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxisText.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxisNumber.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxisDateTime.h"
#include "DocxFactory/DocxMerger/DocxMergerChartValues.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartFieldSingleSeries.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxis.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisText.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisNumber.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisDateTime.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartValues.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/str/StrFunc.h"

using namespace DocxFactory;
using namespace std;



DocxMergerPasteChartFieldSingleSeries::DocxMergerPasteChartFieldSingleSeries( DocxMergerField* p_field ) : DocxMergerPasteChartField( p_field )
{
	switch ( ( ( DocxMergerChartFieldSingleSeries* ) p_field ) ->getCatType() )
	{
	case DocxMergerField::TYPE_TEXT:		m_pasteCatAxis = new DocxMergerPasteChartAxisText(); break;
	case DocxMergerField::TYPE_NUMBER:		m_pasteCatAxis = new DocxMergerPasteChartAxisNumber(); break;
	case DocxMergerField::TYPE_DATE_TIME:	m_pasteCatAxis = new DocxMergerPasteChartAxisDateTime(); break;
	}

	m_pasteVal = new DocxMergerPasteChartValues();
} // c'tor

DocxMergerPasteChartFieldSingleSeries::~DocxMergerPasteChartFieldSingleSeries()
{
	delete m_pasteCatAxis;
	delete m_pasteVal;
} // d'tor


string* DocxMergerPasteChartFieldSingleSeries::getValue()
{
	list<pair<string, char>>::const_iterator l_stringIterator;

	DocxMergerChartFieldSingleSeries*	l_chartField	= ( DocxMergerChartFieldSingleSeries* ) m_field;
	string								l_chartMlPrefix	= l_chartField ->getChartMlPrefix();

	const list<pair<string, char>>*		l_chartStrings	= l_chartField ->getChartStrings();
	const DocxMergerChartAxis*			l_catAxis		= l_chartField ->getCatAxis();
	const DocxMergerChartValues*		l_val			= l_chartField ->getVal();

	size_t								l_len			= l_catAxis ->getLength( m_pasteCatAxis );
	size_t								l_idx;

	m_value = "";

	FOR_EACH( l_stringIterator, l_chartStrings )
	{
		m_value += l_stringIterator ->first;

		switch ( l_stringIterator ->second )
		{
		case 'c':
			m_value += "<" + l_chartMlPrefix + ":ptCount val=\"" + StrFunc::intToStr( l_len ) + "\" />";

			for ( l_idx = 0; l_idx < l_len; l_idx++ )
			{
				m_value +=
					  "<"	+ l_chartMlPrefix + ":pt idx=\"" + StrFunc::intToStr( l_idx ) + "\">"
					+ "<"	+ l_chartMlPrefix + ":v>"
					+ l_catAxis ->getValue( m_pasteCatAxis, l_idx )
					+ "</"	+ l_chartMlPrefix + ":v>"
					+ "</"	+ l_chartMlPrefix + ":pt>";
			}

			break;

		case 'v':
			m_value += "<" + l_chartMlPrefix + ":ptCount val=\"" + StrFunc::intToStr( l_len ) + "\" />";

			for ( l_idx = 0; l_idx < l_len; l_idx++ )
			{
				m_value +=
					  "<"	+ l_chartMlPrefix + ":pt idx=\"" + StrFunc::intToStr( l_idx ) + "\">"
					+ "<"	+ l_chartMlPrefix + ":v>"
					+ l_val ->getValue( m_pasteVal, 0, l_idx )
					+ "</"	+ l_chartMlPrefix + ":v>"
					+ "</"	+ l_chartMlPrefix + ":pt>";
			}

			break;
		}
	}

	return &m_value;
} // getValue



const DocxMergerPasteChartAxis* DocxMergerPasteChartFieldSingleSeries::getPasteCatAxis() const
{
	return m_pasteCatAxis;
} // getPasteCatAxis

const DocxMergerPasteChartValues* DocxMergerPasteChartFieldSingleSeries::getPasteVal() const
{
	return m_pasteVal;
} // getPasteVal
