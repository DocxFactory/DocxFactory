
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartFieldXYSize.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartFieldXYSize.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/str/StrFunc.h"

using namespace DocxFactory;
using namespace std;



DocxMergerPasteChartFieldXYSize::DocxMergerPasteChartFieldXYSize( DocxMergerField* p_field ) : DocxMergerPasteChartField( p_field )
{

} // c'tor

DocxMergerPasteChartFieldXYSize::~DocxMergerPasteChartFieldXYSize()
{

} // d'tor



string* DocxMergerPasteChartFieldXYSize::getValue()
{
	list<pair<string, char>>::const_iterator l_stringIterator;

	DocxMergerChartFieldXYSize*		l_chartField	= ( DocxMergerChartFieldXYSize* ) m_field;
	string							l_chartMlPrefix	= l_chartField ->getChartMlPrefix();
	const list<pair<string, char>>*	l_chartStrings	= l_chartField ->getChartStrings();

	const vector<double>*			l_pasteAxis;
	size_t							l_len;
	size_t							l_idx;

	m_value = "";

	FOR_EACH( l_stringIterator, l_chartStrings )
	{
		m_value += l_stringIterator ->first;

		if ( l_stringIterator ->second )
		{
			switch ( l_stringIterator ->second )
			{
			case 'x': l_pasteAxis = &m_pasteXAxis; break;
			case 'y': l_pasteAxis = &m_pasteYAxis; break;
			case 's': l_pasteAxis = &m_pasteSizeAxis; break;
			}

			l_len	 = l_pasteAxis ->size();
			m_value	+= "<" + l_chartMlPrefix + ":ptCount val=\"" + StrFunc::intToStr( l_len ) + "\" />";

			for ( l_idx = 0; l_idx < l_len; l_idx++ )
			{
				m_value +=
					  "<"	+ l_chartMlPrefix + ":pt idx=\"" + StrFunc::intToStr( l_idx ) + "\">"
					+ "<"	+ l_chartMlPrefix + ":v>"
					+ StrFunc::intToStr( l_pasteAxis ->at( l_idx ) )
					+ "</"	+ l_chartMlPrefix + ":v>"
					+ "</"	+ l_chartMlPrefix + ":pt>";
			}
		}
	}

	return &m_value;
} // getValue



void DocxMergerPasteChartFieldXYSize::pushBack( double p_x, double p_y, double p_size )
{
	m_pasteXAxis.push_back( p_x );
	m_pasteYAxis.push_back( p_y );
	m_pasteSizeAxis.push_back( p_size );
} // pushBack



const vector<double>* DocxMergerPasteChartFieldXYSize::getPasteXAxis() const
{
	return &m_pasteXAxis;
} // getPasteXAxis

const vector<double>* DocxMergerPasteChartFieldXYSize::getPasteYAxis() const
{
	return &m_pasteYAxis;
} // getPasteYAxis

const vector<double>* DocxMergerPasteChartFieldXYSize::getPasteSizeAxis() const
{
	return &m_pasteSizeAxis;
} // getPasteSizeAxis
