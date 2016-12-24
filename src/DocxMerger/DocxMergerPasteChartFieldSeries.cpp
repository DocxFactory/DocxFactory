
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartFieldSeries.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxis.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxisText.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxisNumber.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxisDateTime.h"
#include "DocxFactory/DocxMerger/DocxMergerChartValues.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartFieldSeries.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxis.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisText.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisNumber.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisDateTime.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartValues.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/str/StrFunc.h"

using namespace DocxFactory;
using namespace std;



DocxMergerPasteChartFieldSeries::DocxMergerPasteChartFieldSeries( DocxMergerField* p_field ) : DocxMergerPasteChartField( p_field )
{
	switch ( ( ( DocxMergerChartFieldSeries* ) p_field ) ->getCatType() )
	{
	case DocxMergerField::TYPE_TEXT:		m_pasteCatAxis = new DocxMergerPasteChartAxisText(); break;
	case DocxMergerField::TYPE_NUMBER:		m_pasteCatAxis = new DocxMergerPasteChartAxisNumber(); break;
	case DocxMergerField::TYPE_DATE_TIME:	m_pasteCatAxis = new DocxMergerPasteChartAxisDateTime(); break;
	}

	m_pasteSerAxis	= new DocxMergerPasteChartAxisText();
	m_pasteVal		= new DocxMergerPasteChartValues();
} // c'tor

DocxMergerPasteChartFieldSeries::~DocxMergerPasteChartFieldSeries()
{
	delete m_pasteSerAxis;
	delete m_pasteCatAxis;
	delete m_pasteVal;
} // d'tor



string* DocxMergerPasteChartFieldSeries::getValue()
{
	list<pair<string, char>>::const_iterator	l_chartStringIterator;
	list<pair<string, char>>::const_iterator	l_seriesStringIterator;

	DocxMergerChartFieldSeries*				l_chartField	= ( DocxMergerChartFieldSeries* ) m_field;
	string									l_chartMlPrefix	= l_chartField ->getChartMlPrefix();

	const list<pair<string, char>>*			l_chartStrings	= l_chartField ->getChartStrings();
	const vector<list<pair<string, char>>>*	l_seriesStrings	= l_chartField ->getSeriesStrings();
	const list<pair<string, char>>*			l_strings;

	const DocxMergerChartAxisText*			l_serAxis		= l_chartField ->getSerAxis();
	const DocxMergerChartAxis*				l_catAxis		= l_chartField ->getCatAxis();
	const DocxMergerChartValues*			l_val			= l_chartField ->getVal();

	size_t									l_strLen		= l_seriesStrings ->size();
	size_t									l_serLen		= l_serAxis ->getLength( m_pasteSerAxis );
	size_t									l_catLen		= l_catAxis ->getLength( m_pasteCatAxis );
	size_t									l_serIdx;
	size_t									l_catIdx;

	m_value = "";

	FOR_EACH( l_chartStringIterator, l_chartStrings )
	{
		m_value += l_chartStringIterator ->first;

		switch ( l_chartStringIterator ->second )
		{
		case 's':

			for ( l_serIdx = 0; l_serIdx < l_serLen; l_serIdx++ )
			{
				l_strings = &( l_seriesStrings ->at( l_serIdx < l_strLen ? l_serIdx : l_strLen - 1 ) );

				FOR_EACH( l_seriesStringIterator, l_strings )
				{
					m_value += l_seriesStringIterator ->first;

					switch ( l_seriesStringIterator ->second )
					{
					case 'i':
					case 'o':
						m_value += StrFunc::intToStr( l_serIdx );
						break;

					case 't':
						m_value += l_serAxis ->getValue( m_pasteSerAxis, l_serIdx );
						break;

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

			break;
		}
	}

	return &m_value;
} // getValue



const DocxMergerPasteChartAxisText* DocxMergerPasteChartFieldSeries::getPasteSerAxis() const
{
	return m_pasteSerAxis;
} // getPasteSerAxis

const DocxMergerPasteChartAxis* DocxMergerPasteChartFieldSeries::getPasteCatAxis() const
{
	return m_pasteCatAxis;
} // getPasteCatAxis

const DocxMergerPasteChartValues* DocxMergerPasteChartFieldSeries::getPasteVal() const
{
	return m_pasteVal;
} // getPasteVal
