
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartValues.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartValues.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/str/StrFunc.h"

#include <vector>

using namespace DocxFactory;
using namespace std;



DocxMergerChartValues::DocxMergerChartValues()
{

} // c'tor

DocxMergerChartValues::~DocxMergerChartValues()
{

} // d'tor



void DocxMergerChartValues::setValue( DocxMergerPasteChartValues* p_pasteValues, size_t p_series, size_t p_category, const string& p_value )
{
	double l_value = LocaleFunc::strToNum( p_value );

	p_pasteValues ->setValue( p_series, p_category, l_value );
} // setValue

void DocxMergerChartValues::setValue( DocxMergerPasteChartValues* p_pasteValues, size_t p_series, size_t p_category, double p_value )
{
	p_pasteValues ->setValue( p_series, p_category, p_value );
} // setValue

string DocxMergerChartValues::getValue( DocxMergerPasteChartValues* p_pasteValues, size_t p_series, size_t p_category ) const
{
	return StrFunc::intToStr( ( *p_pasteValues ->getValues() )[ p_series ][ p_category ] );
} // getValue
