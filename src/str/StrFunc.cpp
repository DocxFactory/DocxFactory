
#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/str/StrEntries.h"

#include "DocxFactory/util/DocxFactoryDefs.h"

#include "boost/scoped_array.hpp"

#include <cstdlib>
#include <cstdio>
#include <cctype>

using namespace DocxFactory;
using namespace std;



const string StrFunc::g_base64Chars =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";

inline bool StrFunc::isBase64( char c )
{
	return ( isalnum(c) || c == '+' || c == '/' );
} // isBase64



bool StrFunc::isNum( const string& p_str )
{
	// the if below is necessary otherwise blank will be num. it's not an optimization.
	if ( p_str.empty() )
		return false;

	const char*	l_str;

	l_str = p_str.c_str();

	if ( *l_str == '-' || *l_str == '+' )
		l_str++;

	for (; *l_str; l_str++ )
	{
		if ( *l_str == ',' )
			continue;

		else
		if ( *l_str == '.' )
		{
			l_str++;

			for (; *l_str; l_str++ )
			{
				if ( !IS_DIGIT( *l_str ) )
					return false;
			}

			break;
		}

		else
		if ( !IS_DIGIT( *l_str ) )
			return false;
	}

	return true;
} // isNum

bool StrFunc::isHex( const string& p_str )
{
	const char*	l_str;

	l_str = p_str.c_str();

	for (; *l_str; l_str++ )
	{
		if ( !IS_HEXDIGIT( *l_str ) )
			return false;
	}

	return true;
} // isHex


bool StrFunc::begins( const string& p_str, const string& p_prefix )
{
	size_t	l_lenStr	= p_str.length();
	size_t	l_lenPrefix	= p_prefix.length();

	return ( l_lenStr >= l_lenPrefix && p_str.substr( 0, l_lenPrefix ) == p_prefix );
} // begins

bool StrFunc::ends( const string& p_str, const string& p_suffix )
{
	size_t	l_lenStr	= p_str.length();
	size_t	l_lenSuffix	= p_suffix.length();

	return ( l_lenStr >= l_lenSuffix && p_str.substr( l_lenStr - l_lenSuffix, l_lenSuffix ) == p_suffix );
} // ends

string StrFunc::caps( const string& p_str )
{
	size_t	l_len = p_str.length();
	size_t	l_pos;

	boost::scoped_array<char>	l_str( new char[ l_len + 1 ] );

	for ( l_pos = 0; l_pos < l_len; ++l_pos )
		l_str[ l_pos ] = toupper( p_str[ l_pos ] );

	l_str[ l_pos ] = NULL;

	return l_str.get();
} // caps

string StrFunc::lc( const string& p_str )
{
	size_t	l_len = p_str.length();
	size_t	l_pos;

	boost::scoped_array<char>	l_str( new char[ l_len + 1 ] );

	for ( l_pos = 0; l_pos < l_len; ++l_pos )
		l_str[ l_pos ] = tolower( p_str[ l_pos ] );

	l_str[ l_pos ] = NULL;

	return l_str.get();
} // lc



string StrFunc::leftTrim( const string& p_str, const string& p_chars )
{
	size_t	l_len	= p_str.length();
	size_t	l_pos	= 0;

	while ( true )
	{
		if ( l_pos >= l_len )
			return "";

		if ( !strchr( p_chars.c_str(), p_str[ l_pos ] ) )
			break;

		l_pos++;
	}

	return p_str.substr( l_pos );
} // leftTrim

string StrFunc::rightTrim( const string& p_str, const string& p_chars )
{
	size_t	l_pos = p_str.length() - 1;

	while ( true )
	{
		if ( l_pos == -1 )
			return "";

		if ( !strchr( p_chars.c_str(), p_str[ l_pos ] ) )
			break;

		l_pos--;
	}

	return p_str.substr( 0, l_pos + 1 );
} // rightTrim

string StrFunc::trim( const string& p_str, const string& p_chars )
{
	size_t	l_len	= p_str.length();
	size_t	l_pos1	= 0;
	size_t	l_pos2	= l_len - 1;

	while ( true )
	{
		if ( l_pos1 >= l_len )
			return "";

		if ( !strchr( p_chars.c_str(), p_str[ l_pos1 ] ) )
			break;

		l_pos1++;
	}

	while ( true )
	{
		if ( l_pos2 == -1 )
			return "";

		if ( !strchr( p_chars.c_str(), p_str[ l_pos2 ] ) )
			break;

		l_pos2--;
	}

	return p_str.substr( l_pos1, l_pos2 - l_pos1 + 1 );
} // trim



