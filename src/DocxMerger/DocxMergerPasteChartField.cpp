
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartField.h"

using namespace DocxFactory;
using namespace std;



DocxMergerPasteChartField::DocxMergerPasteChartField( DocxMergerField* p_field ) : DocxMergerPasteField( p_field )
{

} // c'tor

DocxMergerPasteChartField::~DocxMergerPasteChartField()
{

} // d'tor



string* DocxMergerPasteChartField::getValue()
{
	return &m_value;
} // getValue
