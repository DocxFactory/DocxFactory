
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartFieldXYSize.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartFieldXYSize.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/UnzipFile.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/str/StrFunc.h"

using namespace DocxFactory;
using namespace std;



DocxMergerChartFieldXYSize::DocxMergerChartFieldXYSize() : DocxMergerChartField( DocxMergerChartField::CHART_X_Y_SIZE )
{

} // c'tor

DocxMergerChartFieldXYSize::~DocxMergerChartFieldXYSize()
{

} // d'tor



void DocxMergerChartFieldXYSize::setChartValue( const string& p_series, const string& p_category, double p_value )
{
	double l_series		= LocaleFunc::strToNum( p_series );
	double l_category	= LocaleFunc::strToNum( p_category );

	setPasteChartValue( l_series, l_category, p_value );
} // setChartValue

void DocxMergerChartFieldXYSize::setChartValue( const string& p_series, double p_category, double p_value )
{
	double l_series = LocaleFunc::strToNum( p_series );

	setPasteChartValue( l_series, p_category, p_value );
} // setChartValue

void DocxMergerChartFieldXYSize::setChartValue( double p_series, double p_category, double p_value )
{
	setPasteChartValue( p_series, p_category, p_value );
} // setChartValue

void DocxMergerChartFieldXYSize::setPasteChartValue( double p_x, double p_y, double p_size )
{
	DocxMergerPasteChartFieldXYSize* l_pasteField = createPasteChartField<DocxMergerPasteChartFieldXYSize>();

	l_pasteField ->pushBack( p_x, p_y, p_size );
} // setPasteChartValue



void DocxMergerChartFieldXYSize::deserialize( UnzipFile* p_unzipFile )
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



const list<pair<string, char>>* DocxMergerChartFieldXYSize::getChartStrings() const
{
	return &m_chartStrings;
} // getChartStrings
