
#include "DocxFactory/DocxMerger/DocxMergerItemFile.h"
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerBooleanField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteFieldGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteBooleanField.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/UnzipFile.h"

#include "DocxFactory/str/StrFunc.h"

using namespace DocxFactory;
using namespace std;



DocxMergerBooleanField::DocxMergerBooleanField() : DocxMergerField( DocxMergerField::TYPE_BOOLEAN )
{

} // c'tor

DocxMergerBooleanField::~DocxMergerBooleanField()
{

} // d'tor



void DocxMergerBooleanField::save( DocxMergerPasteFieldGroup* p_pasteFieldGroup )
{
	ZipFile* l_zipFile = m_itemFile ->getZipFile();

	map<DocxMergerField*, DocxMergerPasteField*>::const_iterator	l_pasteFieldIterator;
	const map<DocxMergerField*, DocxMergerPasteField*>*				l_pasteFields = p_pasteFieldGroup ->getPasteFieldsByField();
	DocxMergerPasteBooleanField*									l_pasteBooleanField;

	if ( p_pasteFieldGroup )
	{
		l_pasteFieldIterator = l_pasteFields ->find( this );
		if ( l_pasteFieldIterator != l_pasteFields ->end() )
		{
			l_pasteBooleanField = ( DocxMergerPasteBooleanField* ) l_pasteFieldIterator ->second;

			if ( m_value == DocxMergerBooleanField::BOOLEAN_TRUE	&& l_pasteBooleanField ->getValue() == true
			  || m_value == DocxMergerBooleanField::BOOLEAN_FALSE	&& l_pasteBooleanField ->getValue() == false )
				( *l_zipFile ) << m_booleanString;
		}

		else
		if ( m_value == DocxMergerBooleanField::BOOLEAN_UNDEFINED )
			( *l_zipFile ) << m_booleanString;
	}

	else
	if ( m_value == DocxMergerBooleanField::BOOLEAN_UNDEFINED )
		( *l_zipFile ) << m_booleanString;

} // save



void DocxMergerBooleanField::setClipboardValue( const string& p_value )
{
	string l_value = StrFunc::lc( StrFunc::trim( p_value ) );

	if ( l_value == ""
	  || l_value == "0"
	  || l_value == "0.0"
	  || l_value == "no"
	  || l_value == "false" )
		createPasteField<bool, DocxMergerPasteBooleanField>( false );

	else
		createPasteField<bool, DocxMergerPasteBooleanField>( true );
} // setClipboardValue( string )

void DocxMergerBooleanField::setClipboardValue( double p_value )
{
	if ( p_value == 0.0 )
		createPasteField<bool, DocxMergerPasteBooleanField>( false );

	else
		createPasteField<bool, DocxMergerPasteBooleanField>( true );
} // setClipboardValue( double )



void DocxMergerBooleanField::deserialize( UnzipFile* p_unzipFile )
{
	DocxMergerField::deserialize( p_unzipFile );

	m_booleanString	=					p_unzipFile ->readStr();
	m_value			= ( BooleanValue )	p_unzipFile ->readNum<int16>();
} // deserialize
