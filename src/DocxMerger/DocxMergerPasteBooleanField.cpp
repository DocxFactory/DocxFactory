
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerBooleanField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteBooleanField.h"

using namespace DocxFactory;
using namespace std;



DocxMergerPasteBooleanField::DocxMergerPasteBooleanField( DocxMergerField* p_field, bool p_value ) : DocxMergerPasteField( p_field )
{
	m_value = p_value;
} // c'tor

DocxMergerPasteBooleanField::~DocxMergerPasteBooleanField()
{

} // d'tor



bool DocxMergerPasteBooleanField::getValue() const
{
	return m_value;
} // getValue
