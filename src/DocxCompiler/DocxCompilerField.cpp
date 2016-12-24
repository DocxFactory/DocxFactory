
#include "DocxFactory/DocxCompiler/DocxCompilerItem.h"
#include "DocxFactory/DocxCompiler/DocxCompilerField.h"

#include "DocxFactory/parser/ParserFunc.h"

#include "DocxFactory/zip/ZipFile.h"

#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include <vector>
#include <utility>
#include <climits>

using namespace DocxFactory;
using namespace std;



bool										DocxCompilerField::m_initialized	= false;
map<string, DocxCompilerField::FieldParam>*	DocxCompilerField::m_paramByStr		= NULL;
map<DocxCompilerField::FieldParam, string>*	DocxCompilerField::m_strByParam		= NULL;



void DocxCompilerField::getTypeAndFormat( 
	string&						p_format,
	FieldType&					p_fieldType,
	map<FieldParam, string>*	p_valueByParam )
{
	if ( !m_initialized )
		initialize();

	map<FieldParam, string>::iterator l_valueByParamIterator;

	unsigned short	l_textCnt;
	unsigned short	l_numberCnt;
	unsigned short	l_datetimeCnt;
	unsigned short	l_altChunkCnt;
	unsigned short	l_barcodeCnt;

	vector<string>	l_wordList;
	FieldParam		l_param;
	string			l_value;
	size_t			l_pos;
	size_t			l_len;

	FieldParam		l_case;
	bool			l_trim;
	FieldParam		l_altChunk;

	FieldParam		l_symbology;
	int				l_checkDigit;
	int				l_cols;
	int				l_channels;
	int				l_size;
	int				l_mode;
	int				l_security;
	bool			l_square;
	bool			l_gs1;
	double			l_scale;
	int				l_height;
	bool			l_showValue;
	int				l_angle;
	int				l_fontSize;
	string			l_font;
	string			l_fgcolor;
	string			l_bgcolor;
	int				l_whiteSpaceWidth;
	int				l_borderWidth;
	FieldParam		l_borderOption;

	string			l_str;
	char			l_ch;
	size_t			i;

	p_fieldType	= TYPE_TEXT; 
	p_valueByParam ->clear();

	if ( p_format.empty() )
		return;

	if ( StrFunc::lc( p_format ) == "general" )
		return;



	l_textCnt		= 0;
	l_numberCnt		= 0;
	l_datetimeCnt	= 0;
	l_altChunkCnt	= 0;
	l_barcodeCnt	= 0;

	try
	{	
		l_wordList  = ParserFunc::parseWordList( p_format, ParserFunc::SPACE_KEEP, false );
		p_format	= "";

		l_pos       = 0;
		l_len       = l_wordList.size();  

		_word:
		{
			while ( l_pos < l_len )
			{
				_param:
				{
					i = l_pos;

					if ( i != 0 && StrFunc::trim( l_wordList[i] ).empty() )
						i++;

					if ( i >= l_len )
						goto _paramEnd;

					if ( l_wordList[i] == "color" ) // backward compatibility
						 l_wordList[i] = "fgcolor";

					l_param	= strToParam( l_wordList[i] );

					i++;



					switch ( l_param )
					{
						case PARAM_TEXT:

							if ( l_textCnt != USHRT_MAX )
								 l_textCnt = USHRT_MAX;

							break;

						case PARAM_NUMBER:

							if ( l_numberCnt != USHRT_MAX )
								 l_numberCnt = USHRT_MAX;

							break;

						case PARAM_DATE_TIME:

							if ( l_datetimeCnt != USHRT_MAX )
								 l_datetimeCnt = USHRT_MAX;

							break;

						case PARAM_UPPER_CASE:
						case PARAM_LOWER_CASE:
						case PARAM_TITLE_CASE:
						case PARAM_FIRST_CAPITAL: 
						case PARAM_TRIM:

							if ( l_textCnt != USHRT_MAX )
								 l_textCnt++;

							break;

						case PARAM_HTML:
						case PARAM_RTF:

							if ( l_altChunkCnt != USHRT_MAX )
								 l_altChunkCnt = USHRT_MAX;

							break;

						case PARAM_CODE11:
						case PARAM_CODE25:
						case PARAM_CODE25_IATA:
						case PARAM_CODE25_INTER:
						case PARAM_CODE25_LOGIC:
						case PARAM_CODE25_IND:
						case PARAM_ITF14:
						case PARAM_CODE39:
						case PARAM_CODE39_EXT:
						case PARAM_CODE93:
						case PARAM_PZN:
						case PARAM_LOGMARS:
						case PARAM_CODE32:
						case PARAM_CODE128:
						case PARAM_CODE128B:
						case PARAM_EAN128:
						case PARAM_EAN14:
						case PARAM_NVE18:
						case PARAM_UPCA:
						case PARAM_UPCE:
						case PARAM_EAN:
						case PARAM_ISBN:
						case PARAM_CODABAR:
						case PARAM_PHARMA:
						case PARAM_PHARMA_2TRACK:
						case PARAM_PLESSEY:
						case PARAM_MSI_PLESSEY:
						case PARAM_TELEPEN:
						case PARAM_TELEPEN_NUM:
						case PARAM_RSS14:
						case PARAM_RSS14_STACK:
						case PARAM_RSS14_STACKOMNI:
						case PARAM_RSS_LTD:
						case PARAM_RSS_EXP:
						case PARAM_RSS_EXPSTACK:
						case PARAM_CHANNEL:
						case PARAM_FIM:
						case PARAM_FLAT:
						case PARAM_DAFT:
						case PARAM_POSTNET:
						case PARAM_PLANET:
						case PARAM_ONECODE:
						case PARAM_RM4SCC:
						case PARAM_DPLEIT:
						case PARAM_DPIDENT:
						case PARAM_AUSPOST:
						case PARAM_AUSREPLY:
						case PARAM_AUSROUTE:
						case PARAM_AUSREDIRECT:
						case PARAM_KIX:
						case PARAM_JAPANPOST:
						case PARAM_KOREAPOST:
						case PARAM_CODE16K:
						case PARAM_CODE49:
						case PARAM_PDF417:
						case PARAM_PDF417_TRUNC:
						case PARAM_MICROPDF:
						case PARAM_DATAMATRIX:
						case PARAM_QRCODE:
						case PARAM_MICROQR:
						case PARAM_MAXICODE:
						case PARAM_AZTEC:
						case PARAM_AZRUNE:
						case PARAM_CODEONE:
						case PARAM_GRIDMATRIX:
						case PARAM_HIBC_CODE39:
						case PARAM_HIBC_CODE128:
						case PARAM_HIBC_PDF417:
						case PARAM_HIBC_MICROPDF:
						case PARAM_HIBC_DATAMATRIX:
						case PARAM_HIBC_QRCODE:
						case PARAM_HIBC_AZTEC:
						case PARAM_CS_EAN:
						case PARAM_CS_EAN128:
						case PARAM_CS_UPCA:
						case PARAM_CS_UPCE:
						case PARAM_CS_RSS14:
						case PARAM_CS_RSS14_STACK:
						case PARAM_CS_RSS14_STACKOMNI:
						case PARAM_CS_RSS_LTD:
						case PARAM_CS_RSS_EXP:
						case PARAM_CS_RSS_EXPSTACK:

							if ( l_barcodeCnt != USHRT_MAX )
								 l_barcodeCnt = USHRT_MAX;

							break;

						case PARAM_CHECK_DIGIT:
						case PARAM_COLS:
						case PARAM_CHANNELS:
						case PARAM_SIZE:
						case PARAM_MODE:
						case PARAM_SECURITY:
						case PARAM_SQUARE:
						case PARAM_GS1:
						case PARAM_SCALE:
						case PARAM_HEIGHT:
						case PARAM_SHOW_VALUE:
						case PARAM_ANGLE:
						case PARAM_FONT_SIZE:
						case PARAM_FONT:
						case PARAM_FGCOLOR:
						case PARAM_BGCOLOR:
						case PARAM_WHITESPACE_WIDTH:
						case PARAM_BORDER_WIDTH:
						case PARAM_BIND:
						case PARAM_BOX:

							if ( l_barcodeCnt != USHRT_MAX )
								 l_barcodeCnt++;

							break;

						case PARAM_UNDEFINED:
							goto _paramEnd;
					}



					_value:
					{
						l_value  = "";
						l_pos    = i;

						if ( i >= l_len )
							goto _valueEnd;

						if ( StrFunc::trim( l_wordList[i] ).empty() )
							i++;

						if ( i >= l_len )
							goto _valueEnd;

						if ( l_wordList[i] != ":" )
							goto _valueEnd;

						i++;

						if ( i >= l_len )
							goto _valueEnd;

						if ( StrFunc::trim( l_wordList[i] ).empty() )
							i++;

						if ( i >= l_len )
							goto _valueEnd;

						l_str = l_wordList[i];

						if ( l_str.length() > 0 && ( l_str[0] == '"' || l_str[0] == '\'' ) )
							 l_str = ParserFunc::unquote( l_str );

						i++;

						l_value  = l_str;
						l_pos    = i;
					}

					_valueEnd:

					p_valueByParam ->insert( make_pair( l_param, l_value ) );

					goto _word;
				}

				_paramEnd:

				while ( l_pos < i )
				{
					p_format += l_wordList[ l_pos ];
					l_pos++;
				}
			}	
		}

		p_format = StrFunc::trim( p_format );
	}

	catch ( ... )
	{

	}



		 if ( l_textCnt		== USHRT_MAX ) p_fieldType = TYPE_TEXT;
	else if ( l_numberCnt	== USHRT_MAX ) p_fieldType = TYPE_NUMBER;
	else if ( l_datetimeCnt	== USHRT_MAX ) p_fieldType = TYPE_DATE_TIME;
	else if ( l_altChunkCnt	== USHRT_MAX ) p_fieldType = TYPE_ALT_CHUNK;
	else if ( l_barcodeCnt	== USHRT_MAX ) p_fieldType = TYPE_BARCODE;

	else
	{
		l_pos    = 0;
		l_len    = p_format.length();

		while ( l_pos < l_len )
		{
			l_ch = p_format[ l_pos ];

			if ( l_ch == '\\' )
				l_pos += 2;

			else
			if ( l_ch == 'x' || l_ch == 'X' )
			{
				l_textCnt++;
				l_pos++;

				if ( l_pos + 2 < l_len && p_format[ l_pos ] == '(' )
				{
					i = p_format.find( ")", l_pos + 2 );

					if ( i != string::npos )
					{
						l_str = StrFunc::trim( p_format.substr( l_pos + 1, i - l_pos - 1 ) );

						if ( StrFunc::isNum( l_str ) )
						{
							l_textCnt	+= abs( StrFunc::strToInt<int>( l_str ) );
							l_pos		 = i + 1;
						}
					}
				}
			}

			else
			if ( l_ch == '[' && l_pos + 1 < l_len )
			{
				i = p_format.find( "]", l_pos + 1 );
				if ( i != string::npos )
				{
					l_str = StrFunc::normalizeColor( p_format.substr( l_pos + 1, i - l_pos - 1 ) );
					if ( !l_str.empty() )

						l_numberCnt++;

					l_pos++;
				}

				else
					l_pos++;
			}

			else if ( l_ch == '0' || l_ch == '#' )
			{
				l_numberCnt++;
				l_pos++;
			}

			else
			if ( l_ch == '9'
			  && ( p_format.substr( l_pos, 7 ) == "9/99/99" 
				|| p_format.substr( l_pos, 7 ) == "9-99-99" ) )
			{
				l_datetimeCnt	+= 8;
				l_pos			+= 8;

				if ( l_pos < l_len && p_format.substr( l_pos, 2 ) == "99" )
				{
					l_datetimeCnt	+= 2;
					l_pos			+= 2;
				}
			}

			else
			if ( strchr( "MdyYDwWqQhHmsSfFzZ", l_ch ) )
			{
				l_datetimeCnt	+= 1;
				l_pos			+= 1; 
			}

			else
			if ( l_pos + 1 < l_len && ( l_ch == 'a' || l_ch == 'A' ) )
			{
				if ( StrFunc::lc( p_format.substr( l_pos + 1, 4 ) ) == "m/pm" )
				{
					l_datetimeCnt	+= 5;
					l_pos			+= 5;
				}

				else
				if ( StrFunc::lc( p_format.substr( l_pos + 1, 2 ) ) == "/p" )
				{
					l_datetimeCnt	+= 3;
					l_pos			+= 3;
				}

				else
					l_pos++;
			}

			else
				l_pos++;
		}

		// text is the default type
		// if all counts are 0 the type will be text.

		if ( l_textCnt >= l_numberCnt
		  && l_textCnt >= l_datetimeCnt
		  && l_textCnt >= l_altChunkCnt
		  && l_textCnt >= l_barcodeCnt )
			
			p_fieldType = TYPE_TEXT;

		else
		if ( l_numberCnt >= l_datetimeCnt
		  && l_numberCnt >= l_altChunkCnt
		  && l_numberCnt >= l_barcodeCnt )
			
			p_fieldType = TYPE_NUMBER;

		else
		if ( l_datetimeCnt >= l_altChunkCnt
		  && l_datetimeCnt >= l_barcodeCnt )

			p_fieldType = TYPE_DATE_TIME;
		
		else
		if ( l_altChunkCnt >= l_barcodeCnt )

			p_fieldType = TYPE_ALT_CHUNK;
		
		else
			p_fieldType = TYPE_BARCODE;
	}



	switch ( p_fieldType )
	{
	case TYPE_TEXT:

		l_case	= PARAM_UNDEFINED;
		l_trim	= false;

		FOR_EACH( l_valueByParamIterator, p_valueByParam )
		{
			l_param = l_valueByParamIterator ->first;

			switch ( l_param )
			{
			case PARAM_UPPER_CASE:
			case PARAM_LOWER_CASE:
			case PARAM_TITLE_CASE:
			case PARAM_FIRST_CAPITAL:

				if ( l_case == PARAM_UNDEFINED )
					 l_case = l_param;

				break;

			case PARAM_TRIM:

				if ( !l_trim )
					  l_trim = true;

				break;
			}
		}

		p_valueByParam ->clear();

		if ( l_case != PARAM_UNDEFINED )
			p_valueByParam ->insert( make_pair( l_case, "" ) );

		if ( l_trim )
			p_valueByParam ->insert( make_pair( PARAM_TRIM, "" ) );

		break;

	case TYPE_NUMBER:

		if ( p_format.empty() )
			 p_format = "#,##0.###"; 

		p_valueByParam ->clear();
		break;

	case TYPE_DATE_TIME:

		if ( p_format.empty() )
			 p_format = "99/99/99";

		p_valueByParam ->clear();
		break;

	case TYPE_ALT_CHUNK:

		l_altChunk = PARAM_UNDEFINED;

		if ( p_valueByParam ->find( PARAM_RTF ) != p_valueByParam ->end() )
			l_altChunk = PARAM_RTF;

		else
			l_altChunk = PARAM_HTML;

		p_format = "";
		p_valueByParam ->clear();

		if ( l_altChunk != PARAM_UNDEFINED )
			p_valueByParam ->insert( make_pair( l_altChunk, "" ) );

		break;

	case TYPE_BARCODE:

		l_symbology			= PARAM_UNDEFINED;
		l_checkDigit		= -1;
		l_cols				= -1;
		l_channels			= -1;
		l_size				= -1;
		l_mode				= -1;
		l_security			= -1;
		l_square			= false;
		l_gs1				= false;

		l_scale				= 0.0f;
		l_height			= 0;
		l_showValue			= false;
		l_angle				= 0;
		l_fontSize			= 0;
		l_font				= "";
		l_fgcolor			= "";
		l_bgcolor			= "";
		l_whiteSpaceWidth	= 0;
		l_borderWidth		= 0;
		l_borderOption		= PARAM_UNDEFINED;

		FOR_EACH( l_valueByParamIterator, p_valueByParam )
		{
			l_param	= l_valueByParamIterator ->first;
			l_value	= l_valueByParamIterator ->second;

			switch ( l_param )
			{
			case PARAM_CODE11:
			case PARAM_CODE25:
			case PARAM_CODE25_IATA:
			case PARAM_CODE25_INTER:
			case PARAM_CODE25_LOGIC:
			case PARAM_CODE25_IND:
			case PARAM_ITF14:
			case PARAM_CODE39:
			case PARAM_CODE39_EXT:
			case PARAM_CODE93:
			case PARAM_PZN:
			case PARAM_LOGMARS:
			case PARAM_CODE32:
			case PARAM_CODE128:
			case PARAM_CODE128B:
			case PARAM_EAN128:
			case PARAM_EAN14:
			case PARAM_NVE18:
			case PARAM_UPCA:
			case PARAM_UPCE:
			case PARAM_EAN:
			case PARAM_ISBN:
			case PARAM_CODABAR:
			case PARAM_PHARMA:
			case PARAM_PHARMA_2TRACK:
			case PARAM_PLESSEY:
			case PARAM_MSI_PLESSEY:
			case PARAM_TELEPEN:
			case PARAM_TELEPEN_NUM:
			case PARAM_RSS14:
			case PARAM_RSS14_STACK:
			case PARAM_RSS14_STACKOMNI:
			case PARAM_RSS_LTD:
			case PARAM_RSS_EXP:
			case PARAM_RSS_EXPSTACK:
			case PARAM_CHANNEL:
			case PARAM_FIM:
			case PARAM_FLAT:
			case PARAM_DAFT:
			case PARAM_POSTNET:
			case PARAM_PLANET:
			case PARAM_ONECODE:
			case PARAM_RM4SCC:
			case PARAM_DPLEIT:
			case PARAM_DPIDENT:
			case PARAM_AUSPOST:
			case PARAM_AUSREPLY:
			case PARAM_AUSROUTE:
			case PARAM_AUSREDIRECT:
			case PARAM_KIX:
			case PARAM_JAPANPOST:
			case PARAM_KOREAPOST:
			case PARAM_CODE16K:
			case PARAM_CODE49:
			case PARAM_PDF417:
			case PARAM_PDF417_TRUNC:
			case PARAM_MICROPDF:
			case PARAM_DATAMATRIX:
			case PARAM_QRCODE:
			case PARAM_MICROQR:
			case PARAM_MAXICODE:
			case PARAM_AZTEC:
			case PARAM_AZRUNE:
			case PARAM_CODEONE:
			case PARAM_GRIDMATRIX:
			case PARAM_HIBC_CODE39:
			case PARAM_HIBC_CODE128:
			case PARAM_HIBC_PDF417:
			case PARAM_HIBC_MICROPDF:
			case PARAM_HIBC_DATAMATRIX:
			case PARAM_HIBC_QRCODE:
			case PARAM_HIBC_AZTEC:
			case PARAM_CS_EAN:
			case PARAM_CS_EAN128:
			case PARAM_CS_UPCA:
			case PARAM_CS_UPCE:
			case PARAM_CS_RSS14:
			case PARAM_CS_RSS14_STACK:
			case PARAM_CS_RSS14_STACKOMNI:
			case PARAM_CS_RSS_LTD:
			case PARAM_CS_RSS_EXP:
			case PARAM_CS_RSS_EXPSTACK:

				if ( l_symbology == PARAM_UNDEFINED )
					 l_symbology = l_param;

				break;

			case PARAM_CHECK_DIGIT:

				if ( l_checkDigit == -1 )
				{
					if ( l_value.empty() )
						 l_checkDigit = 1;

					else
					if ( StrFunc::isNum( l_value ) )
						 l_checkDigit = StrFunc::strToInt<int>( l_value );
				}

				break;

			case PARAM_COLS:

				if ( l_cols == -1 && StrFunc::isNum( l_value ) )
					 l_cols = StrFunc::strToInt<int>( l_value );

				break;

			case PARAM_CHANNELS:

				if ( l_channels == -1 && StrFunc::isNum( l_value ) )
					 l_channels = StrFunc::strToInt<int>( l_value );

				break;

			case PARAM_SIZE:

				if ( l_size == -1 && StrFunc::isNum( l_value ) )
					 l_size = StrFunc::strToInt<int>( l_value );

				break;

			case PARAM_MODE:

				if ( l_mode == -1 && StrFunc::isNum( l_value ) )
					 l_mode = StrFunc::strToInt<int>( l_value );

				break;

			case PARAM_SECURITY:

				if ( l_security == -1 && StrFunc::isNum( l_value ) )
					 l_security = StrFunc::strToInt<int>( l_value );

				break;

			case PARAM_SQUARE:

				if ( !l_square )
					  l_square = true;

				break;

			case PARAM_GS1:

				if ( !l_gs1 )
					  l_gs1 = true;

				break;

			case PARAM_SCALE:

				if ( l_scale == 0.0f && StrFunc::isNum( l_value ) )
					 l_scale = StrFunc::strToInt<double>( l_value );

				break;

			case PARAM_HEIGHT:

				if ( l_height == 0 && StrFunc::isNum( l_value ) )
					 l_height = StrFunc::strToInt<int>( l_value );

				break;

			case PARAM_SHOW_VALUE:

				if ( !l_showValue )
					  l_showValue = true;

				break;

			case PARAM_ANGLE:

				if ( l_angle == 0 && StrFunc::isNum( l_value ) )
					 l_angle = StrFunc::strToInt<int>( l_value );

				break;

			case PARAM_FONT_SIZE:

				if ( l_fontSize == 0 )
					 l_fontSize = StrFunc::strToInt<int>( l_value );

			case PARAM_FONT:

				if ( l_font.empty() )
					 l_font = l_value;

			case PARAM_FGCOLOR:

				if ( l_fgcolor.empty() )
					 l_fgcolor = l_value;

				break;

			case PARAM_BGCOLOR:

				if ( l_bgcolor.empty() )
					 l_bgcolor = l_value;

				break;

			case PARAM_WHITESPACE_WIDTH:

				if ( l_whiteSpaceWidth == 0 && StrFunc::isNum( l_value ) )
					 l_whiteSpaceWidth = StrFunc::strToInt<int>( l_value );

				break;

			case PARAM_BORDER_WIDTH:

				if ( l_borderWidth == 0 && StrFunc::isNum( l_value ) )
					 l_borderWidth = StrFunc::strToInt<int>( l_value );

				break;

			case PARAM_BIND:
			case PARAM_BOX:

				if ( l_borderOption == PARAM_UNDEFINED )
					 l_borderOption = l_param;

				break;
			}
		}

		p_format = "";
		p_valueByParam ->clear();

		if ( l_symbology == PARAM_UNDEFINED )
			 l_symbology = PARAM_CODE128;

		if ( l_checkDigit < 0 )
			 l_checkDigit = -1;

		if ( l_cols < 0 )
			 l_cols = -1;

		if ( l_channels < 0 )
			 l_channels = -1;

		if ( l_size < 0 )
			 l_size = -1;

		if ( l_mode < 0 )
			 l_mode = -1;

		if ( l_security < 0 )
			 l_security = -1;

		if ( l_scale < 0.0f )
			 l_scale = 0.0f;

		if ( l_height < 0 )
			 l_height = 0;

		if ( l_angle != 0 )
		{
			if ( l_angle < 0 )
				 l_angle = 360 - abs( l_angle ) % 360;

			else
			if ( l_angle >= 360 )
				 l_angle = l_angle % 360;
		}

		if ( l_fontSize != 0 && l_fontSize < 8 )
			 l_fontSize = 8;

		if ( l_fontSize > 20 )
			 l_fontSize = 15;

		if (!l_font.empty() )
			 l_font = StrFunc::trim( l_font );

		if (!l_fgcolor.empty() )
			 l_fgcolor = StrFunc::normalizeColor( l_fgcolor );

		if (!l_bgcolor.empty() )
			 l_bgcolor = StrFunc::normalizeColor( l_bgcolor );

		if ( l_whiteSpaceWidth < 0 )
			 l_whiteSpaceWidth = 0;

		if ( l_whiteSpaceWidth > 99 )
			 l_whiteSpaceWidth = 99;

		if ( l_borderWidth < 0 )
			 l_borderWidth = 0;

		if ( l_borderWidth > 16 )
			 l_borderWidth = 16;

		if ( l_symbology != PARAM_UNDEFINED )
			p_valueByParam ->insert( make_pair( l_symbology, "" ) );

		if ( l_checkDigit != -1 )
			p_valueByParam ->insert( make_pair( PARAM_CHECK_DIGIT, StrFunc::intToStr( l_checkDigit ) ) );

		if ( l_cols != -1 )
			p_valueByParam ->insert( make_pair( PARAM_COLS, StrFunc::intToStr( l_cols ) ) );

		if ( l_channels != -1 )
			p_valueByParam ->insert( make_pair( PARAM_CHANNELS, StrFunc::intToStr( l_channels ) ) );

		if ( l_size != -1 )
			p_valueByParam ->insert( make_pair( PARAM_SIZE, StrFunc::intToStr( l_size ) ) );

		if ( l_mode != -1 )
			p_valueByParam ->insert( make_pair( PARAM_MODE, StrFunc::intToStr( l_mode ) ) );

		if ( l_security != -1 )
			p_valueByParam ->insert( make_pair( PARAM_SECURITY, StrFunc::intToStr( l_security ) ) );

		if ( l_square )
			p_valueByParam ->insert( make_pair( PARAM_SQUARE, "" ) );

		if ( l_gs1 )
			p_valueByParam ->insert( make_pair( PARAM_GS1, "" ) );

		if ( l_scale != 0.0f )
			p_valueByParam ->insert( make_pair( PARAM_SCALE, StrFunc::intToStr( l_scale ) ) );

		if ( l_height != 0 )
			p_valueByParam ->insert( make_pair( PARAM_HEIGHT, StrFunc::intToStr( l_height ) ) );

		if ( l_showValue )						
			p_valueByParam ->insert( make_pair( PARAM_SHOW_VALUE, "" ) );

		if ( l_angle != 0 )
			p_valueByParam ->insert( make_pair( PARAM_ANGLE, StrFunc::intToStr( l_angle ) ) );

		if ( l_fontSize != 0 )
			p_valueByParam ->insert( make_pair( PARAM_FONT_SIZE, StrFunc::intToStr( l_fontSize ) ) );

		if ( !l_font.empty() )
			p_valueByParam ->insert( make_pair( PARAM_FONT, l_font ) );

		if ( !l_fgcolor.empty() )
			p_valueByParam ->insert( make_pair( PARAM_FGCOLOR, l_fgcolor ) );

		if ( !l_bgcolor.empty() )
			p_valueByParam ->insert( make_pair( PARAM_BGCOLOR, l_bgcolor ) );

		if ( l_whiteSpaceWidth != 0 )
			p_valueByParam ->insert( make_pair( PARAM_WHITESPACE_WIDTH, StrFunc::intToStr( l_whiteSpaceWidth ) ) );

		if ( l_borderWidth != 0 )
			p_valueByParam ->insert( make_pair( PARAM_BORDER_WIDTH, StrFunc::intToStr( l_borderWidth ) ) );

		if ( l_borderOption != PARAM_UNDEFINED )
			p_valueByParam ->insert( make_pair( l_borderOption, "" ) );

		break;
	}
} // getTypeAndFormat



