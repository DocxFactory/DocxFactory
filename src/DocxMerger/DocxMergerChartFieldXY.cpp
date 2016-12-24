
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartFieldXY.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartFieldXY.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/UnzipFile.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/str/StrFunc.h"

using namespace DocxFactory;
using namespace std;



DocxMergerChartFieldXY::DocxMergerChartFieldXY() : DocxMergerChartField( DocxMergerChartField::CHART_X_Y )
{

} // c'tor

DocxMergerChartFieldXY::~DocxMergerChartFieldXY()
{

} // d'tor



void DocxMergerChartFieldXY::setChartValue( const string& p_series, const string& p_category, double p_value )
{
	double l_series		= LocaleFunc::strToNum( p_series );
	double l_category	= LocaleFunc::strToNum( p_category );

	setPasteChartValue( l_series, l_category );
} // setChartValue

void DocxMergerChartFieldXY::setChartValue( const string& p_series, double p_category, double p_value )
{
	double l_series		= LocaleFunc::strToNum( p_series );

	setPasteChartValue( l_series, p_category );
} // setChartValue

void DocxMergerChartFieldXY::setChartValue( double p_series, double p_category, double p_value )
{
	setPasteChartValue( p_series, p_category );
} // setChartValue

void DocxMergerChartFieldXY::setPasteChartValue( double p_x, double p_y )
{
	DocxMergerPasteChartFieldXY* l_pasteField = createPasteChartField<DocxMergerPasteChartFieldXY>();

	l_pasteField ->pushBack( p_x, p_y );
} // setPasteChartValue



void DocxMergerChartFieldXY::deserialize( UnzipFile* p_unzipFile )
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
} // deserialize



const list<pair<string, char>>* DocxMergerChartFieldXY::getChartStrings() const
{
	return &m_chartStrings;
} // getChartStrings
