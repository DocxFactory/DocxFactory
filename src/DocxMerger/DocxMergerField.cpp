
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteFieldGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"

#include "DocxFactory/zip/UnzipFile.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/str/StrFunc.h"

using namespace DocxFactory;
using namespace std;



DocxMergerField::DocxMergerField( FieldType p_type )
{
	m_type = p_type;
} // c'tor

DocxMergerField::~DocxMergerField()
{

} // d'tor



void DocxMergerField::save( DocxMergerPasteFieldGroup* p_pasteFieldGroup )
{

} // save



void DocxMergerField::setClipboardValue( const string& p_value )
{

} // setClipboardValue( string )

void DocxMergerField::setClipboardValue( double p_value )
{

} // setClipboardValue( double )



void DocxMergerField::deserialize( UnzipFile* p_unzipFile )
{
	p_unzipFile ->insertPtrBySeq( p_unzipFile ->readNum<uint32>(), this );

	m_item		= ( DocxMergerItem* )	p_unzipFile ->readNum<uint32>();
	m_name		=						p_unzipFile ->readStr();
} // deserialize

void DocxMergerField::link( UnzipFile* p_unzipFile )
{
	const map<uint32, void*>*	l_ptrsBySeq = p_unzipFile ->getPtrsBySeq();
	uint32						l_ptrSeq;

	l_ptrSeq	= ( uint32 ) m_item;
	m_item		= ( DocxMergerItem* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;
	m_itemFile	= m_item ->getItemFile();
	m_file		= m_itemFile ->getFile();
} // link



DocxMergerItemFile* DocxMergerField::getItemFile() const
{
	return m_itemFile;
} // getItemFile

DocxMergerItem* DocxMergerField::getItem() const
{
	return m_item;
} // getItem

DocxMergerField::FieldType DocxMergerField::getType() const
{
	return m_type;
} // getType

string DocxMergerField::getName() const
{
	return m_name;
} // getName