string StrFunc::removeDuplicate( const string& p_str, const string& p_chars )
{
	size_t	l_len = p_str.length();
	size_t	l_pos;

	boost::scoped_array<char>	l_str( new char[ l_len + 1 ] );
	size_t						l_strLen	= 0;

	const char*					l_chars		= p_chars.c_str();
	char						l_replace	= p_chars[ p_chars.length() - 1];

	for ( l_pos = 0; l_pos < l_len; ++l_pos )
	{
		if ( strchr( l_chars, p_str[ l_pos ] ) )
		{
			if ( l_strLen == 0 || l_str[ l_strLen - 1 ] != l_replace )
				l_str[ l_strLen++ ] = l_replace;
		}

		else
		{
			l_str[ l_strLen++ ] = p_str[ l_pos ];
		}
	}

	l_str[ l_strLen ] = NULL;

	return l_str.get();
} // removeDuplicate

string StrFunc::replace( const string& p_str, const string& p_from, const string& p_to )
{
	string	l_str		= p_str;
	size_t	l_fromLen	= p_from.length();
	size_t	l_toLen		= p_to.length();
	size_t	l_pos		= 0;

	while( ( l_pos = l_str.find( p_from, l_pos ) ) != std::string::npos ) 
	{
        l_str.replace( l_pos, l_fromLen, p_to );
		l_pos += l_toLen;
    }

	return l_str;
} // replace

string StrFunc::padRight( const string& p_str, size_t p_len, char p_ch )
{
	if ( p_str.length() > p_len )
		return p_str.substr( 0, p_len );

	else
	if ( p_str.length() < p_len )
	{
		string l_str = p_str;
		return l_str.insert( l_str.length(), p_len - l_str.length(), p_ch );
	}

	return p_str;
} // padRight

string StrFunc::padLeft( const string& p_str, size_t p_len, char p_ch )
{
	if ( p_str.length() > p_len )
		return p_str.substr( 0, p_len );

	else
	if ( p_str.length() < p_len )
	{
		string l_str = p_str;
		return l_str.insert( 0, p_len - l_str.length(), p_ch );
	}

	return p_str;
} // padLeft



void StrFunc::strToSet( const string& p_str, set<string>& p_set, const string& p_delim )
{
	p_set.clear();

	if ( p_str.empty() )
		return;

	size_t	l_len	= p_str.length();
	size_t	l_pos;
	size_t	l_start	= 0;

	while ( l_start < l_len )
	{
		l_pos = p_str.find( p_delim, l_start );

		if( l_pos == string::npos )
			l_pos = l_len;

		p_set.insert( p_str.substr( l_start, l_pos - l_start ) );

		l_start = l_pos + 1;
	}
} // strToSet

string StrFunc::setToStr( const set<string>& p_set, const string& p_delim )
{
	if ( p_set.empty() )
		return "";

	set<string>::const_iterator l_iterator;
	string						l_str;

	FOR_EACH( l_iterator, &p_set )
	{
		l_str += ( l_str.empty() ? "" : p_delim )
			+ *l_iterator;
	}

	return l_str;
} // setToStr



string StrFunc::encodeBase64( const char* p_data, size_t p_len )
{
	unsigned char l_block3[3];
	unsigned char l_block4[4];

	string l_str;
	size_t l_pos;

	int	i = 0;
	int	j = 0;

	for ( l_pos = 0; l_pos < p_len; l_pos++ )
	{
		l_block3[ i++ ] = ( unsigned char ) p_data[ l_pos ];

		if ( i == 3 )
		{
			l_block4[0] = ( l_block3[0] & 0xfc ) >> 2;
			l_block4[1] = ( ( l_block3[0] & 0x03 ) << 4 ) + ( ( l_block3[1] & 0xf0 ) >> 4 );
			l_block4[2] = ( ( l_block3[1] & 0x0f ) << 2 ) + ( ( l_block3[2] & 0xc0 ) >> 6 );
			l_block4[3] = l_block3[2] & 0x3f;

			for ( i = 0; i < 4; i++ )
				l_str += g_base64Chars[ l_block4[i] ];

			i = 0;
		}
	}

	if (i)
	{
		for ( j = i; j < 3; j++ )
			l_block3[j] = '\0';

		l_block4[0] = ( l_block3[0] & 0xfc ) >> 2;
		l_block4[1] = ( ( l_block3[0] & 0x03 ) << 4 ) + ( ( l_block3[1] & 0xf0 ) >> 4 );
		l_block4[2] = ( ( l_block3[1] & 0x0f ) << 2 ) + ( ( l_block3[2] & 0xc0 ) >> 6 );
		l_block4[3] = l_block3[2] & 0x3f;

	    for ( j = 0; j < i + 1; j++ )
			l_str += g_base64Chars[ l_block4[j] ];

		while ( i++ < 3 )
			l_str += '=';
	}

	return l_str;
} // encodeBase64

