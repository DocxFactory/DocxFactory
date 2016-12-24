
#include "DocxFactory/DocxMerger/DocxMergerFile.h"
#include "DocxFactory/DocxMerger/DocxMergerItemFile.h"
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerAltChunkField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteFieldGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteAltChunkField.h"

#include "DocxFactory/opc/OpcPart.h"
#include "DocxFactory/opc/OpcRelationships.h"
#include "DocxFactory/opc/OpcRelationship.h"
#include "DocxFactory/opc/OpcContentTypes.h"
#include "DocxFactory/opc/OpcFunc.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/UnzipFile.h"
#include "DocxFactory/os/OsFunc.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

using namespace DocxFactory;
using namespace std;



DocxMergerAltChunkField::DocxMergerAltChunkField() : DocxMergerField( DocxMergerField::TYPE_ALT_CHUNK )
{

} // c'tor

DocxMergerAltChunkField::~DocxMergerAltChunkField()
{

} // d'tor



void DocxMergerAltChunkField::save( DocxMergerPasteFieldGroup* p_pasteFieldGroup )
{
	if ( !p_pasteFieldGroup )
		return;

	ZipFile* l_zipFile = m_itemFile ->getZipFile();

	map<DocxMergerField*, DocxMergerPasteField*>::const_iterator	l_pasteFieldIterator;
	const map<DocxMergerField*, DocxMergerPasteField*>*				l_pasteFields = p_pasteFieldGroup ->getPasteFieldsByField();

	DocxMergerPasteAltChunkField*	l_pasteAltChunkField;
	string							l_fileRId;

	l_pasteFieldIterator = l_pasteFields ->find( this );
	if ( l_pasteFieldIterator != l_pasteFields ->end() )
	{
		l_pasteAltChunkField = ( DocxMergerPasteAltChunkField* ) l_pasteFieldIterator ->second;

		insertAltChunk(
			l_pasteAltChunkField ->getValue(),
			l_pasteAltChunkField ->getType(),
			l_fileRId );

		( *l_zipFile ) << m_altChunkString1;
		( *l_zipFile ) << l_fileRId;
		( *l_zipFile ) << m_altChunkString2;
	}
} // save

void DocxMergerAltChunkField::insertAltChunk( const string* p_value, AltChunkType p_type, string& p_fileRId )
{
	DocxMergerFile*		l_file = m_itemFile ->getFile();
	OpcPart*			l_part;
	OpcRelationship*	l_relationship;

	string				l_path;
	string				l_ext;
	string				l_contentType;

	switch ( p_type )
	{
	case TYPE_MHTML:
		l_ext			= ".mht";
		l_contentType	= "message/rfc822";
		break;

	case TYPE_HTML:
		l_ext			= ".htm";
		l_contentType	= "text/html";
		break;

	case TYPE_RTF:
		l_ext			= ".rtf";
		l_contentType	= "application/rtf";
		break;
	}

	l_path = l_file ->getAltChunkDir() + "altChunk-dfw-" + StrFunc::intToStr( l_file ->nextAltChunkSeq() ) + l_ext;
	l_part = l_file ->insertPart( l_path );
	l_file ->getContentTypes() ->insertDefault( l_ext, l_contentType );

	l_part ->setBuf( p_value ->c_str(), p_value ->length() );
	l_part ->setChangeStatus( OpcPart::PART_CHANGED_IN_BUFFER );

	l_relationship	= m_itemFile ->getPart() ->getRelationships() ->insertRelationship( "http://schemas.openxmlformats.org/officeDocument/2006/relationships/aFChunk", l_part );
	p_fileRId		= l_relationship ->getId();
} // insertAltChunk



void DocxMergerAltChunkField::setClipboardValue( const string& p_value )
{
	string l_value;
	string l_multiPart;

	switch ( m_altChunkType )
	{
	case DocxMergerAltChunkField::TYPE_MHTML:

		createPasteField( l_value, TYPE_MHTML );

		break;

	case DocxMergerAltChunkField::TYPE_HTML:

		if ( !StrFunc::begins( p_value, "<!" )
		  && !StrFunc::begins( p_value, "<?" )
		  && !StrFunc::begins( StrFunc::lc( p_value.substr( 0, 6 ) ), "<html>" ) )
			l_value = "<html><body>" + p_value + "</body></html>";

		else
			l_value = p_value;

		getMultiPart( l_value, l_multiPart );

		if ( !l_multiPart.empty() )

			createPasteField(
				  "MIME-Version: 1.0\r\n"
				  "Content-Type: multipart/related; type=\"text/html\"; boundary=\"----=_dfw-separator\"\r\n"
				  "\r\n"
				  "------=_dfw-separator\r\n"
				  "Content-Type: text/html;charset=\"utf-8\"\r\n"
				  "Content-Transfer-Encoding: binary\r\n"
				  "\r\n"
				+ l_value + "\r\n"
				+ "\r\n"
				+ "------=_dfw-separator\r\n"
				+ l_multiPart
				+ "------=_dfw-separator--\r\n",
				TYPE_MHTML );

		else
			createPasteField( l_value, TYPE_HTML );

		break;

	case DocxMergerAltChunkField::TYPE_RTF:

		if ( !StrFunc::begins( p_value, "{" ) )
			createPasteField( "{\\rtf1\\ansi\\deff0" + p_value + "}", TYPE_RTF );

		else
			createPasteField( p_value, TYPE_RTF );

		break;
	}
} // setClipboardValue( string )

