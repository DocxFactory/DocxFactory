
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"

using namespace DocxFactory;
using namespace std;



DocxMergerPasteField::DocxMergerPasteField( DocxMergerField* p_field )
{
	m_field = p_field;
} // c'tor

DocxMergerPasteField::~DocxMergerPasteField()
{

} // d'tor