string StrFunc::decodeBase64( const string& p_data )
{
	unsigned char l_block3[3];
	unsigned char l_block4[4];

	string l_str;
	size_t l_len = p_data.length();
	size_t l_pos = 0;

	int	i = 0;
	int	j = 0;

	while ( l_pos < l_len && p_data[ l_pos ] != '=' && isBase64( p_data[ l_pos ] ) )
	{
		l_block4[ i++ ] = ( unsigned char ) p_data[ l_pos ];

	    if ( i == 4 )
		{
			for ( i = 0; i < 4; i++ )
				l_block4[i] = g_base64Chars.find( l_block4[i] );

			l_block3[0] = ( l_block4[0] << 2 ) + ( ( l_block4[1] & 0x30 ) >> 4 );
			l_block3[1] = ( ( l_block4[1] & 0xf ) << 4 ) + ( ( l_block4[2] & 0x3c ) >> 2 );
			l_block3[2] = ( ( l_block4[2] & 0x3 ) << 6 ) + l_block4[3];

			for ( i = 0; i < 3; i++ )
				l_str += l_block3[i];

			i = 0;
		}

		l_pos++;
	}

	if (i)
	{
		for ( j = i; j < 4; j++ )
			l_block4[j] = 0;

		for ( j = 0; j < 4; j++ )
			l_block4[j] = g_base64Chars.find( l_block4[j] );

		l_block3[0] = ( l_block4[0] << 2 ) + ( ( l_block4[1] & 0x30 ) >> 4 );
		l_block3[1] = ( ( l_block4[1] & 0xf ) << 4 ) + ( ( l_block4[2] & 0x3c ) >> 2 );
		l_block3[2] = ( ( l_block4[2] & 0x3 ) << 6 ) + l_block4[3];

	    for ( j = 0; j < i - 1; j++ )
			l_str += l_block3[j];
	}

	return l_str;
} // decodeBase64



string StrFunc::normalizeColor( const string& p_str )
{
	string l_str = caps( trim( p_str ) );

	if ( l_str.length() == 7 && l_str.substr( 0, 1 ) == "#" && isHex( l_str.substr(1) ) )
		 return l_str.substr(1);

	else
	if ( isHex( l_str ) )
		 return l_str;

	else if ( l_str == "WHITE" )								return "FFFFFF";
	else if ( l_str == "SILVER"		|| l_str == "LIGHTGRAY" )	return "C0C0C0";
	else if ( l_str == "GRAY"		|| l_str == "DARKGRAY" )	return "808080";
	else if ( l_str == "BLACK" )								return "000000";
	else if ( l_str == "RED" )									return "FF0000";
	else if ( l_str == "MAROON"		|| l_str == "DARKRED" )		return "800000";
	else if ( l_str == "YELLOW" )								return "FFFF00";
	else if ( l_str == "OLIVE"		|| l_str == "DARKYELLOW" )	return "808000";
	else if ( l_str == "LIME" )									return "00FF00";
	else if ( l_str == "GREEN"		|| l_str == "DARKGREEN" )	return "008000";
	else if ( l_str == "AQUA"		|| l_str == "CYAN" )		return "00FFFF";
	else if ( l_str == "TEAL"		|| l_str == "DARKCYAN" )	return "008080";
	else if ( l_str == "BLUE" )									return "0000FF";
	else if ( l_str == "NAVY"		|| l_str == "DARKBLUE" )	return "000080";
	else if ( l_str == "FUCHSIA"	|| l_str == "MAGENTA" )		return "FF00FF";
	else if ( l_str == "PURPLE"		|| l_str == "DARKMAGENTA" )	return "800080";
	else														return "";
} // normalizeColor
