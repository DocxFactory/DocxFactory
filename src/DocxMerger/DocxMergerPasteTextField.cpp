
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteTextField.h"

using namespace DocxFactory;
using namespace std;



DocxMergerPasteTextField::DocxMergerPasteTextField( DocxMergerField* p_field, const string& p_value ) : DocxMergerPasteField( p_field )
{
	m_value = p_value;
} // c'tor

DocxMergerPasteTextField::~DocxMergerPasteTextField()
{

} // d'tor



const string* DocxMergerPasteTextField::getValue() const
{
	return &m_value;
} // getValue



