
#include "DocxFactory/DocxMerger/DocxMergerItemFile.h"
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerDateTimeField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteFieldGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteDateTimeField.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/UnzipFile.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/locale/DateTimeFormat.h"

#include "DocxFactory/util/DocxFactoryDefs.h"

using namespace DocxFactory;
using namespace std;



DocxMergerDateTimeField::DocxMergerDateTimeField() : DocxMergerField( DocxMergerField::TYPE_DATE_TIME )
{

} // c'tor

DocxMergerDateTimeField::~DocxMergerDateTimeField()
{
	list<DateTimeFormat*>::iterator l_dateTimeFormatIterator;
	FOR_EACH( l_dateTimeFormatIterator, &m_dateTimeFormat )
	{
		delete *l_dateTimeFormatIterator;
	}
} // d'tor



void DocxMergerDateTimeField::save( DocxMergerPasteFieldGroup* p_pasteFieldGroup )
{
	if ( !p_pasteFieldGroup )
		return;

	ZipFile* l_zipFile = m_itemFile ->getZipFile();

	map<DocxMergerField*, DocxMergerPasteField*>::const_iterator	l_pasteFieldIterator;
	const map<DocxMergerField*, DocxMergerPasteField*>*				l_pasteFields = p_pasteFieldGroup ->getPasteFieldsByField();

	DocxMergerPasteDateTimeField*	l_pasteDateTimeField;
	string							l_str;

	l_pasteFieldIterator = l_pasteFields ->find( this );
	if ( l_pasteFieldIterator != l_pasteFields ->end() )
	{
		l_pasteDateTimeField = ( DocxMergerPasteDateTimeField* ) l_pasteFieldIterator ->second;

		l_str = LocaleFunc::dateTimeToStrUtf8(
			l_pasteDateTimeField ->getTime(),
			l_pasteDateTimeField ->getTimeZone(),
			&m_dateTimeFormat );

		( *l_zipFile ) << l_str;
	}
} // save



void DocxMergerDateTimeField::setClipboardValue( const string& p_value )
{
	pair<double, short> l_value;
	
	try
	{
		l_value = LocaleFunc::strToDateTime( p_value );
	}
	
	catch ( ... )
	{
		return;
	}

	createPasteField( l_value.first, l_value.second );
} // setClipboardValue( string )

void DocxMergerDateTimeField::setClipboardValue( double p_value )
{
	createPasteField( p_value, 0 );
} // setClipboardValue( double )

void DocxMergerDateTimeField::createPasteField( double p_time, short p_timeZone )
{
	DocxMergerPasteDateTimeField*	l_pasteField;
	bool							l_ok;

	l_pasteField	= new DocxMergerPasteDateTimeField( this, p_time, p_timeZone );
	l_ok			= m_itemFile ->getPasteFieldGroup() ->insertPasteField( this, l_pasteField );

	if ( !l_ok )
		delete l_pasteField;
} // createPasteField



void DocxMergerDateTimeField::deserialize( UnzipFile* p_unzipFile )
{
	DateTimeFormat*	l_dateTimeFormat;
	uint32			l_size;
	uint32			i;

	DocxMergerField::deserialize( p_unzipFile );

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size ; ++i )
	{
		l_dateTimeFormat = new DateTimeFormat();
		l_dateTimeFormat ->m_str		=			p_unzipFile ->readStr();
		l_dateTimeFormat ->m_entry		= ( char )	p_unzipFile ->readNum<uint8>();
		l_dateTimeFormat ->m_entryLen	=			p_unzipFile ->readNum<uint16>();

		m_dateTimeFormat.push_back( l_dateTimeFormat );
	}
} // deserializeDateTimeFormats

