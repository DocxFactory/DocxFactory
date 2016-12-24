
#include "DocxFactory/DocxCompiler/DocxCompilerDateTimeField.h"

#include "DocxFactory/zip/ZipFile.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/locale/DateTimeFormat.h"

#include "DocxFactory/util/DocxFactoryDefs.h"

using namespace DocxFactory;
using namespace std;



DocxCompilerDateTimeField::DocxCompilerDateTimeField(
	DocxCompilerItem*				p_item,
	const string&					p_name,
	const string&					p_format,
	const map<FieldParam, string>*	p_valueByParam,
	xercesc::DOMElement*			p_placeHolderNode )
	
	: DocxCompilerField( 
		p_item,
		TYPE_DATE_TIME,
		p_name,
		p_format,
		p_valueByParam,
		p_placeHolderNode )
{
	LocaleFunc::getDateTimeFormat( p_format, &m_dateTimeFormat );
} // c'tor

DocxCompilerDateTimeField::~DocxCompilerDateTimeField()
{
	list<DateTimeFormat*>::iterator l_dateTimeIterator;
	FOR_EACH( l_dateTimeIterator, &m_dateTimeFormat )
	{
		delete *l_dateTimeIterator;
	}
} // d'tor



void DocxCompilerDateTimeField::serialize( ZipFile* p_zipFile )
{
	list<DateTimeFormat*>::iterator l_dateTimeIterator;
	DateTimeFormat*					l_dateTimeFormat;

	DocxCompilerField::serialize( p_zipFile );

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_dateTimeFormat.size() );
	FOR_EACH( l_dateTimeIterator, &m_dateTimeFormat )
	{
		l_dateTimeFormat = *l_dateTimeIterator;

		p_zipFile ->writeStr		( l_dateTimeFormat ->m_str );
		p_zipFile ->writeNum<uint8>	( l_dateTimeFormat ->m_entry );
		p_zipFile ->writeNum<uint16>( l_dateTimeFormat ->m_entryLen );
	}
} // serialize