void DocxMergerAltChunkField::setClipboardValue( double p_value )
{
	string l_value = LocaleFunc::numToStr( p_value, 0, 3, true );

	setClipboardValue( l_value );
} // setClipboardValue( double )

void DocxMergerAltChunkField::createPasteField( const string& p_value, AltChunkType p_type )
{
	DocxMergerPasteAltChunkField*	l_pasteField;
	bool							l_ok;

	l_pasteField	= new DocxMergerPasteAltChunkField( this, p_value, p_type );
	l_ok			= m_itemFile ->getPasteFieldGroup() ->insertPasteField( this, l_pasteField );

	if ( !l_ok )
		delete l_pasteField;
} // createPasteField



void DocxMergerAltChunkField::getMultiPart( const string& p_str, string& p_multiPart )
{
	byte*	l_buf;
	size_t	l_bufSize;

	string	l_file;
	string	l_ext;
	string	l_mime;

	string	l_name;
	string	l_value;

	size_t	l_len = p_str.length();
	size_t	l_pos = 0;

	p_multiPart = "";

	while ( l_pos < l_len )
	{
		switch ( p_str[ l_pos ] )
		{
		case '"':

			l_pos++;

			while ( l_pos < l_len && p_str[ l_pos ] != '"' )
			{
				l_pos++;
			}

			break;

		case '<':

			if ( l_len - l_pos >= 4
			  && p_str[ l_pos + 1 ] == '!'
			  && p_str[ l_pos + 2 ] == '-'
			  && p_str[ l_pos + 3 ] == '-' )
			{
				l_pos += 4;

				while ( l_pos < l_len )
				{
					if ( l_len - l_pos < 3 )
					{
						l_pos = l_len;
						break;
					}

					else
					if ( p_str[ l_pos ]		== '-'
					  && p_str[ l_pos + 1 ] == '-'
					  && p_str[ l_pos + 2 ] == '>' )
					{
						l_pos += 2;
						break;
					}

					l_pos++;
				}
			}

			else
			if ( l_len - l_pos >= 9
			  && p_str[ l_pos + 1 ] == '!'
			  && p_str[ l_pos + 2 ] == '['
			  && p_str[ l_pos + 3 ] == 'C'
			  && p_str[ l_pos + 4 ] == 'D'
			  && p_str[ l_pos + 5 ] == 'A'
			  && p_str[ l_pos + 6 ] == 'T'
			  && p_str[ l_pos + 7 ] == 'A'
			  && p_str[ l_pos + 8 ] == '[' )
			{
				l_pos += 9;

				while ( l_pos < l_len )
				{
					if ( l_len - l_pos < 3 )
					{
						l_pos = l_len;
						break;
					}

					else
					if ( p_str[ l_pos ]		== ']'
					  && p_str[ l_pos + 1 ] == ']'
					  && p_str[ l_pos + 2 ] == '>' )
					{
						l_pos += 2;
						break;
					}

					l_pos++;
				}
			}

			else
			if ( l_len - l_pos >= 5
			  && ( p_str[ l_pos + 1 ] == 'i' || p_str[ l_pos + 1 ] == 'I' )
			  && ( p_str[ l_pos + 2 ] == 'm' || p_str[ l_pos + 2 ] == 'M' )
			  && ( p_str[ l_pos + 3 ] == 'g' || p_str[ l_pos + 3 ] == 'G' )
			  && ( p_str[ l_pos + 4 ] == ' ' || p_str[ l_pos + 4 ] == '\t' || p_str[ l_pos + 4 ] == '\n' || p_str[ l_pos + 4 ] == '\r' )  )
			{
				l_pos += 5;

				while ( getAttr( p_str, l_len, l_pos, l_name, l_value ) )
				{
					if ( StrFunc::lc( l_name ) == "src" && !l_value.empty() && l_value.length() <= 100 )
					{
						if ( l_value.find( "://" ) != string::npos )
							break;

						l_file	= OsFunc::normalizePath( l_value );
						l_file	= OsFunc::getFullPath( l_file );

						l_ext	= OsFunc::getSubPath( l_file, OsFunc::SUBPATH_EXT, OsFunc::SUBPATH_EXT );
						l_ext	= StrFunc::lc( l_ext );

							 if ( l_ext == ".jpg" || l_ext == ".jpeg" )	l_mime = "image/jpeg";
						else if ( l_ext == ".png" )						l_mime = "image/png";
						else if ( l_ext == ".tiff" || l_ext == ".tif" )	l_mime = "image/tiff";
						else if ( l_ext == ".gif" )						l_mime = "image/gif";
						else if ( l_ext == ".bmp" )						l_mime = "image/bmp";
						else if ( l_ext == ".ico" )						l_mime = "image/x-icon";
						else											l_mime = "application/octet-stream";

						try
						{
							l_buf = OsFunc::readFile( l_file, l_bufSize );

							p_multiPart = p_multiPart + ( !p_multiPart.empty() ? "------=_dfw-separator\r\n" : "" ) +
								  "Content-Type: " + l_mime + "\r\n"
								  "Content-Transfer-Encoding: base64\r\n"
								  "Content-Location: " + l_value + "\r\n"
								  "\r\n"
								+ StrFunc::encodeBase64( ( char* ) l_buf, l_bufSize )
								+ "\r\n"
								  "\r\n";

							delete[] l_buf;
						}

						catch ( ... )
						{
						}

						break;
					}
				}
			}

			break;
		}

		l_pos++;
	}
} // getMultiPart

