
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxis.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxisNumber.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxis.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisNumber.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/str/StrFunc.h"

using namespace DocxFactory;
using namespace std;



DocxMergerChartAxisNumber::DocxMergerChartAxisNumber() : DocxMergerChartAxis( DocxMergerField::TYPE_NUMBER )
{

} // c'tor

DocxMergerChartAxisNumber::~DocxMergerChartAxisNumber()
{

} // d'tor



size_t DocxMergerChartAxisNumber::setValue( DocxMergerPasteChartAxis* p_pasteAxis, const string& p_value )
{
	double l_value = LocaleFunc::strToNum( p_value );

	return ( ( DocxMergerPasteChartAxisNumber* ) p_pasteAxis ) ->setValue( l_value );
} // setValue

size_t DocxMergerChartAxisNumber::setValue( DocxMergerPasteChartAxis* p_pasteAxis, double p_value )
{
	return ( ( DocxMergerPasteChartAxisNumber* ) p_pasteAxis ) ->setValue( p_value );
} // setValue

string DocxMergerChartAxisNumber::getValue( DocxMergerPasteChartAxis* p_pasteAxis, size_t p_index ) const
{
 	return StrFunc::intToStr( ( ( DocxMergerPasteChartAxisNumber* ) p_pasteAxis ) ->getValues() ->at( p_index ) );
} // getValue

size_t DocxMergerChartAxisNumber::getLength( DocxMergerPasteChartAxis* p_pasteAxis ) const
{
	return ( ( DocxMergerPasteChartAxisNumber* ) p_pasteAxis ) ->getValues() ->size();
} // getLength
