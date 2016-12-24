
#include "DocxFactory/DocxMerger/DocxMergerItemFile.h"
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerNumberField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteFieldGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteNumberField.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/UnzipFile.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/locale/NumFormat.h"
#include "DocxFactory/str/StrFunc.h"

using namespace DocxFactory;
using namespace std;



DocxMergerNumberField::DocxMergerNumberField() : DocxMergerField( DocxMergerField::TYPE_NUMBER )
{
	m_numFormat = new NumFormat();
} // c'tor

DocxMergerNumberField::~DocxMergerNumberField()
{
	delete m_numFormat;
} // d'tor



void DocxMergerNumberField::save( DocxMergerPasteFieldGroup* p_pasteFieldGroup )
{
	if ( !p_pasteFieldGroup )
		return;

	ZipFile* l_zipFile = m_itemFile ->getZipFile();

	map<DocxMergerField*, DocxMergerPasteField*>::const_iterator	l_pasteFieldIterator;
	const map<DocxMergerField*, DocxMergerPasteField*>*				l_pasteFields = p_pasteFieldGroup ->getPasteFieldsByField();

	DocxMergerPasteNumberField*	l_pasteNumberField;
	string						l_str;

	l_pasteFieldIterator = l_pasteFields ->find( this );
	if ( l_pasteFieldIterator != l_pasteFields ->end() )
	{
		l_pasteNumberField = ( DocxMergerPasteNumberField* ) l_pasteFieldIterator ->second;

		l_str = LocaleFunc::numToStr(
			l_pasteNumberField ->getValue(),
			m_numFormat );

		( *l_zipFile ) << l_str;
	}
} // save



void DocxMergerNumberField::setClipboardValue( const string& p_value )
{
	double l_value = LocaleFunc::strToNum( p_value );

	createPasteField<double, DocxMergerPasteNumberField>( l_value );
} // setClipboardValue( string )

void DocxMergerNumberField::setClipboardValue( double p_value )
{
	createPasteField<double, DocxMergerPasteNumberField>( p_value );
} // setClipboardValue( double )



void DocxMergerNumberField::deserialize( UnzipFile* p_unzipFile )
{
	DocxMergerField::deserialize( p_unzipFile );

	m_numFormat ->m_posColor		= p_unzipFile ->readStr();
	m_numFormat ->m_posPrefix		= p_unzipFile ->readStr();
	m_numFormat ->m_posLeftDigCnt	= p_unzipFile ->readNum<uint16>();
	m_numFormat ->m_posRightDigCnt	= p_unzipFile ->readNum<uint16>();	
	m_numFormat ->m_posRightExtCnt	= p_unzipFile ->readNum<uint16>();		
	m_numFormat ->m_posSuffix		= p_unzipFile ->readStr();
	m_numFormat ->m_posSep			= p_unzipFile ->readNum<uint8>() != 0;
	m_numFormat ->m_posPercent		= p_unzipFile ->readNum<uint8>() != 0;

	m_numFormat ->m_negColor		= p_unzipFile ->readStr();
	m_numFormat ->m_negPrefix		= p_unzipFile ->readStr();
	m_numFormat ->m_negLeftDigCnt	= p_unzipFile ->readNum<uint16>();	
	m_numFormat ->m_negRightDigCnt	= p_unzipFile ->readNum<uint16>();	
	m_numFormat ->m_negRightExtCnt	= p_unzipFile ->readNum<uint16>();
	m_numFormat ->m_negSuffix		= p_unzipFile ->readStr();
	m_numFormat ->m_negSep			= p_unzipFile ->readNum<uint8>() != 0;
	m_numFormat ->m_negPercent		= p_unzipFile ->readNum<uint8>() != 0;

	m_numFormat ->m_zeroColor		= p_unzipFile ->readStr();	
	m_numFormat ->m_zeroPrefix		= p_unzipFile ->readStr();
	m_numFormat ->m_zeroLeftDigCnt	= p_unzipFile ->readNum<uint16>();
	m_numFormat ->m_zeroRightDigCnt	= p_unzipFile ->readNum<uint16>();
	m_numFormat ->m_zeroSuffix		= p_unzipFile ->readStr();
	m_numFormat ->m_zeroSep			= p_unzipFile ->readNum<uint8>() != 0;
} // deserialize
