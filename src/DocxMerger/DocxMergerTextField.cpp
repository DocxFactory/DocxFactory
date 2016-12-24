
#include "DocxFactory/DocxMerger/DocxMergerItemFile.h"
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerTextField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteFieldGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteTextField.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/UnzipFile.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/str/StrFunc.h"

#include "boost/scoped_array.hpp"

using namespace DocxFactory;
using namespace std;



DocxMergerTextField::DocxMergerTextField() : DocxMergerField( DocxMergerField::TYPE_TEXT )
{

} // c'tor

DocxMergerTextField::~DocxMergerTextField()
{

} // d'tor



void DocxMergerTextField::save( DocxMergerPasteFieldGroup* p_pasteFieldGroup )
{
	if ( !p_pasteFieldGroup )
		return;

	ZipFile*	l_zipFile		= m_itemFile ->getZipFile();
	string		l_wordMlPrefix	= m_itemFile ->getWordMlPrefix();

	map<DocxMergerField*, DocxMergerPasteField*>::const_iterator	l_pasteFieldIterator;
	const map<DocxMergerField*, DocxMergerPasteField*>*				l_pasteFields = p_pasteFieldGroup ->getPasteFieldsByField();

	DocxMergerPasteTextField*	l_pasteTextField;
	string						l_str;

	l_pasteFieldIterator = l_pasteFields ->find( this );
	if ( l_pasteFieldIterator != l_pasteFields ->end() )
	{
		l_pasteTextField = ( DocxMergerPasteTextField* ) l_pasteFieldIterator ->second;

		l_str = encodeT( *( l_pasteTextField ->getValue() ), l_wordMlPrefix );
		( *l_zipFile ) << l_str;
	}
} // save



void DocxMergerTextField::setClipboardValue( const string& p_value )
{
	createPasteField<const string&, DocxMergerPasteTextField>( p_value );
} // setClipboardValue( string )

void DocxMergerTextField::setClipboardValue( double p_value )
{
	string l_value = LocaleFunc::numToStr( p_value, 0, 3, true );

	createPasteField<const string&, DocxMergerPasteTextField>( l_value );
} // setClipboardValue( double )



void DocxMergerTextField::deserialize( UnzipFile* p_unzipFile )
{
	DocxMergerField::deserialize( p_unzipFile );
} // deserialize



// ** encodeT does not contain any protection against invalid utf8 chars

string DocxMergerTextField::encodeT(
	const string& p_srcStr,
	const string& p_wordMlPrefix )
{
	size_t			l_srcLen	= p_srcStr.length();
	size_t			l_srcPos	= 0;

	size_t			l_dstPos	= 0;
	bool			l_newline	= false;
	bool			l_tab		= false;
	unsigned char	l_ch;

	boost::scoped_array<char>	l_dstStr( new char[ l_srcLen * 6 + 1 ] );

	while ( l_srcPos < l_srcLen )
	{
		l_ch = ( unsigned char ) p_srcStr[ l_srcPos ];

		if ( l_ch == '\r' )
		{
			if ( l_srcPos + 1 < l_srcLen && p_srcStr[ l_srcPos + 1 ] == '\n' )
				 l_srcPos++;

			l_dstStr[ l_dstPos ] = '\n';
			l_dstPos++;

			l_newline = true;
		}

		else if ( l_ch == '\n' )
		{
			l_dstStr[ l_dstPos ] = '\n';
			l_dstPos++;

			l_newline = true;
		}

		else if ( l_ch == '\f' )
		{
			l_dstStr[ l_dstPos ] = '\n';
			l_dstPos++;

			l_newline = true;
		}

		else if ( l_ch == '\t' )
		{
			l_dstStr[ l_dstPos ] = '\t';
			l_dstPos++;

			l_tab = true;
		}

		else if ( l_ch == '&' ) { strncpy( l_dstStr.get() + l_dstPos, "&amp;", 5 );		l_dstPos += 5; }
		else if ( l_ch == '"' ) { strncpy( l_dstStr.get() + l_dstPos, "&quot;", 6 );	l_dstPos += 6; }
		else if ( l_ch == '~' ) { strncpy( l_dstStr.get() + l_dstPos, "&apos;", 6 );	l_dstPos += 6; }
		else if ( l_ch == '<' )	{ strncpy( l_dstStr.get() + l_dstPos, "&lt;", 4 );		l_dstPos += 4; }
		else if ( l_ch == '>' )	{ strncpy( l_dstStr.get() + l_dstPos, "&gt;", 4 );		l_dstPos += 4; }

		else if ( l_ch <= 31 || l_ch == 127 )
		{
			l_dstStr[ l_dstPos ] = '?';
			l_dstPos++;
		}

		else
		{
			l_dstStr[ l_dstPos ] = p_srcStr[ l_srcPos ];
			l_dstPos++;
		}

		l_srcPos++;
	}

	l_dstStr[ l_dstPos ] = '\0';



	if ( l_newline || l_tab )
		return StrFunc::replace( StrFunc::replace( l_dstStr.get(),
		
			  "\n",
			  "</" + p_wordMlPrefix + ":t>"
			+ "<"  + p_wordMlPrefix + ":br/>"
			+ "<"  + p_wordMlPrefix + ":t xml:space= \"preserve\">" ),
			
			  "\t", 
			  "</" + p_wordMlPrefix + ":t>"
			+ "<"  + p_wordMlPrefix + ":tab/>"
			+ "<"  + p_wordMlPrefix + ":t xml:space= \"preserve\">" );

	else
		return l_dstStr.get();
} // encodeT
