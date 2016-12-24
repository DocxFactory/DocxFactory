
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteFieldGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"

using namespace DocxFactory;
using namespace std;



DocxMergerPasteFieldGroup::DocxMergerPasteFieldGroup()
{

} // c'tor

DocxMergerPasteFieldGroup::~DocxMergerPasteFieldGroup()
{
	map<DocxMergerField*, DocxMergerPasteField*>::iterator l_pasteFieldIterator;
	FOR_EACH( l_pasteFieldIterator, &m_pasteFieldsByField )
	{
		delete l_pasteFieldIterator ->second;
	}
} // d'tor



bool DocxMergerPasteFieldGroup::insertPasteField( DocxMergerField* p_field, DocxMergerPasteField* p_pasteField )
{
	pair<map<DocxMergerField*, DocxMergerPasteField*>::iterator, bool> l_retVal;

	l_retVal = m_pasteFieldsByField.insert( make_pair( p_field, p_pasteField ) );

	return l_retVal.second;
} // insertPasteField



const map<DocxMergerField*, DocxMergerPasteField*>* DocxMergerPasteFieldGroup::getPasteFieldsByField() const
{
	return &m_pasteFieldsByField;
} // getPasteFieldsByField
