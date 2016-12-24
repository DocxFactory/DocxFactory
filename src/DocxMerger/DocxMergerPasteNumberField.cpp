
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteNumberField.h"

using namespace DocxFactory;
using namespace std;



DocxMergerPasteNumberField::DocxMergerPasteNumberField( DocxMergerField* p_field, double p_value ) : DocxMergerPasteField( p_field )
{
	m_value	= p_value;
} // c'tor

DocxMergerPasteNumberField::~DocxMergerPasteNumberField()
{

} // d'tor



double DocxMergerPasteNumberField::getValue() const
{
	return m_value;
} // getValue
