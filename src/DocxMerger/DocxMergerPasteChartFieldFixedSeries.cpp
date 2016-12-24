
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartFieldFixedSeries.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxis.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxisText.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxisNumber.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxisDateTime.h"
#include "DocxFactory/DocxMerger/DocxMergerChartValues.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartFieldFixedSeries.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxis.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisText.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisNumber.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisDateTime.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartValues.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/str/StrFunc.h"

using namespace DocxFactory;
using namespace std;



DocxMergerPasteChartFieldFixedSeries::DocxMergerPasteChartFieldFixedSeries( DocxMergerField* p_field, size_t p_serLength ) : DocxMergerPasteChartField( p_field )
{
	switch ( ( ( DocxMergerChartFieldFixedSeries* ) p_field ) ->getCatType() )
	{
	case DocxMergerField::TYPE_TEXT:		m_pasteCatAxis = new DocxMergerPasteChartAxisText(); break;
	case DocxMergerField::TYPE_NUMBER:		m_pasteCatAxis = new DocxMergerPasteChartAxisNumber(); break;
	case DocxMergerField::TYPE_DATE_TIME:	m_pasteCatAxis = new DocxMergerPasteChartAxisDateTime(); break;
	}

	m_pasteVal = new DocxMergerPasteChartValues( p_serLength, 1 );
} // c'tor

DocxMergerPasteChartFieldFixedSeries::~DocxMergerPasteChartFieldFixedSeries()
{
	delete m_pasteCatAxis;
	delete m_pasteVal;
} // d'tor



string* DocxMergerPasteChartFieldFixedSeries::getValue()
{
	list<pair<string, char>>::const_iterator	l_chartStringIterator;
	list<pair<string, char>>::const_iterator	l_seriesStringIterator;

	DocxMergerChartFieldFixedSeries*		l_chartField	= ( DocxMergerChartFieldFixedSeries* ) m_field;
	string									l_chartMlPrefix	= l_chartField ->getChartMlPrefix();

	const list<pair<string, char>>*			l_chartStrings	= l_chartField ->getChartStrings();
	const vector<list<pair<string, char>>>*	l_seriesStrings	= l_chartField ->getSeriesStrings();
	const map<string, char>*				l_seriesIdx		= l_chartField ->getSeriesIdx();
	const list<pair<string, char>>*			l_strings;

	const DocxMergerChartAxis*				l_catAxis		= l_chartField ->getCatAxis();
	const DocxMergerChartValues*			l_val			= l_chartField ->getVal();

	size_t									l_serLen		= l_seriesIdx ->size();
	size_t									l_catLen		= l_catAxis ->getLength( m_pasteCatAxis );
	size_t									l_serIdx;
	size_t									l_catIdx;

	m_value = "";

	FOR_EACH( l_chartStringIterator, l_chartStrings )
	{
		m_value += l_chartStringIterator ->first;

		if ( l_chartStringIterator ->second >= '0' 
		  && l_chartStringIterator ->second <= '9' )
		{
			l_serIdx	= ( size_t ) ( l_chartStringIterator ->second - '0' );
			l_strings	= &( l_seriesStrings ->at( l_serIdx ) );

			FOR_EACH( l_seriesStringIterator, l_strings )
			{
				m_value += l_seriesStringIterator ->first;

				switch ( l_seriesStringIterator ->second )
				{
				case 'c':
					m_value += "<" + l_chartMlPrefix + ":ptCount val=\"" + StrFunc::intToStr( l_catLen ) + "\" />";

					for ( l_catIdx = 0; l_catIdx < l_catLen; l_catIdx++ )
					{
						m_value +=
							  "<"	+ l_chartMlPrefix + ":pt idx=\"" + StrFunc::intToStr( l_catIdx ) + "\">"
							+ "<"	+ l_chartMlPrefix + ":v>"
							+ l_catAxis ->getValue( m_pasteCatAxis, l_catIdx )
							+ "</"	+ l_chartMlPrefix + ":v>"
							+ "</"	+ l_chartMlPrefix + ":pt>";
					}

					break;

				case 'v':
					m_value += "<" + l_chartMlPrefix + ":ptCount val=\"" + StrFunc::intToStr( l_catLen ) + "\" />";

					for ( l_catIdx = 0; l_catIdx < l_catLen; l_catIdx++ )
					{
						m_value +=
							  "<"	+ l_chartMlPrefix + ":pt idx=\"" + StrFunc::intToStr( l_catIdx ) + "\">"
							+ "<"	+ l_chartMlPrefix + ":v>"
							+ l_val ->getValue( m_pasteVal, l_serIdx, l_catIdx )
							+ "</"	+ l_chartMlPrefix + ":v>"
							+ "</"	+ l_chartMlPrefix + ":pt>";
					}

					break;
				}
			}
		}
	}

	return &m_value;
} // getValue



const DocxMergerPasteChartAxis* DocxMergerPasteChartFieldFixedSeries::getPasteCatAxis() const
{
	return m_pasteCatAxis;
} // getPasteCatAxis

const DocxMergerPasteChartValues* DocxMergerPasteChartFieldFixedSeries::getPasteVal() const
{
	return m_pasteVal;
} // getPasteVal
