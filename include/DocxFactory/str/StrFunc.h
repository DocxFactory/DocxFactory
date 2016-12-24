
#ifndef __DOCXFACTORY_STR_FUNC_H__
#define __DOCXFACTORY_STR_FUNC_H__

#include "boost/type_traits.hpp"
#include "boost/utility/enable_if.hpp"

#include <set>
#include <string>
#include <cstring>
#include <cmath>



namespace DocxFactory
{	
	using namespace std;

	class StrFunc
	{
	public:
		template <class T>
		static typename boost::enable_if<boost::is_signed<T>, string>::type			intToStr( T p_val );

		template <class T>
		static typename boost::enable_if<boost::is_unsigned<T>, string>::type		intToStr( T p_val );

		template <class T>
		static typename boost::enable_if<boost::is_floating_point<T>, string>::type	intToStr( T p_val, unsigned short p_precision = 3 );

		template <class T>
		static typename boost::enable_if<boost::is_integral<T>, T>::type			strToInt( const string& p_str );

		template <class T>
		static typename boost::enable_if<boost::is_floating_point<T>, T>::type		strToInt( const string& p_str );

		static bool		isNum			( const string& p_str );
		static bool		isHex			( const string& p_str );

		static bool		begins			( const string& p_str, const string& p_prefix );
		static bool		ends			( const string& p_str, const string& p_suffix );
		static string	caps			( const string& p_str );
		static string	lc				( const string& p_str );

		static string	trim			( const string& p_str, const string& p_chars = "\n\r\t " );
		static string	leftTrim		( const string& p_str, const string& p_chars = "\n\r\t " );
		static string	rightTrim		( const string& p_str, const string& p_chars = "\n\r\t " );
		static string	removeDuplicate ( const string& p_str, const string& p_chars = "\n\r\t " );
		static string	replace			( const string& p_str, const string& p_from, const string& p_to );
		static string	padRight		( const string& p_str, size_t p_len, char p_ch = ' ' );
		static string	padLeft			( const string& p_str, size_t p_len, char p_ch = ' ' );

		static void		strToSet		( const string& p_str, set<string>& p_set, const string& p_delim = "," );
		static string	setToStr		( const set<string>& p_set,	const string& p_delim = "," );

		static string	encodeBase64	( const char* p_data, size_t p_len );
		static string	decodeBase64	( const string& p_str );

		static string	normalizeColor	( const string& p_str );

	protected:

	private:
		static const string g_base64Chars;
		static bool isBase64( char c );

		StrFunc();
		StrFunc( const StrFunc& p_other );
		StrFunc& operator = ( const StrFunc& p_other );

	};

	template <class T>
	typename boost::enable_if<boost::is_signed<T>, string>::type StrFunc::intToStr( T p_val )
	{
		// the if below is necessary otherwise 0 will return blank. it's not an optimization.
		if ( p_val == 0 )
			return "0";

		const int		l_maxSize = 30;
		char			l_str[ l_maxSize ];
		char			l_ch;
		bool			l_neg	= false;
		unsigned short	l_pos	= l_maxSize - 1;

		if ( p_val < 0 )
		{
			l_neg = true;
			p_val = -p_val;
		}

		l_str[ l_pos ] = '\0';

		while ( p_val > 0 )
		{
			l_ch	= p_val % 10;
			p_val	= ( p_val - l_ch ) / 10;

			l_pos--;
			l_str[ l_pos ] = ( l_ch + '0' );
		}

		if ( l_neg )
		{
			l_pos--;
			l_str[ l_pos ] = '-';
		}

		return l_str + l_pos;
	} // intToStr<signed>

	template <class T>
	typename boost::enable_if<boost::is_unsigned<T>, string>::type StrFunc::intToStr( T p_val )
	{
		// the if below is necessary otherwise 0 will return blank. it's not an optimization.
		if ( p_val == 0 )
			return "0";

		const int		l_maxSize = 30;
		char			l_str[ l_maxSize ];
		char			l_ch;
		unsigned short	l_pos	= l_maxSize - 1;

		l_str[ l_pos ] = '\0';

		while ( p_val > 0 )
		{
			l_ch	= p_val % 10;
			p_val	= ( p_val - l_ch ) / 10;

			l_pos--;
			l_str[ l_pos ] = ( l_ch + '0' );
		}

		return l_str + l_pos;
	} // intToStr<unsigned>
 
