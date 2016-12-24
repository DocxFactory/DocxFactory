
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxis.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxisText.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxis.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxisText.h"

#include "DocxFactory/locale/LocaleFunc.h"

using namespace DocxFactory;
using namespace std;



DocxMergerChartAxisText::DocxMergerChartAxisText() : DocxMergerChartAxis( DocxMergerField::TYPE_TEXT )
{

} // c'tor

DocxMergerChartAxisText::~DocxMergerChartAxisText()
{

} // d'tor



size_t DocxMergerChartAxisText::setValue( DocxMergerPasteChartAxis* p_pasteAxis, const string& p_value )
{
	return ( ( DocxMergerPasteChartAxisText* ) p_pasteAxis ) ->setValue( p_value );
} // setValue

size_t DocxMergerChartAxisText::setValue( DocxMergerPasteChartAxis* p_pasteAxis, double p_value )
{
	string l_value = LocaleFunc::numToStr( p_value );

	return ( ( DocxMergerPasteChartAxisText* ) p_pasteAxis ) ->setValue( l_value );
} // setValue

string DocxMergerChartAxisText::getValue( DocxMergerPasteChartAxis* p_pasteAxis, size_t p_index ) const
{
	return ( ( DocxMergerPasteChartAxisText* ) p_pasteAxis ) ->getValues() ->at( p_index );
} // getValue

size_t DocxMergerChartAxisText::getLength( DocxMergerPasteChartAxis* p_pasteAxis ) const
{
	return ( ( DocxMergerPasteChartAxisText* ) p_pasteAxis ) ->getValues() ->size();
} // getLength