DocxCompilerField::FieldParam DocxCompilerField::strToParam( const string& p_str )
{
	if ( !m_initialized )
		initialize();

	if ( p_str.empty() )
		return PARAM_UNDEFINED;

	map<string, FieldParam>::iterator l_paramByStrIterator = m_paramByStr ->find( StrFunc::lc( p_str ) );

	if ( l_paramByStrIterator != m_paramByStr ->end() )
		return l_paramByStrIterator ->second;

	else
		return PARAM_UNDEFINED;
} // strToParam

string DocxCompilerField::paramToStr( DocxCompilerField::FieldParam p_param )
{
	if ( !m_initialized )
		initialize();

	map<FieldParam, string>::iterator l_strByParamIterator = m_strByParam ->find( p_param );

	if ( l_strByParamIterator != m_strByParam ->end() )
		return l_strByParamIterator ->second;

	else
		return "";
} // paramToStr

void DocxCompilerField::initialize()
{
	map<FieldParam, string>::iterator l_strByParamIterator;

	m_initialized	= true;
	m_strByParam	= new map<FieldParam, string>();
	m_paramByStr	= new map<string, FieldParam>();

	m_strByParam ->insert( make_pair( PARAM_TEXT,						"Text" ) );
	m_strByParam ->insert( make_pair( PARAM_NUMBER,						"Number" ) );
	m_strByParam ->insert( make_pair( PARAM_DATE_TIME,					"DateTime" ) );
	m_strByParam ->insert( make_pair( PARAM_UPPER_CASE,					"UpperCase" ) );
	m_strByParam ->insert( make_pair( PARAM_LOWER_CASE,					"LowerCase" ) );
	m_strByParam ->insert( make_pair( PARAM_TITLE_CASE,					"TitleCase" ) );
	m_strByParam ->insert( make_pair( PARAM_FIRST_CAPITAL,				"FirstCapital" ) );
	m_strByParam ->insert( make_pair( PARAM_TRIM,						"Trim" ) );
	m_strByParam ->insert( make_pair( PARAM_HTML,						"HTML" ) );
	m_strByParam ->insert( make_pair( PARAM_RTF,						"RTF" ) );
	m_strByParam ->insert( make_pair( PARAM_USE_IMAGE_FILE_SIZE,		"UseImageFileSize" ) );
	m_strByParam ->insert( make_pair( PARAM_USE_PLACE_HOLDER_WIDTH,		"UsePlaceHolderWidth" ) );
	m_strByParam ->insert( make_pair( PARAM_USE_PLACE_HOLDER_HEIGHT,	"UsePlaceHolderHeight" ) );
	m_strByParam ->insert( make_pair( PARAM_USE_PLACE_HOLDER_SIZE,		"UsePlaceHolderSize" ) );
	m_strByParam ->insert( make_pair( PARAM_CODE11,						"Code11" ) );
	m_strByParam ->insert( make_pair( PARAM_CODE25,						"Code25" ) );
	m_strByParam ->insert( make_pair( PARAM_CODE25_IATA,				"Code25IATA" ) );
	m_strByParam ->insert( make_pair( PARAM_CODE25_INTER,				"Code25Inter" ) );
	m_strByParam ->insert( make_pair( PARAM_CODE25_LOGIC,				"Code25Logic" ) );
	m_strByParam ->insert( make_pair( PARAM_CODE25_IND,					"Code25Ind" ) );
	m_strByParam ->insert( make_pair( PARAM_ITF14,						"ITF14" ) );
	m_strByParam ->insert( make_pair( PARAM_CODE39,						"Code39" ) );
	m_strByParam ->insert( make_pair( PARAM_CODE39_EXT,					"Code39Ext" ) );
	m_strByParam ->insert( make_pair( PARAM_CODE93,						"Code93" ) );
	m_strByParam ->insert( make_pair( PARAM_PZN,						"PZN" ) );
	m_strByParam ->insert( make_pair( PARAM_LOGMARS,					"LOGMARS" ) );
	m_strByParam ->insert( make_pair( PARAM_CODE32,						"Code32" ) );
	m_strByParam ->insert( make_pair( PARAM_CODE128,					"Code128" ) );
	m_strByParam ->insert( make_pair( PARAM_CODE128B,					"Code128B" ) );
	m_strByParam ->insert( make_pair( PARAM_EAN128,						"EAN128" ) );
	m_strByParam ->insert( make_pair( PARAM_EAN14,						"EAN14" ) );
	m_strByParam ->insert( make_pair( PARAM_NVE18,						"NVE14" ) );
	m_strByParam ->insert( make_pair( PARAM_UPCA,						"UPCA" ) );
	m_strByParam ->insert( make_pair( PARAM_UPCE,						"UPCE" ) );
	m_strByParam ->insert( make_pair( PARAM_EAN,						"EAN" ) );
	m_strByParam ->insert( make_pair( PARAM_ISBN,						"ISBN" ) );
	m_strByParam ->insert( make_pair( PARAM_CODABAR,					"Codabar" ) );
	m_strByParam ->insert( make_pair( PARAM_PHARMA,						"Pharma" ) );
	m_strByParam ->insert( make_pair( PARAM_PHARMA_2TRACK,				"Pharma2Track" ) );
	m_strByParam ->insert( make_pair( PARAM_PLESSEY,					"Plessey" ) );
	m_strByParam ->insert( make_pair( PARAM_MSI_PLESSEY,				"MSIPlessey" ) );
	m_strByParam ->insert( make_pair( PARAM_TELEPEN,					"Telepen" ) );
	m_strByParam ->insert( make_pair( PARAM_TELEPEN_NUM,				"TelepenNum" ) );
	m_strByParam ->insert( make_pair( PARAM_RSS14,						"RSS14" ) );
	m_strByParam ->insert( make_pair( PARAM_RSS14_STACK,				"RSS14Stack" ) );
	m_strByParam ->insert( make_pair( PARAM_RSS14_STACKOMNI,			"RSS14StackOmni" ) );
	m_strByParam ->insert( make_pair( PARAM_RSS_LTD,					"RSSLtd" ) );
	m_strByParam ->insert( make_pair( PARAM_RSS_EXP,					"RSSExp" ) );
	m_strByParam ->insert( make_pair( PARAM_RSS_EXPSTACK,				"RSSExpStack" ) );
	m_strByParam ->insert( make_pair( PARAM_CHANNEL,					"Channel" ) );
	m_strByParam ->insert( make_pair( PARAM_FIM,						"FIM" ) );
	m_strByParam ->insert( make_pair( PARAM_FLAT,						"Flat" ) );
	m_strByParam ->insert( make_pair( PARAM_DAFT,						"DAFT" ) );
	m_strByParam ->insert( make_pair( PARAM_POSTNET,					"PostNet" ) );
	m_strByParam ->insert( make_pair( PARAM_PLANET,						"PLANET" ) );
	m_strByParam ->insert( make_pair( PARAM_ONECODE,					"OneCode" ) );
	m_strByParam ->insert( make_pair( PARAM_RM4SCC,						"RM4SCC" ) );
	m_strByParam ->insert( make_pair( PARAM_DPLEIT,						"DPLeit" ) );
	m_strByParam ->insert( make_pair( PARAM_DPIDENT,					"DPIdent" ) );
	m_strByParam ->insert( make_pair( PARAM_AUSPOST,					"AUSPost" ) );
	m_strByParam ->insert( make_pair( PARAM_AUSREPLY,					"AUSReply" ) );
	m_strByParam ->insert( make_pair( PARAM_AUSROUTE,					"AUSRoute" ) );
	m_strByParam ->insert( make_pair( PARAM_AUSREDIRECT,				"AUSRedirect" ) );
	m_strByParam ->insert( make_pair( PARAM_KIX,						"KIX" ) );
	m_strByParam ->insert( make_pair( PARAM_JAPANPOST,					"JapanPost" ) );
	m_strByParam ->insert( make_pair( PARAM_KOREAPOST,					"KoreaPost" ) );
	m_strByParam ->insert( make_pair( PARAM_CODE16K,					"Code16K" ) );
	m_strByParam ->insert( make_pair( PARAM_CODE49,						"Code49" ) );
	m_strByParam ->insert( make_pair( PARAM_PDF417,						"PDF417" ) );
	m_strByParam ->insert( make_pair( PARAM_PDF417_TRUNC,				"PDF417Trunc" ) );
	m_strByParam ->insert( make_pair( PARAM_MICROPDF,					"MicroPDF" ) );
	m_strByParam ->insert( make_pair( PARAM_DATAMATRIX,					"DataMatrix" ) );
	m_strByParam ->insert( make_pair( PARAM_QRCODE,						"QRCode" ) );
	m_strByParam ->insert( make_pair( PARAM_MICROQR,					"MicroQR" ) );
	m_strByParam ->insert( make_pair( PARAM_MAXICODE,					"MaxiCode" ) );
	m_strByParam ->insert( make_pair( PARAM_AZTEC,						"Aztec" ) );
	m_strByParam ->insert( make_pair( PARAM_AZRUNE,						"AzRune" ) );
	m_strByParam ->insert( make_pair( PARAM_CODEONE,					"CodeOne" ) );
	m_strByParam ->insert( make_pair( PARAM_GRIDMATRIX,					"GridMatrix" ) );
	m_strByParam ->insert( make_pair( PARAM_HIBC_CODE39,				"HIBCCode39" ) );
	m_strByParam ->insert( make_pair( PARAM_HIBC_CODE128,				"HIBCCode128" ) );
	m_strByParam ->insert( make_pair( PARAM_HIBC_PDF417,				"HIBCPDF417" ) );
	m_strByParam ->insert( make_pair( PARAM_HIBC_MICROPDF,				"HIBCMicroPDF" ) );
	m_strByParam ->insert( make_pair( PARAM_HIBC_DATAMATRIX,			"HIBCDataMatrix" ) );
	m_strByParam ->insert( make_pair( PARAM_HIBC_QRCODE,				"HIBCQRCode" ) );
	m_strByParam ->insert( make_pair( PARAM_HIBC_AZTEC,					"HIBCAztec" ) );
	m_strByParam ->insert( make_pair( PARAM_CS_EAN,						"CCEAN" ) );
	m_strByParam ->insert( make_pair( PARAM_CS_EAN128,					"CCEAN128" ) );
	m_strByParam ->insert( make_pair( PARAM_CS_UPCA,					"CCUPCA" ) );
	m_strByParam ->insert( make_pair( PARAM_CS_UPCE,					"CCUPCE" ) );
	m_strByParam ->insert( make_pair( PARAM_CS_RSS14,					"CCRSS14" ) );
	m_strByParam ->insert( make_pair( PARAM_CS_RSS14_STACK,				"CCRSS14Stack" ) );
	m_strByParam ->insert( make_pair( PARAM_CS_RSS14_STACKOMNI,			"CCRSS14StackOmni" ) );
	m_strByParam ->insert( make_pair( PARAM_CS_RSS_LTD,					"CCRSSLtd" ) );
	m_strByParam ->insert( make_pair( PARAM_CS_RSS_EXP,					"CCRSSExp" ) );
	m_strByParam ->insert( make_pair( PARAM_CS_RSS_EXPSTACK,			"CCRSSExpStack" ) );
	m_strByParam ->insert( make_pair( PARAM_CHECK_DIGIT,				"CheckDigit" ) );
	m_strByParam ->insert( make_pair( PARAM_COLS,						"Cols" ) );
	m_strByParam ->insert( make_pair( PARAM_CHANNELS,					"Channels" ) );
	m_strByParam ->insert( make_pair( PARAM_SIZE,						"Size" ) );
	m_strByParam ->insert( make_pair( PARAM_MODE,						"Mode" ) );
	m_strByParam ->insert( make_pair( PARAM_SECURITY,					"Security" ) );
	m_strByParam ->insert( make_pair( PARAM_SQUARE,						"Square" ) );
	m_strByParam ->insert( make_pair( PARAM_GS1,						"GS1" ) );
	m_strByParam ->insert( make_pair( PARAM_SCALE,						"Scale" ) );
	m_strByParam ->insert( make_pair( PARAM_HEIGHT,						"Height" ) );
	m_strByParam ->insert( make_pair( PARAM_SHOW_VALUE,					"ShowValue" ) );
	m_strByParam ->insert( make_pair( PARAM_ANGLE,						"Angle" ) );
	m_strByParam ->insert( make_pair( PARAM_FONT_SIZE,					"FontSize" ) );
	m_strByParam ->insert( make_pair( PARAM_FONT,						"Font" ) );
	m_strByParam ->insert( make_pair( PARAM_FGCOLOR,					"FGColor" ) );
	m_strByParam ->insert( make_pair( PARAM_BGCOLOR,					"BGColor" ) );
	m_strByParam ->insert( make_pair( PARAM_WHITESPACE_WIDTH,			"WhiteSpace" ) );
	m_strByParam ->insert( make_pair( PARAM_BORDER_WIDTH,				"Border" ) );
	m_strByParam ->insert( make_pair( PARAM_BIND,						"Bind" ) );
	m_strByParam ->insert( make_pair( PARAM_BOX,						"Box" ) );

	FOR_EACH( l_strByParamIterator, m_strByParam )
	{
		m_paramByStr ->insert( make_pair( StrFunc::lc( l_strByParamIterator ->second ), l_strByParamIterator ->first ) );
	}
} // initialize



