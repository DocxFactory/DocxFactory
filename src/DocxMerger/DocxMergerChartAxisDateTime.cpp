
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxis.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxisDateTime.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxis.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisDateTime.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/str/StrFunc.h"

using namespace DocxFactory;
using namespace std;



DocxMergerChartAxisDateTime::DocxMergerChartAxisDateTime() : DocxMergerChartAxis( DocxMergerField::TYPE_DATE_TIME )
{

} // c'tor

DocxMergerChartAxisDateTime::~DocxMergerChartAxisDateTime()
{

} // d'tor



size_t DocxMergerChartAxisDateTime::setValue( DocxMergerPasteChartAxis* p_pasteAxis, const string& p_value )
{
	pair<double, short> l_value;
	
	try
	{
		l_value = LocaleFunc::strToDateTime( p_value );
	}
	
	catch ( ... )
	{
		l_value.first	= 0.0f;
		l_value.second	= 0;
	}

	return ( ( DocxMergerPasteChartAxisDateTime* ) p_pasteAxis ) ->setValue( l_value.first );
} // setValue

size_t DocxMergerChartAxisDateTime::setValue( DocxMergerPasteChartAxis* p_pasteAxis, double p_value )
{
	return ( ( DocxMergerPasteChartAxisDateTime* ) p_pasteAxis ) ->setValue( p_value );
} // setValue

string DocxMergerChartAxisDateTime::getValue( DocxMergerPasteChartAxis* p_pasteAxis, size_t p_index ) const
{
	return StrFunc::intToStr(
		( ( DocxMergerPasteChartAxisDateTime* ) p_pasteAxis ) ->getValues() ->at( p_index ) / 86400.0 + 25569.0	);
} // getValue

size_t DocxMergerChartAxisDateTime::getLength( DocxMergerPasteChartAxis* p_pasteAxis ) const
{
	return ( ( DocxMergerPasteChartAxisDateTime* ) p_pasteAxis ) ->getValues() ->size();
} // getLength