	template <class T>
	typename boost::enable_if<boost::is_floating_point<T>, string>::type StrFunc::intToStr( T p_val, unsigned short p_precision )
	{
		const int		l_maxSize = 50; // -30.10
		char			l_buff	[ l_maxSize ];
		char			l_str	[ l_maxSize ];
		char			l_ch;

		bool			l_neg = false;
		long long		l_int;
		double			l_frac;
		double			l_val;

		unsigned short	l_pos;
		unsigned short	l_len = 0;

		if ( p_val < 0 )
		{
			l_neg = true;
			p_val = -p_val;
		}

		if ( p_val > 1e+30f )
			return "?";

		if ( p_precision > 10 )
			p_precision = 10;

		if ( p_precision > 0 )
		{
			double l_pow = pow( 10.0f, p_precision );
			p_val = p_val * l_pow;

			l_frac = modf( p_val, &l_val );
			if ( l_frac >= 0.5 ) l_val++;

			p_val = ( l_val + 0.01 ) / l_pow;

			l_frac	= modf( p_val, &l_val );
			l_int	= ( long long ) l_val;
		}

		else
		{
			l_frac = modf( p_val, &l_val );
			if ( l_frac >= 0.5 ) l_val++;

			// not required because l_frac will not be used - l_frac = 0.0f;
			l_int = ( long long ) l_val;
		}



		if ( l_int == 0 )
		{
			strcpy( l_str, "0" );
			l_len = 1;
		}

		else
		{
			l_pos = l_maxSize - 1;
			l_buff[ l_pos ] = '\0';

			while ( l_int > 0 )
			{
				l_ch = l_int % 10;
				l_int = ( l_int - l_ch ) / 10;

				l_pos--;
				l_buff[ l_pos ] = ( l_ch + '0' );
			}

			if ( l_neg )
			{
				l_pos--;
				l_buff[ l_pos ] = '-';
			}

			strcpy( l_str, l_buff + l_pos );
			l_len = l_maxSize - l_pos - 1;
		}



		if ( p_precision > 0 )
		{
			size_t	l_lastPos;
			bool	l_zero = false;

			l_pos = 0;

			while ( p_precision > l_pos )
			{
				l_frac	= l_frac * 10;
				l_frac	= modf( l_frac, &l_val );

				l_ch = ( char ) l_val + '0';
				if ( l_ch == '0' )
				{
					if ( !l_zero )
					{
						l_lastPos	= l_pos;
						l_zero		= true;
					}
				}

				else
					l_zero = false;

				l_buff[ l_pos ] = l_ch;
				l_pos++;
			}

			if ( l_zero )
				l_pos = l_lastPos;

			if ( l_pos > 0 )
			{
				l_buff[ l_pos ] = '\0';

				l_str[ l_len ] = '.';
				l_len++;

				strcpy( l_str + l_len, l_buff );
				// not required because this is the last concat - l_len += l_pos;
			}
		}

		return l_str;
	} // intToStr<floating_point>



	template <class T>
	typename boost::enable_if<boost::is_integral<T>, T>::type StrFunc::strToInt( const string& p_str )
	{
		const char*	l_str;
		T			l_retVal	= 0;
		bool		l_neg		= false;

		l_str = p_str.c_str();

		if ( *l_str == '-' )
		{
			l_neg = true;
			l_str++;
		}

		else if ( *l_str == '+' )
			l_str++;

		for (; *l_str; l_str++ )
		{
			if ( *l_str == ',' )
				continue;

			else if ( *l_str < '0' || *l_str > '9' )
				break;

			l_retVal = 10 * l_retVal + ( *l_str - '0' );
		}

		if ( l_neg )
			l_retVal = -1 * l_retVal;

		return l_retVal;
	} // strToInt<is_integral>

	template <class T>
	typename boost::enable_if<boost::is_floating_point<T>, T>::type StrFunc::strToInt( const string& p_str )
	{
		const char*	l_str;
		T			l_retVal	= 0;
		T			l_pow		= 1;
		bool		l_neg		= false;

		l_str = p_str.c_str();

		if ( *l_str == '-' )
		{
			l_neg = true;
			l_str++;
		}

		else if ( *l_str == '+' )
			l_str++;

		for (; *l_str; l_str++ )
		{
			if ( *l_str == ',' )
				continue;

			else if ( *l_str == '.' )
			{
				l_str++;

				for (; *l_str; l_str++ )
				{
					if ( *l_str < '0' || *l_str > '9' )
						break;

					l_pow = 10 * l_pow;
					l_retVal += ( *l_str - '0' ) / l_pow;
				}
				break;
			}

			else if ( *l_str < '0' || *l_str > '9' )
				break;

			l_retVal = 10 * l_retVal + ( *l_str - '0' );
		}

		if ( l_neg )
			l_retVal = -l_retVal;

		return l_retVal;
	} // strToInt<floating_point>
};

#endif