bool DocxMergerAltChunkField::getAttr( const string& p_str, const size_t& p_len, size_t& p_pos, string& p_name, string& p_value )
{
	p_name	= "";
	p_value	= "";

	if ( !skipSpace( p_str, p_len, p_pos ) )
		return false;

	if ( !getWord( p_str, p_len, p_pos, p_name ) )
		return false;

	if ( !skipSpace( p_str, p_len, p_pos ) )
		return false;

	if ( p_str[ p_pos ] != '=' )
		return true;

	p_pos++;

	if ( !skipSpace( p_str, p_len, p_pos ) )
		return false;

	if ( !getWord( p_str, p_len, p_pos, p_value ) )
		return false;

	return true;
} // getAttr

bool DocxMergerAltChunkField::getWord( const string& p_str, const size_t& p_len, size_t& p_pos, string& p_word )
{
	if ( p_str[ p_pos ] == '"' )
	{
		p_pos++;

		while ( true )
		{
			if ( p_pos >= p_len )
				return false;

			else
			if ( p_str[ p_pos ] == '"' )
			{
				p_pos++;
				break;
			}

			p_word	+= p_str[ p_pos ];
			p_pos	+= 1;
		}
	}

	else
	{
		while ( true )
		{
			if ( p_pos >= p_len )
				return false;

			else
			if ( p_str[ p_pos ] == '>' )
			{
				p_pos++;
				return false;
			}

			else
			if ( p_len - p_pos >= 2 && p_str[ p_pos ] == '/' && p_str[ p_pos + 1 ] == '>' )
			{
				p_pos += 2;
				return false;
			}

			else
			if ( !( p_str[ p_pos ] >= 'a' && p_str[ p_pos ] <= 'z'
				 || p_str[ p_pos ] >= 'A' && p_str[ p_pos ] <= 'Z'
				 || p_str[ p_pos ] >= '0' && p_str[ p_pos ] <= '9'
				 || p_str[ p_pos ] == '_' ) )
				break;

			p_word	+= p_str[ p_pos ];
			p_pos	+= 1;
		}
	}

	return true;
} // getWord

bool DocxMergerAltChunkField::skipSpace( const string& p_str, const size_t& p_len, size_t& p_pos )
{
	while ( true )
	{
		if ( p_pos >= p_len )
			return false;

		else
		if ( p_str[ p_pos ] == '>' )
		{
			p_pos++;
			return false;
		}

		else
		if ( p_len - p_pos >= 2 && p_str[ p_pos ] == '/'  && p_str[ p_pos + 1 ] == '>' )
		{
			p_pos += 2;
			return false;
		}

		else
		if ( p_str[ p_pos ] != ' ' && p_str[ p_pos ] != '\t' && p_str[ p_pos ] != '\n' && p_str[ p_pos ] != '\r' )
			break;

		p_pos++;
	}

	return true;
} // skipSpace



void DocxMergerAltChunkField::deserialize( UnzipFile* p_unzipFile )
{
	DocxMergerField::deserialize( p_unzipFile );

	m_altChunkType		= ( DocxMergerAltChunkField::AltChunkType ) p_unzipFile ->readNum<int16>();
	m_altChunkString1	= p_unzipFile ->readStr();
	m_altChunkString2	= p_unzipFile ->readStr();
} // deserialize



DocxMergerAltChunkField::AltChunkType DocxMergerAltChunkField::getAltChunkType() const
{
	return m_altChunkType;
} // getAltChunkType
