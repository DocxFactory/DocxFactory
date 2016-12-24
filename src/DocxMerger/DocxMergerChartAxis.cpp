
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartAxis.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxis.h"

using namespace DocxFactory;
using namespace std;



DocxMergerChartAxis::DocxMergerChartAxis( DocxMergerField::FieldType p_axisType )
{

} // c'tor

DocxMergerChartAxis::~DocxMergerChartAxis()
{

} // d'tor



size_t DocxMergerChartAxis::setValue( DocxMergerPasteChartAxis* p_pasteAxis, const string& p_value )
{
	return 0;
} // setValue

size_t DocxMergerChartAxis::setValue( DocxMergerPasteChartAxis* p_pasteAxis, double p_value )
{
	return 0;
} // setValue

string DocxMergerChartAxis::getValue( DocxMergerPasteChartAxis* p_pasteAxis, size_t p_index ) const
{
	return "";
} // getValue

size_t DocxMergerChartAxis::getLength( DocxMergerPasteChartAxis* p_pasteAxis ) const
{
	return 0;
} // getLength
