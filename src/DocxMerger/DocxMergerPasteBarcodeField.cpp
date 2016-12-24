
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerBarcodeField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteBarcodeField.h"

using namespace DocxFactory;
using namespace std;



DocxMergerPasteBarcodeField::DocxMergerPasteBarcodeField( DocxMergerField* p_field, const string& p_value ) : DocxMergerPasteField( p_field )
{
	m_value = p_value;
} // c'tor

DocxMergerPasteBarcodeField::~DocxMergerPasteBarcodeField()
{

} // d'tor



const string* DocxMergerPasteBarcodeField::getValue()
{
	return &m_value;
} // getValue
