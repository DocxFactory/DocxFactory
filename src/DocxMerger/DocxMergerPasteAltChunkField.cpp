
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerAltChunkField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteAltChunkField.h"

using namespace DocxFactory;
using namespace std;



DocxMergerPasteAltChunkField::DocxMergerPasteAltChunkField( DocxMergerField* p_field, const string& p_value, DocxMergerAltChunkField::AltChunkType p_type ) : DocxMergerPasteField( p_field )
{
	m_value	= p_value;
	m_type	= p_type;
} // c'tor

DocxMergerPasteAltChunkField::~DocxMergerPasteAltChunkField()
{

} // d'tor



const string* DocxMergerPasteAltChunkField::getValue() const
{
	return &m_value;
} // getValue

DocxMergerAltChunkField::AltChunkType DocxMergerPasteAltChunkField::getType() const
{
	return m_type;
} // getType