DocxCompilerField::DocxCompilerField(
	DocxCompilerItem*				p_item,
	FieldType						p_type,
	const string&					p_name,
	const string&					p_format,
	const map<FieldParam, string>*	p_valueByParam,
	xercesc::DOMElement*			p_placeHolderNode )
{
	m_item				= p_item;
	m_type				= p_type;
	m_name				= StrFunc::lc( p_name );
	m_format			= p_format;
	m_placeHolderNode	= p_placeHolderNode;

	if( p_valueByParam ) // can be null
		m_valueByParam = *p_valueByParam;
} // c'tor

DocxCompilerField::~DocxCompilerField()
{

} // d'tor



void DocxCompilerField::serialize( ZipFile* p_zipFile )
{
	// type is saved in the DocxCompilerItemFile::serializeFields() function
	p_zipFile ->writePtr( this );
	p_zipFile ->writePtr( m_item );
	p_zipFile ->writeStr( m_name );
} // serialize



void DocxCompilerField::setItem( DocxCompilerItem* p_item )
{
	m_item = p_item;
} // setItem

DocxCompilerItem* DocxCompilerField::getItem() const
{
	return m_item;
} // getItem

DocxCompilerField::FieldType DocxCompilerField::getType() const
{
	return m_type;
} // getType

string DocxCompilerField::getName() const
{
	return m_name;
} // getName

string DocxCompilerField::getFormat() const
{
	return m_format;
} // getFormat

const map<DocxCompilerField::FieldParam, string>* DocxCompilerField::getValueByParam() const
{
	return &m_valueByParam;
} // getValueByParam

xercesc::DOMElement* DocxCompilerField::getPlaceHolderNode() const
{
	return m_placeHolderNode;
} // getPlaceHolderNode
