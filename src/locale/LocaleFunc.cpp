
#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/locale/NumFormat.h"
#include "DocxFactory/locale/DateTimeFormat.h"
#include "DocxFactory/locale/InvalidCodePageException.h"
#include "DocxFactory/locale/InvalidFirstWeekDayException.h"
#include "DocxFactory/locale/InvalidDateFormatException.h"
#include "DocxFactory/locale/InvalidDateTimeStrException.h"
#include "DocxFactory/locale/InvalidYearOffsetException.h"
#include "DocxFactory/locale/IcuException.h"

#include "DocxFactory/time/TimeFunc.h"
#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/str/StrEntries.h"

#include "DocxFactory/util/DocxFactoryDefs.h"
#include "DocxFactory/util/tchar.h"

#include "unicode/ucnv.h"
#include "unicode/unistr.h"
#include "unicode/ustring.h"

#include "boost/scoped_array.hpp"

#include <locale>

#if defined( _WIN32 ) || defined( _WIN64 )
	#include <windows.h>
#else
	#include <langinfo.h>
#endif

using namespace DocxFactory;
using namespace std;



// static data members definition

bool		LocaleFunc::m_initialized			= false;
string		LocaleFunc::m_codePage				= "";
bool		LocaleFunc::m_singleByteCodePage	= false;
UConverter*	LocaleFunc::m_cnv					= NULL;

char		LocaleFunc::m_numFracSep			= 0;
char		LocaleFunc::m_numThSep				= 0; 

string						LocaleFunc::m_dateFormat	= "";
LocaleFunc::FirstWeekDay	LocaleFunc::m_firstWeekDay	= SUNDAY;
unsigned short				LocaleFunc::m_yearOffset	= 0;

string		LocaleFunc::m_weekDayFullNames[7];
string		LocaleFunc::m_weekDayFullNamesUtf8[7];
string		LocaleFunc::m_weekDayShortNames[7];
string		LocaleFunc::m_weekDayShortNamesUtf8[7];

string		LocaleFunc::m_monthFullNames[12];
string		LocaleFunc::m_monthFullNamesUtf8[12];
string		LocaleFunc::m_monthShortNames[12];
string		LocaleFunc::m_monthShortNamesUtf8[12];



void LocaleFunc::setCodePage()
{
	if ( !m_initialized )
		initialize();

	const char*	l_icuName;
	const char*	l_stdName;
	UErrorCode	l_err =	U_ZERO_ERROR;

	l_icuName = ucnv_getDefaultName();

	if ( l_stdName = ucnv_getStandardName( l_icuName, "MIME", &l_err ) )
		m_codePage = l_stdName;

	else
	if ( l_stdName = ucnv_getStandardName( l_icuName, "IANA", &l_err ) )
		m_codePage = l_stdName;

	else
		m_codePage = l_icuName;

	setCnv();
} // setCodePage

void LocaleFunc::setCodePage( const string& p_codePage )
{
	if ( !m_initialized )
		initialize();

	UErrorCode l_err = U_ZERO_ERROR;

	if ( ucnv_open( p_codePage.c_str(), &l_err ) == NULL )
		throw InvalidCodePageException( p_codePage,	__FILE__, __LINE__ );

	m_codePage = p_codePage;

	setCnv();

	setWeekDayNamesUtf8(
		m_weekDayFullNamesUtf8[0],
		m_weekDayFullNamesUtf8[1],
		m_weekDayFullNamesUtf8[2],
		m_weekDayFullNamesUtf8[3],
		m_weekDayFullNamesUtf8[4],
		m_weekDayFullNamesUtf8[5],
		m_weekDayFullNamesUtf8[6],
		
		m_weekDayShortNamesUtf8[0],
		m_weekDayShortNamesUtf8[1],
		m_weekDayShortNamesUtf8[2],
		m_weekDayShortNamesUtf8[3],
		m_weekDayShortNamesUtf8[4],
		m_weekDayShortNamesUtf8[5],
		m_weekDayShortNamesUtf8[6] );

	setMonthNamesUtf8(
		m_monthFullNamesUtf8[0],
		m_monthFullNamesUtf8[1],
		m_monthFullNamesUtf8[2],
		m_monthFullNamesUtf8[3],
		m_monthFullNamesUtf8[4],
		m_monthFullNamesUtf8[5],
		m_monthFullNamesUtf8[6],
		m_monthFullNamesUtf8[7],
		m_monthFullNamesUtf8[8],
		m_monthFullNamesUtf8[9],
		m_monthFullNamesUtf8[10],
		m_monthFullNamesUtf8[11],
		
		m_monthShortNamesUtf8[0],
		m_monthShortNamesUtf8[1],
		m_monthShortNamesUtf8[2],
		m_monthShortNamesUtf8[3],
		m_monthShortNamesUtf8[4],
		m_monthShortNamesUtf8[5],
		m_monthShortNamesUtf8[6],
		m_monthShortNamesUtf8[7],
		m_monthShortNamesUtf8[8],
		m_monthShortNamesUtf8[9],
		m_monthShortNamesUtf8[10],
		m_monthShortNamesUtf8[11] );
} // setCodePage



void LocaleFunc::setNumFracSep( char p_frac )
{
	if ( !m_initialized )
		initialize();

	if ( p_frac == 0 )
	{
		locale l_env( "" );
		m_numFracSep = use_facet<numpunct<char>>( l_env ).decimal_point();
	}

	else
		m_numFracSep = p_frac;
} // setNumFracSep

void LocaleFunc::setNumThSep( char p_th )
{
	if ( !m_initialized )
		initialize();

	if ( p_th == 0 )
	{
		locale l_env( "" );
		m_numThSep = use_facet<numpunct<char>>( l_env ).thousands_sep();
	}

	else
		m_numThSep = p_th;
} // setNumThSep



void LocaleFunc::setDateFormat( const string& p_dateFormat )
{
	if ( !m_initialized )
		initialize();

	string l_str;							 	
	time_get<char>::dateorder l_dateFormat;
	
	if ( p_dateFormat.empty() )
	{
		locale l_env( "" );

		l_dateFormat = use_facet<time_get<char>>( l_env ).date_order();

		switch( l_dateFormat )
		{
			case time_get<char>::dmy	: m_dateFormat = "dmy"; break;
			case time_get<char>::mdy	: m_dateFormat = "mdy"; break;
			case time_get<char>::ymd	: m_dateFormat = "ymd"; break;
			case time_get<char>::ydm	: m_dateFormat = "ydm"; break;
			default						: m_dateFormat = "mdy"; break;			
		}
	}

	else
	{
		l_str = StrFunc::lc( p_dateFormat );

		if ( l_str != "dmy"
		  && l_str != "mdy"
		  && l_str != "ymd"
		  && l_str != "ydm" )

			throw InvalidDateFormatException( p_dateFormat, __FILE__, __LINE__ );

		m_dateFormat = l_str;
	}
} // setDateFormat

void LocaleFunc::setFirstWeekDay()
{

#if defined( _WIN32 ) || defined( _WIN64 )

	TCHAR	l_buff[2];
	LCID	l_localeId = GetThreadLocale(); 

	GetLocaleInfo(
		l_localeId,
		LOCALE_IFIRSTDAYOFWEEK,
		l_buff,
		2 ) ;

	if ( l_buff[0] == _T('0') )
		m_firstWeekDay = MONDAY;

	else
		m_firstWeekDay = SUNDAY; 	 

#else

	char* l_byte = nl_langinfo( _NL_TIME_FIRST_WEEKDAY );

	if ( l_byte )
	{
		if ( *l_byte == 2 )
			m_firstWeekDay = MONDAY;

		else
			m_firstWeekDay = SUNDAY;
	}

	else
		m_firstWeekDay = SUNDAY;	

#endif
} // setFirstWeekDay

void LocaleFunc::setFirstWeekDay( FirstWeekDay p_weekday )
{
	if ( !m_initialized )
		initialize();

	m_firstWeekDay = p_weekday;
} // setFirstWeekDay

void LocaleFunc::setYearOffset( unsigned short p_year )
{
	if ( !m_initialized )
		initialize();

	if ( p_year == 0 )
		m_yearOffset = 50;

	else
	{
		if ( p_year < 1900 || p_year > 1999 )
			throw InvalidYearOffsetException( p_year, __FILE__, __LINE__ );

		m_yearOffset = p_year - 1900;
	}
} // setYearOffset



void LocaleFunc::setWeekDayNames()
{
	if ( !m_initialized )
		initialize();

	char			l_buff[ 30 ];
	tm				l_date;
	unsigned short	i;

	for ( i = 0; i < 7; i++ )
	{
		l_date.tm_wday = i;
		strftime( l_buff, 30, "%A", &l_date );

		m_weekDayFullNames[i]		= l_buff;
		m_weekDayFullNamesUtf8[i]	= convertToUtf8( string( l_buff ) );
	}

	for ( i = 0; i < 7 ; i++ )
	{
		l_date.tm_wday = i ;
		strftime( l_buff, 30, "%a", &l_date );

		m_weekDayShortNames[i]		= l_buff;
		m_weekDayShortNamesUtf8[i]	= convertToUtf8( string( l_buff ) );
	}
} // setWeekDayNames

void LocaleFunc::setWeekDayNames(
	const string& p_weekDay1,
	const string& p_weekDay2,
	const string& p_weekDay3,
	const string& p_weekDay4,
	const string& p_weekDay5,
	const string& p_weekDay6,
	const string& p_weekDay7 )
{
	if ( !m_initialized )
		initialize();

	m_weekDayFullNames[0]		= p_weekDay1;
	m_weekDayFullNames[1]		= p_weekDay2;
	m_weekDayFullNames[2]		= p_weekDay3;
	m_weekDayFullNames[3]		= p_weekDay4;
	m_weekDayFullNames[4]		= p_weekDay5;
	m_weekDayFullNames[5]		= p_weekDay6;
	m_weekDayFullNames[6]		= p_weekDay7;

	m_weekDayShortNames[0]		= getSubStr( p_weekDay1, 0, 3 );
	m_weekDayShortNames[1]		= getSubStr( p_weekDay2, 0, 3 );
	m_weekDayShortNames[2]		= getSubStr( p_weekDay3, 0, 3 );
	m_weekDayShortNames[3]		= getSubStr( p_weekDay4, 0, 3 );
	m_weekDayShortNames[4]		= getSubStr( p_weekDay5, 0, 3 );
	m_weekDayShortNames[5]		= getSubStr( p_weekDay6, 0, 3 );
	m_weekDayShortNames[6]		= getSubStr( p_weekDay7, 0, 3 );

	m_weekDayFullNamesUtf8[0]	= convertToUtf8( p_weekDay1 );
	m_weekDayFullNamesUtf8[1]	= convertToUtf8( p_weekDay2) ;
	m_weekDayFullNamesUtf8[2]	= convertToUtf8( p_weekDay3 );
	m_weekDayFullNamesUtf8[3]	= convertToUtf8( p_weekDay4 );
	m_weekDayFullNamesUtf8[4]	= convertToUtf8( p_weekDay5 );
	m_weekDayFullNamesUtf8[5]	= convertToUtf8( p_weekDay6 );
	m_weekDayFullNamesUtf8[6]	= convertToUtf8( p_weekDay7 );

	m_weekDayShortNamesUtf8[0]	= convertToUtf8( m_weekDayShortNames[0] );
	m_weekDayShortNamesUtf8[1]	= convertToUtf8( m_weekDayShortNames[1] );
	m_weekDayShortNamesUtf8[2]	= convertToUtf8( m_weekDayShortNames[2] );
	m_weekDayShortNamesUtf8[3]	= convertToUtf8( m_weekDayShortNames[3] );
	m_weekDayShortNamesUtf8[4]	= convertToUtf8( m_weekDayShortNames[4] );
	m_weekDayShortNamesUtf8[5]	= convertToUtf8( m_weekDayShortNames[5] );
	m_weekDayShortNamesUtf8[6]	= convertToUtf8( m_weekDayShortNames[6] );
} // setWeekDayNames

void LocaleFunc::setWeekDayNames(
	const string& p_fullName1,
	const string& p_fullName2,
	const string& p_fullName3,
	const string& p_fullName4,
	const string& p_fullName5,
	const string& p_fullName6,
	const string& p_fullName7,

	const string& p_shortName1,
	const string& p_shortName2,
	const string& p_shortName3,
	const string& p_shortName4,
	const string& p_shortName5,								
	const string& p_shortName6,
	const string& p_shortName7 )
{
	if ( !m_initialized )
		initialize();

	m_weekDayFullNames[0]		= p_fullName1;
	m_weekDayFullNames[1]		= p_fullName2;
	m_weekDayFullNames[2]		= p_fullName3;
	m_weekDayFullNames[3]		= p_fullName4;
	m_weekDayFullNames[4]		= p_fullName5;
	m_weekDayFullNames[5]		= p_fullName6;
	m_weekDayFullNames[6]		= p_fullName7;

	m_weekDayShortNames[0]		= p_shortName1;
	m_weekDayShortNames[1]		= p_shortName2;
	m_weekDayShortNames[2]		= p_shortName3;
	m_weekDayShortNames[3]		= p_shortName4;
	m_weekDayShortNames[4]		= p_shortName5;
	m_weekDayShortNames[5]		= p_shortName6;
	m_weekDayShortNames[6]		= p_shortName7;

	m_weekDayFullNamesUtf8[0]	= convertToUtf8( p_fullName1 );
	m_weekDayFullNamesUtf8[1]	= convertToUtf8( p_fullName2) ;
	m_weekDayFullNamesUtf8[2]	= convertToUtf8( p_fullName3 );
	m_weekDayFullNamesUtf8[3]	= convertToUtf8( p_fullName4 );
	m_weekDayFullNamesUtf8[4]	= convertToUtf8( p_fullName5 );
	m_weekDayFullNamesUtf8[5]	= convertToUtf8( p_fullName6 );
	m_weekDayFullNamesUtf8[6]	= convertToUtf8( p_fullName7 );

	m_weekDayShortNamesUtf8[0]	= convertToUtf8( p_shortName1 );
	m_weekDayShortNamesUtf8[1]	= convertToUtf8( p_shortName2 );
	m_weekDayShortNamesUtf8[2]	= convertToUtf8( p_shortName3 );
	m_weekDayShortNamesUtf8[3]	= convertToUtf8( p_shortName4 );
	m_weekDayShortNamesUtf8[4]	= convertToUtf8( p_shortName5 );
	m_weekDayShortNamesUtf8[5]	= convertToUtf8( p_shortName6 );
	m_weekDayShortNamesUtf8[6]	= convertToUtf8( p_shortName7 );
} // setWeekDayNames

void LocaleFunc::setWeekDayNamesUtf8()
{
	if ( !m_initialized )
		initialize();

	setWeekDayNames();
} // setWeekDayNamesUtf8

void LocaleFunc::setWeekDayNamesUtf8(
	const string& p_weekDay1,
	const string& p_weekDay2,
	const string& p_weekDay3,
	const string& p_weekDay4,
	const string& p_weekDay5,
	const string& p_weekDay6,
	const string& p_weekDay7 )
{
	if ( !m_initialized )
		initialize();

	m_weekDayFullNamesUtf8[0]	= p_weekDay1;
	m_weekDayFullNamesUtf8[1]	= p_weekDay2;
	m_weekDayFullNamesUtf8[2]	= p_weekDay3;
	m_weekDayFullNamesUtf8[3]	= p_weekDay4;
	m_weekDayFullNamesUtf8[4]	= p_weekDay5;
	m_weekDayFullNamesUtf8[5]	= p_weekDay6;
	m_weekDayFullNamesUtf8[6]	= p_weekDay7;

	m_weekDayShortNamesUtf8[0]	= getSubStrUtf8( p_weekDay1, 0, 3 );
	m_weekDayShortNamesUtf8[1]	= getSubStrUtf8( p_weekDay2, 0, 3 );
	m_weekDayShortNamesUtf8[2]	= getSubStrUtf8( p_weekDay3, 0, 3 );
	m_weekDayShortNamesUtf8[3]	= getSubStrUtf8( p_weekDay4, 0, 3 );
	m_weekDayShortNamesUtf8[4]	= getSubStrUtf8( p_weekDay5, 0, 3 );
	m_weekDayShortNamesUtf8[5]	= getSubStrUtf8( p_weekDay6, 0, 3 );
	m_weekDayShortNamesUtf8[6]	= getSubStrUtf8( p_weekDay7, 0, 3 );

	m_weekDayFullNames[0]		= convertFromUtf8( p_weekDay1 );
	m_weekDayFullNames[1]		= convertFromUtf8( p_weekDay2 );
	m_weekDayFullNames[2]		= convertFromUtf8( p_weekDay3 );
	m_weekDayFullNames[3]		= convertFromUtf8( p_weekDay4 );
	m_weekDayFullNames[4]		= convertFromUtf8( p_weekDay5 );
	m_weekDayFullNames[5]		= convertFromUtf8( p_weekDay6 );
	m_weekDayFullNames[6]		= convertFromUtf8( p_weekDay7 );

	m_weekDayShortNames[0]		= convertFromUtf8( m_weekDayShortNamesUtf8[0] );
	m_weekDayShortNames[1]		= convertFromUtf8( m_weekDayShortNamesUtf8[1] );
	m_weekDayShortNames[2]		= convertFromUtf8( m_weekDayShortNamesUtf8[2] );
	m_weekDayShortNames[3]		= convertFromUtf8( m_weekDayShortNamesUtf8[3] );
	m_weekDayShortNames[4]		= convertFromUtf8( m_weekDayShortNamesUtf8[4] );
	m_weekDayShortNames[5]		= convertFromUtf8( m_weekDayShortNamesUtf8[5] );
	m_weekDayShortNames[6]		= convertFromUtf8( m_weekDayShortNamesUtf8[6] );
} // setWeekDayNamesUtf8

void LocaleFunc::setWeekDayNamesUtf8(
	const string& p_fullName1,
	const string& p_fullName2,
	const string& p_fullName3,
	const string& p_fullName4,
	const string& p_fullName5,
	const string& p_fullName6,
	const string& p_fullName7,

	const string& p_shortName1,
	const string& p_shortName2,
	const string& p_shortName3,
	const string& p_shortName4,
	const string& p_shortName5,								
	const string& p_shortName6,
	const string& p_shortName7 )
{
	if ( !m_initialized )
		initialize();

	m_weekDayFullNamesUtf8[0]	= p_fullName1;
	m_weekDayFullNamesUtf8[1]	= p_fullName2;
	m_weekDayFullNamesUtf8[2]	= p_fullName3;
	m_weekDayFullNamesUtf8[3]	= p_fullName4;
	m_weekDayFullNamesUtf8[4]	= p_fullName5;
	m_weekDayFullNamesUtf8[5]	= p_fullName6;
	m_weekDayFullNamesUtf8[6]	= p_fullName7;

	m_weekDayShortNamesUtf8[0]	= p_shortName1;
	m_weekDayShortNamesUtf8[1]	= p_shortName2;
	m_weekDayShortNamesUtf8[2]	= p_shortName3;
	m_weekDayShortNamesUtf8[3]	= p_shortName4;
	m_weekDayShortNamesUtf8[4]	= p_shortName5;
	m_weekDayShortNamesUtf8[5]	= p_shortName6;
	m_weekDayShortNamesUtf8[6]	= p_shortName7;

	m_weekDayFullNames[0]		= convertFromUtf8( p_fullName1 );
	m_weekDayFullNames[1]		= convertFromUtf8( p_fullName2 );
	m_weekDayFullNames[2]		= convertFromUtf8( p_fullName3 );
	m_weekDayFullNames[3]		= convertFromUtf8( p_fullName4 );
	m_weekDayFullNames[4]		= convertFromUtf8( p_fullName5 );
	m_weekDayFullNames[5]		= convertFromUtf8( p_fullName6 );
	m_weekDayFullNames[6]		= convertFromUtf8( p_fullName7 );

	m_weekDayShortNames[0]		= convertFromUtf8( p_shortName1 );
	m_weekDayShortNames[1]		= convertFromUtf8( p_shortName2 );
	m_weekDayShortNames[2]		= convertFromUtf8( p_shortName3 );
	m_weekDayShortNames[3]		= convertFromUtf8( p_shortName4 );
	m_weekDayShortNames[4]		= convertFromUtf8( p_shortName5 );
	m_weekDayShortNames[5]		= convertFromUtf8( p_shortName6 );
	m_weekDayShortNames[6]		= convertFromUtf8( p_shortName7 );
} // setWeekDayNamesUtf8

void LocaleFunc::setMonthNames()
{
	if ( !m_initialized )
		initialize();

	char			l_buff[30];
	tm				l_date;
	unsigned short	i;

	for( i = 0; i < 12 ; i++ )
	{
		l_date.tm_mon = i ;
		strftime( l_buff, 20, "%B", &l_date );

		m_monthFullNames[i]		= l_buff;
		m_monthFullNamesUtf8[i]	= convertToUtf8( l_buff );
	}

	for( i = 0; i < 12 ; i++ )
	{
		l_date.tm_mon = i ;
		strftime( l_buff, 20, "%b", &l_date );

		m_monthShortNames[i]		= l_buff;
		m_monthShortNamesUtf8[i]	= convertToUtf8 ( l_buff );
	}
} // setMonthNames

void LocaleFunc::setMonthNames(
	const string& p_month1,
	const string& p_month2,
	const string& p_month3,
	const string& p_month4,
	const string& p_month5,
	const string& p_month6,
	const string& p_month7,
	const string& p_month8,
	const string& p_month9,
	const string& p_month10,
	const string& p_month11,
	const string& p_month12 )
{
	if ( !m_initialized )
		initialize();

	m_monthFullNames[0]			= p_month1;
	m_monthFullNames[1]			= p_month2;
	m_monthFullNames[2]			= p_month3;
	m_monthFullNames[3]			= p_month4;
	m_monthFullNames[4]			= p_month5;
	m_monthFullNames[5]			= p_month6;
	m_monthFullNames[6]			= p_month7;
	m_monthFullNames[7]			= p_month8;
	m_monthFullNames[8]			= p_month9;
	m_monthFullNames[9]			= p_month10;
	m_monthFullNames[10]		= p_month11;
	m_monthFullNames[11]		= p_month12;

	m_monthShortNames[0]		= getSubStr( p_month1, 0, 3 );
	m_monthShortNames[1]		= getSubStr( p_month2, 0, 3 );
	m_monthShortNames[2]		= getSubStr( p_month3, 0, 3 );
	m_monthShortNames[3]		= getSubStr( p_month4, 0, 3 );
	m_monthShortNames[4]		= getSubStr( p_month5, 0, 3 );
	m_monthShortNames[5]		= getSubStr( p_month6, 0, 3 );
	m_monthShortNames[6]		= getSubStr( p_month7, 0, 3 );
	m_monthShortNames[7]		= getSubStr( p_month8, 0, 3 );
	m_monthShortNames[8]		= getSubStr( p_month9, 0, 3 );
	m_monthShortNames[9]		= getSubStr( p_month10, 0, 3 );
	m_monthShortNames[10]		= getSubStr( p_month11, 0, 3 );
	m_monthShortNames[11]		= getSubStr( p_month12, 0, 3 );

	m_monthFullNamesUtf8[0]		= convertToUtf8( p_month1 );
	m_monthFullNamesUtf8[1]		= convertToUtf8( p_month2 );
	m_monthFullNamesUtf8[2]		= convertToUtf8( p_month3 );
	m_monthFullNamesUtf8[3]		= convertToUtf8( p_month4 );
	m_monthFullNamesUtf8[4]		= convertToUtf8( p_month5 );
	m_monthFullNamesUtf8[5]		= convertToUtf8( p_month6 );
	m_monthFullNamesUtf8[6]		= convertToUtf8( p_month7 );
	m_monthFullNamesUtf8[7]		= convertToUtf8( p_month8 );
	m_monthFullNamesUtf8[8]		= convertToUtf8( p_month9 );
	m_monthFullNamesUtf8[9]		= convertToUtf8( p_month10 );
	m_monthFullNamesUtf8[10]	= convertToUtf8( p_month11 );
	m_monthFullNamesUtf8[11]	= convertToUtf8( p_month12 );

	m_monthShortNamesUtf8[0]	= convertToUtf8( m_monthShortNames[0] );
	m_monthShortNamesUtf8[1]	= convertToUtf8( m_monthShortNames[1] );
	m_monthShortNamesUtf8[2]	= convertToUtf8( m_monthShortNames[2] );
	m_monthShortNamesUtf8[3]	= convertToUtf8( m_monthShortNames[3] );
	m_monthShortNamesUtf8[4]	= convertToUtf8( m_monthShortNames[4] );
	m_monthShortNamesUtf8[5]	= convertToUtf8( m_monthShortNames[5] );
	m_monthShortNamesUtf8[6]	= convertToUtf8( m_monthShortNames[6] );
	m_monthShortNamesUtf8[7]	= convertToUtf8( m_monthShortNames[7] );
	m_monthShortNamesUtf8[8]	= convertToUtf8( m_monthShortNames[8] );
	m_monthShortNamesUtf8[9]	= convertToUtf8( m_monthShortNames[9] );
	m_monthShortNamesUtf8[10]	= convertToUtf8( m_monthShortNames[10] );
	m_monthShortNamesUtf8[11]	= convertToUtf8( m_monthShortNames[11] );
} // setMonthNames

void LocaleFunc::setMonthNames(
	const string& p_fullName1,
	const string& p_fullName2,
	const string& p_fullName3,
	const string& p_fullName4,
	const string& p_fullName5,
	const string& p_fullName6,
	const string& p_fullName7,
	const string& p_fullName8, 
	const string& p_fullName9, 
	const string& p_fullName10, 
	const string& p_fullName11, 
	const string& p_fullName12,

	const string& p_shortName1,
	const string& p_shortName2,
	const string& p_shortName3,
	const string& p_shortName4,
	const string& p_shortName5,
	const string& p_shortName6,
	const string& p_shortName7, 
	const string& p_shortName8,
	const string& p_shortName9,
	const string& p_shortName10, 
	const string& p_shortName11, 
	const string& p_shortName12 )
{
	if ( !m_initialized )
		initialize();

	m_monthFullNames[0]			= p_fullName1;
	m_monthFullNames[1]			= p_fullName2;
	m_monthFullNames[2]			= p_fullName3;
	m_monthFullNames[3]			= p_fullName4;
	m_monthFullNames[4]			= p_fullName5;
	m_monthFullNames[5]			= p_fullName6;
	m_monthFullNames[6]			= p_fullName7;
	m_monthFullNames[7]			= p_fullName8;
	m_monthFullNames[8]			= p_fullName9;
	m_monthFullNames[9]			= p_fullName10;
	m_monthFullNames[10]		= p_fullName11;
	m_monthFullNames[11]		= p_fullName12;

	m_monthShortNames[0]		= p_shortName1;
	m_monthShortNames[1]		= p_shortName2;
	m_monthShortNames[2]		= p_shortName3;
	m_monthShortNames[3]		= p_shortName4;
	m_monthShortNames[4]		= p_shortName5;
	m_monthShortNames[5]		= p_shortName6;
	m_monthShortNames[6]		= p_shortName7;
	m_monthShortNames[7]		= p_shortName8;
	m_monthShortNames[8]		= p_shortName9;
	m_monthShortNames[9]		= p_shortName10;
	m_monthShortNames[10]		= p_shortName11;
	m_monthShortNames[11]		= p_shortName12;

	m_monthFullNamesUtf8[0]		= convertToUtf8( p_fullName1 );
	m_monthFullNamesUtf8[1]		= convertToUtf8( p_fullName2 );
	m_monthFullNamesUtf8[2]		= convertToUtf8( p_fullName3 );
	m_monthFullNamesUtf8[3]		= convertToUtf8( p_fullName4 );
	m_monthFullNamesUtf8[4]		= convertToUtf8( p_fullName5 );
	m_monthFullNamesUtf8[5]		= convertToUtf8( p_fullName6 );
	m_monthFullNamesUtf8[6]		= convertToUtf8( p_fullName7 );
	m_monthFullNamesUtf8[7]		= convertToUtf8( p_fullName8 );
	m_monthFullNamesUtf8[8]		= convertToUtf8( p_fullName9 );
	m_monthFullNamesUtf8[9]		= convertToUtf8( p_fullName10 );
	m_monthFullNamesUtf8[10]	= convertToUtf8( p_fullName11 );
	m_monthFullNamesUtf8[11]	= convertToUtf8( p_fullName12 );

	m_monthShortNamesUtf8[0]	= convertToUtf8( p_shortName1 );
	m_monthShortNamesUtf8[1]	= convertToUtf8( p_shortName2 );
	m_monthShortNamesUtf8[2]	= convertToUtf8( p_shortName3);
	m_monthShortNamesUtf8[3]	= convertToUtf8( p_shortName4);
	m_monthShortNamesUtf8[4]	= convertToUtf8( p_shortName5);
	m_monthShortNamesUtf8[5]	= convertToUtf8( p_shortName6 );
	m_monthShortNamesUtf8[6]	= convertToUtf8( p_shortName7 );
	m_monthShortNamesUtf8[7]	= convertToUtf8( p_shortName8 );
	m_monthShortNamesUtf8[8]	= convertToUtf8( p_shortName9 );
	m_monthShortNamesUtf8[9]	= convertToUtf8( p_shortName10 );
	m_monthShortNamesUtf8[10]	= convertToUtf8( p_shortName11 );
	m_monthShortNamesUtf8[11]	= convertToUtf8( p_shortName12 );
} // setMonthNames

void LocaleFunc::setMonthNamesUtf8()
{
	if ( !m_initialized )
		initialize();

	setMonthNames();
} // setMonthNamesUtf8

void LocaleFunc::setMonthNamesUtf8(
	const string& p_month1,
	const string& p_month2,
	const string& p_month3,
	const string& p_month4,
	const string& p_month5,
	const string& p_month6,
	const string& p_month7,
	const string& p_month8,
	const string& p_month9,
	const string& p_month10,
	const string& p_month11,
	const string& p_month12 )
{
	if ( !m_initialized )
		initialize();

	m_monthFullNamesUtf8[0]		= p_month1;
	m_monthFullNamesUtf8[1]		= p_month2;
	m_monthFullNamesUtf8[2]		= p_month3;
	m_monthFullNamesUtf8[3]		= p_month4;
	m_monthFullNamesUtf8[4]		= p_month5;
	m_monthFullNamesUtf8[5]		= p_month6;
	m_monthFullNamesUtf8[6]		= p_month7;
	m_monthFullNamesUtf8[7]		= p_month8;
	m_monthFullNamesUtf8[8]		= p_month9;
	m_monthFullNamesUtf8[9]		= p_month10;
	m_monthFullNamesUtf8[10]	= p_month11;
	m_monthFullNamesUtf8[11]	= p_month12;

	m_monthShortNamesUtf8[0]	= getSubStrUtf8( p_month1, 0, 3 );
	m_monthShortNamesUtf8[1]	= getSubStrUtf8( p_month2, 0, 3 );
	m_monthShortNamesUtf8[2]	= getSubStrUtf8( p_month3, 0, 3 );
	m_monthShortNamesUtf8[3]	= getSubStrUtf8( p_month4, 0, 3 );
	m_monthShortNamesUtf8[4]	= getSubStrUtf8( p_month5, 0, 3 );
	m_monthShortNamesUtf8[5]	= getSubStrUtf8( p_month6, 0, 3 );
	m_monthShortNamesUtf8[6]	= getSubStrUtf8( p_month7, 0, 3 );
	m_monthShortNamesUtf8[7]	= getSubStrUtf8( p_month8, 0, 3 );
	m_monthShortNamesUtf8[8]	= getSubStrUtf8( p_month9, 0, 3 );
	m_monthShortNamesUtf8[9]	= getSubStrUtf8( p_month10, 0, 3 );
	m_monthShortNamesUtf8[10]	= getSubStrUtf8( p_month11, 0, 3 );
	m_monthShortNamesUtf8[11]	= getSubStrUtf8( p_month12, 0, 3 );

	m_monthFullNames[0]			= convertFromUtf8( p_month1 );
	m_monthFullNames[1]			= convertFromUtf8( p_month2 );
	m_monthFullNames[2]			= convertFromUtf8( p_month3 );
	m_monthFullNames[3]			= convertFromUtf8( p_month4 );
	m_monthFullNames[4]			= convertFromUtf8( p_month5 );
	m_monthFullNames[5]			= convertFromUtf8( p_month6 );
	m_monthFullNames[6]			= convertFromUtf8( p_month7 );
	m_monthFullNames[7]			= convertFromUtf8( p_month8 );
	m_monthFullNames[8]			= convertFromUtf8( p_month9 );
	m_monthFullNames[9]			= convertFromUtf8( p_month10 );
	m_monthFullNames[10]		= convertFromUtf8( p_month11 );
	m_monthFullNames[11]		= convertFromUtf8( p_month12 );

	m_monthShortNames[0]		= convertFromUtf8( m_monthShortNamesUtf8[0] );
	m_monthShortNames[1]		= convertFromUtf8( m_monthShortNamesUtf8[1] );
	m_monthShortNames[2]		= convertFromUtf8( m_monthShortNamesUtf8[2] );
	m_monthShortNames[3]		= convertFromUtf8( m_monthShortNamesUtf8[3] );
	m_monthShortNames[4]		= convertFromUtf8( m_monthShortNamesUtf8[4] );
	m_monthShortNames[5]		= convertFromUtf8( m_monthShortNamesUtf8[5] );
	m_monthShortNames[6]		= convertFromUtf8( m_monthShortNamesUtf8[6] );
	m_monthShortNames[7]		= convertFromUtf8( m_monthShortNamesUtf8[7] );
	m_monthShortNames[8]		= convertFromUtf8( m_monthShortNamesUtf8[8] );
	m_monthShortNames[9]		= convertFromUtf8( m_monthShortNamesUtf8[9] );
	m_monthShortNames[10]		= convertFromUtf8( m_monthShortNamesUtf8[10] );
	m_monthShortNames[11]		= convertFromUtf8( m_monthShortNamesUtf8[11] );
} // setMonthNamesUtf8

void LocaleFunc::setMonthNamesUtf8(
	const string& p_fullName1,
	const string& p_fullName2,
	const string& p_fullName3,
	const string& p_fullName4,
	const string& p_fullName5,
	const string& p_fullName6,
	const string& p_fullName7,
	const string& p_fullName8, 
	const string& p_fullName9, 
	const string& p_fullName10, 
	const string& p_fullName11, 
	const string& p_fullName12,

	const string& p_shortName1,
	const string& p_shortName2,
	const string& p_shortName3,
	const string& p_shortName4,
	const string& p_shortName5,
	const string& p_shortName6,
	const string& p_shortName7, 
	const string& p_shortName8,
	const string& p_shortName9,
	const string& p_shortName10, 
	const string& p_shortName11, 
	const string& p_shortName12 )
{
	if ( !m_initialized )
		initialize();

	m_monthFullNamesUtf8[0]		= p_fullName1;
	m_monthFullNamesUtf8[1]		= p_fullName2;
	m_monthFullNamesUtf8[2]		= p_fullName3;
	m_monthFullNamesUtf8[3]		= p_fullName4;
	m_monthFullNamesUtf8[4]		= p_fullName5;
	m_monthFullNamesUtf8[5]		= p_fullName6;
	m_monthFullNamesUtf8[6]		= p_fullName7;
	m_monthFullNamesUtf8[7]		= p_fullName8;
	m_monthFullNamesUtf8[8]		= p_fullName9;
	m_monthFullNamesUtf8[9]		= p_fullName10;
	m_monthFullNamesUtf8[10]	= p_fullName11;
	m_monthFullNamesUtf8[11]	= p_fullName12;

	m_monthShortNamesUtf8[0]	= p_shortName1;
	m_monthShortNamesUtf8[1]	= p_shortName2;
	m_monthShortNamesUtf8[2]	= p_shortName3;
	m_monthShortNamesUtf8[3]	= p_shortName4;
	m_monthShortNamesUtf8[4]	= p_shortName5;
	m_monthShortNamesUtf8[5]	= p_shortName6;
	m_monthShortNamesUtf8[6]	= p_shortName7;
	m_monthShortNamesUtf8[7]	= p_shortName8;
	m_monthShortNamesUtf8[8]	= p_shortName9;
	m_monthShortNamesUtf8[9]	= p_shortName10;
	m_monthShortNamesUtf8[10]	= p_shortName11;
	m_monthShortNamesUtf8[11]	= p_shortName12;

	m_monthFullNames[0]			= convertFromUtf8( p_fullName1 );
	m_monthFullNames[1]			= convertFromUtf8( p_fullName2 );
	m_monthFullNames[2]			= convertFromUtf8( p_fullName3 );
	m_monthFullNames[3]			= convertFromUtf8( p_fullName4 );
	m_monthFullNames[4]			= convertFromUtf8( p_fullName5 );
	m_monthFullNames[5]			= convertFromUtf8( p_fullName6 );
	m_monthFullNames[6]			= convertFromUtf8( p_fullName7 );
	m_monthFullNames[7]			= convertFromUtf8( p_fullName8 );
	m_monthFullNames[8]			= convertFromUtf8( p_fullName9 );
	m_monthFullNames[9]			= convertFromUtf8( p_fullName10 );
	m_monthFullNames[10]		= convertFromUtf8( p_fullName11 );
	m_monthFullNames[11]		= convertFromUtf8( p_fullName12 );

	m_monthShortNames[0]		= convertFromUtf8( p_shortName1 );
	m_monthShortNames[1]		= convertFromUtf8( p_shortName2 );
	m_monthShortNames[2]		= convertFromUtf8( p_shortName3 );
	m_monthShortNames[3]		= convertFromUtf8( p_shortName4 );
	m_monthShortNames[4]		= convertFromUtf8( p_shortName5 );
	m_monthShortNames[5]		= convertFromUtf8( p_shortName6 );
	m_monthShortNames[6]		= convertFromUtf8( p_shortName7 );
	m_monthShortNames[7]		= convertFromUtf8( p_shortName8 );
	m_monthShortNames[8]		= convertFromUtf8( p_shortName9 );
	m_monthShortNames[9]		= convertFromUtf8( p_shortName10 );
	m_monthShortNames[10]		= convertFromUtf8( p_shortName11 );
	m_monthShortNames[11]		= convertFromUtf8( p_shortName12 );
} // setMonthNamesUtf8



string LocaleFunc::getCodePage()
{
	if ( !m_initialized )
		initialize();

	return m_codePage;
} // getCodePage

char LocaleFunc::getNumFracSep()
{
	if ( !m_initialized )
		initialize();

	return m_numFracSep;
} // getNumSep

char LocaleFunc::getNumThSep()
{
	if ( !m_initialized )
		initialize();

	return m_numThSep;
} // getNumDec

string LocaleFunc::getDateFormat()
{
	if ( !m_initialized )
		initialize();

	return m_dateFormat;
} // getDateFormat

unsigned short LocaleFunc::getYearOffset()
{
	if ( !m_initialized )
		initialize();

	return m_yearOffset + 1900;
} // getYearOffset

unsigned short LocaleFunc::getFirstWeekDay()
{
	if ( !m_initialized )
		initialize();

	return m_firstWeekDay;
} // getFirstWeekDay

string* LocaleFunc::getWeekDayFullNames()
{
	if ( !m_initialized )
		initialize();

	return m_weekDayFullNames;
} // getWeekDayFullNames

string* LocaleFunc::getWeekDayShortNames()
{
	if ( !m_initialized )
		initialize();

	return m_weekDayShortNames;
} // getWeekDayShortNames

string* LocaleFunc::getWeekDayFullNamesUtf8()
{
	if ( !m_initialized )
		initialize();

	return m_weekDayFullNamesUtf8;
} // getWeekDayFullNamesUtf8

string* LocaleFunc::getWeekDayShortNamesUtf8()
{
	if ( !m_initialized )
		initialize();

	return m_weekDayShortNamesUtf8;
} // getWeekDayShortNamesUtf8

string* LocaleFunc::getMonthFullNames()
{
	if ( !m_initialized )
		initialize();

	return m_monthFullNames;
} // getMonthFullNames

string* LocaleFunc::getMonthShortNames()
{
	if ( !m_initialized )
		initialize();

	return m_monthShortNames;
} // getMonthShortNames

string* LocaleFunc::getMonthFullNamesUtf8()
{
	if ( !m_initialized )
		initialize();

	return m_monthFullNamesUtf8;
} // getMonthFullNamesUtf8

string* LocaleFunc::getMonthShortNamesUtf8()
{
	if ( !m_initialized )
		initialize();

	return m_monthShortNamesUtf8;
} // getMonthShortNamesUtf8



unsigned short LocaleFunc::getWeekNum( tm* p_date )
{
	if ( !m_initialized )
		initialize();

	int	l_wDay;
	int	l_days;
	tm	l_date;
	int	l_retval = 0;

	// go to january 1 of p_date year, count days from the following m_firstWeekDay and convert the days to weeks.

	l_date.tm_year	= p_date->tm_year;
	l_date.tm_mday	= 1;
	l_date.tm_mon	= 0;	
	l_date.tm_hour	= 0;
	l_date.tm_min	= 0;
	l_date.tm_sec	= 0;
	
	TimeFunc::mkTime( &l_date );

	l_wDay	= l_date.tm_wday + 1;
	l_days	= p_date ->tm_yday + 1;

	if ( l_wDay != m_firstWeekDay )
		l_retval = l_retval + 1;

	if ( l_wDay > m_firstWeekDay )
		l_days = l_days - ( 7 - ( l_wDay - m_firstWeekDay ) );

	else if ( l_wDay < m_firstWeekDay )
		l_days = l_days - ( m_firstWeekDay - l_wDay );

	l_retval = l_retval + ( l_days - ( l_days % 7 ) ) / 7;

	if ( l_days % 7 != 0 )
		l_retval = l_retval + 1;

	return l_retval; 
} // getWeekNum

unsigned short LocaleFunc::getQuarter( tm* p_date )
{
	int i = p_date ->tm_mon + 1;

	i = ( ( i - i % 3 ) / 3  ) + ( i % 3 != 0 ? 1 : 0 );

	return i;
} // getQuarter



double LocaleFunc::strToNum( const string& p_str )
{
	if ( !m_initialized )
		initialize();

	enum Part
	{
		PREFIX = 0,
		INTEGER,
		FRACTION,
		SUFFIX
	};

	Part	l_part	= PREFIX;
	double	l_val	= 0;
	double	l_dec	= 1;
	char	l_neg	= 0;

	size_t	l_len	= p_str.length();
	size_t	l_pos	= 0;
	char	l_ch;

	while ( l_pos < l_len )
	{
		l_ch = p_str[ l_pos ];

		if ( l_ch == '+' || l_ch == '-' )
		{
			if ( l_neg != 0 )							
				break;

			if ( l_part == INTEGER || l_part == FRACTION )
				l_part = SUFFIX;

			l_neg = l_ch;
		}

		else if ( l_ch == ' '

			// if thousand separator is also space then treated as space only in prefix and suffix.
			&& ( m_numThSep != ' ' || l_part == PREFIX || l_part == SUFFIX ) )					
		{
			if ( l_part == INTEGER || l_part == FRACTION )
				l_part = SUFFIX;

			if ( l_part == SUFFIX  && l_neg != 0 )
				break;
		}

		else if ( IS_DIGIT( l_ch ) )
		{
			if ( l_part == PREFIX )
				l_part = INTEGER;

			switch ( l_part )
			{
				case INTEGER:
					l_val = l_val * 10 + ( l_ch - 48 );
					break;

				case FRACTION:
					l_dec = l_dec * 10;
					l_val = l_val + ( l_ch - 48 ) / l_dec; 
					break;
			}

			if ( l_part == SUFFIX )
				break;
		}

		else if ( l_ch == m_numFracSep )
		{
			if ( l_part == PREFIX || l_part == INTEGER )
				l_part = FRACTION;

			else
				break;
		}

		else if ( l_ch == m_numThSep )
		{
			if ( l_part != INTEGER && l_part != FRACTION )
				break;
		}

		else
			break;

		++l_pos;
	}

	if ( l_neg == '-' )
		l_val = -l_val;

	return l_val;
} // strToNum

string LocaleFunc::numToStr( double p_num, const NumFormat* p_format )
{
	if ( !m_initialized )
		initialize();

	string l_result;

	if ( p_num > 0 )
	{
		l_result = numToStrSingle( 
			p_num,
			p_format ->m_posPrefix,
			p_format ->m_posLeftDigCnt,
			p_format ->m_posRightDigCnt,
			p_format ->m_posRightExtCnt,
			p_format ->m_posSuffix,
			p_format ->m_posSep,
			p_format ->m_posPercent );
	}

	else if ( p_num < 0 )
	{
		l_result = numToStrSingle(
			p_num,
			p_format ->m_negPrefix,
			p_format ->m_negLeftDigCnt,
			p_format ->m_negRightDigCnt,
			p_format ->m_negRightExtCnt,
			p_format ->m_negSuffix,
			p_format ->m_negSep,
			p_format ->m_negPercent );
	}

	else
	{
		l_result = numToStrSingle(
			p_num,
			p_format ->m_zeroPrefix,
			p_format ->m_zeroLeftDigCnt,
			p_format ->m_zeroRightDigCnt,
			0,		
			p_format ->m_zeroSuffix,
			false,	
			false );
	}

	return l_result;
} // numToStr

string LocaleFunc::numToStr(
	double			p_num,
	unsigned short	p_rightDigCnt,
	unsigned short	p_rightExtCnt, 
	bool			p_sep )
{
	if ( !m_initialized )
		initialize();

	string	l_result;

	if ( p_num == 0 )
		l_result = "0";

	else
	{
		l_result = numToStrSingle( 
			p_num,
			( p_num < 0 ? "-" : "" ),	// prefix
			1,							// leftDigCnt
			p_rightDigCnt,				// rightDigCnt
			p_rightExtCnt,				// rightExtCnt
			"",							// suffix,
			p_sep,						// separator	
			false );					// percent 
	}

	return l_result;
} // numToStr

void LocaleFunc::getNumFormat( const string& p_format, NumFormat* p_numFormat )
{
	if ( !m_initialized )
		initialize();

	if ( p_format.empty() )
	{
		// set format to #,##0.#;-#,##0.#;0 

		p_numFormat ->m_posColor		= "";
		p_numFormat ->m_posPrefix		= "";
		p_numFormat ->m_posSuffix		= "";
		p_numFormat ->m_posLeftDigCnt	= 1;
		p_numFormat ->m_posRightDigCnt	= 0;
		p_numFormat ->m_posRightExtCnt	= 1;
		p_numFormat ->m_posSep			= true;
		p_numFormat ->m_posPercent		= false;

		p_numFormat ->m_negColor		= "";
		p_numFormat ->m_negPrefix		= "-";
		p_numFormat ->m_negSuffix		= "";
		p_numFormat ->m_negLeftDigCnt	= 1;
		p_numFormat ->m_negRightDigCnt	= 0;
		p_numFormat ->m_negRightExtCnt	= 1;
		p_numFormat ->m_negSep			= true;
		p_numFormat ->m_negPercent		= false;

		p_numFormat ->m_zeroColor		= "";
		p_numFormat ->m_zeroPrefix		= "";
		p_numFormat ->m_zeroSuffix		= "";
		p_numFormat ->m_zeroLeftDigCnt	= 1;
		p_numFormat ->m_zeroRightDigCnt	= 0;
		p_numFormat ->m_zeroSep			= false;
	}

	enum NumPart
	{
		NUM_PRE,
		NUM_COLOR,
		NUM_PREFIX,
		NUM_INT,
		NUM_DEC,
		NUM_SUFFIX
	};

	string			l_format;
	size_t			l_pos;
	size_t			l_len;
	char			l_ch;

	string			l_color;
	string			l_prefix;
	string			l_suffix;

	unsigned short	l_leftDig;
	unsigned short	l_rightDig;
	unsigned short	l_rightExt;

	bool			l_posFormatSet	= false;
	bool			l_negFormatSet	= false;
	bool			l_zeroFormatSet	= false;

	NumPart			l_part;
	bool			l_escape;
	bool			l_percent;
	bool			l_sep;
	bool			l_dec;
	string			l_str;

	p_numFormat ->m_posColor		= "";
	p_numFormat ->m_posPrefix		= "";
	p_numFormat ->m_posSuffix		= "";
	p_numFormat ->m_posLeftDigCnt	= 0;
	p_numFormat ->m_posRightDigCnt	= 0;
	p_numFormat ->m_posRightExtCnt	= 0;
	p_numFormat ->m_posSep			= false;
	p_numFormat ->m_posPercent		= false;

	p_numFormat ->m_negColor		= "";
	p_numFormat ->m_negPrefix		= "";
	p_numFormat ->m_negSuffix		= "";
	p_numFormat ->m_negLeftDigCnt	= 0;
	p_numFormat ->m_negRightDigCnt	= 0;
	p_numFormat ->m_negRightExtCnt	= 0;
	p_numFormat ->m_negSep			= false;
	p_numFormat ->m_negPercent		= false;

	p_numFormat ->m_zeroColor		= "";
	p_numFormat ->m_zeroPrefix		= "";
	p_numFormat ->m_zeroSuffix		= "";
	p_numFormat ->m_zeroLeftDigCnt	= 0;
	p_numFormat ->m_zeroRightDigCnt	= 0;
	p_numFormat ->m_zeroSep			= false;

	l_format	= StrFunc::trim( p_format );
	l_pos		= 0;
	l_len		= l_format.length();

	while ( true )
	{
		l_color     = "";
		l_prefix    = "";
		l_suffix    = "";
		l_leftDig   = 0;
		l_rightDig  = 0;
		l_rightExt  = 0;
		l_sep		= false;
		l_dec       = false;
		l_percent	= false;

		l_part = NUM_PRE;

		while ( l_pos < l_len )
		{
			l_escape	= false;
			l_ch		= l_format[ l_pos ];

			if ( l_ch == '\\' )
			{
				l_escape	= true;
				l_ch		= l_format[ l_pos + 1 ]; 
				l_pos++;
			}

			if ( l_ch == '[' && !l_escape )
			{
				switch ( l_part )
				{
				case NUM_PRE	: l_part = NUM_COLOR; break;
				case NUM_COLOR	: break;
				case NUM_PREFIX	: break;
				case NUM_INT	: l_part = NUM_SUFFIX; break;
				case NUM_DEC	: l_part = NUM_SUFFIX; break;
				case NUM_SUFFIX	: break;
				}

				switch ( l_part )
				{
				case NUM_COLOR	: l_color += l_ch; break;
				case NUM_PREFIX	: l_prefix += l_ch; break;
				case NUM_INT	: break;
				case NUM_DEC	: break;
				case NUM_SUFFIX	: l_suffix += l_ch; break;
				}
			}

			else
			if ( l_ch == ']' && !l_escape )
			{
				switch ( l_part )
				{
				case NUM_PRE	: l_part = NUM_PREFIX; break;
				case NUM_PREFIX	: break;
				case NUM_INT	: l_part = NUM_SUFFIX; break;
				case NUM_DEC	: l_part = NUM_SUFFIX; break;
				case NUM_SUFFIX	: break;
				}

				switch ( l_part )
				{
				case NUM_COLOR	: l_color += l_ch; break;
				case NUM_PREFIX	: l_prefix += l_ch; break;
				case NUM_INT	: break;
				case NUM_DEC	: break;
				case NUM_SUFFIX	: l_suffix += l_ch; break;
				}

				switch ( l_part )
				{
				case NUM_COLOR	: l_part = NUM_PREFIX; break;
				}
			}

			else
			if ( l_ch == '#' && !l_escape )
			{
				switch ( l_part )
				{
				case NUM_PRE	: l_part = NUM_INT; break;
				case NUM_COLOR	: break;
				case NUM_PREFIX	: l_part = NUM_INT; break;
				case NUM_INT	: if ( l_leftDig > 0 ) l_part = NUM_SUFFIX; break;
				case NUM_DEC	: break;
				case NUM_SUFFIX	: break;
				}

				switch ( l_part )
				{
				case NUM_COLOR	: l_color += l_ch; break;
				case NUM_PREFIX	: l_prefix += l_ch; break;
				case NUM_INT	: break;
				case NUM_DEC	: l_rightExt++; break;
				case NUM_SUFFIX	: l_suffix += l_ch; break;
				}
			}

			else
			if ( l_ch == ',' && !l_escape )
			{
				switch ( l_part )
				{
				case NUM_PRE	: l_part = NUM_PREFIX; break;
				case NUM_COLOR	: break;
				case NUM_PREFIX	: break;
				case NUM_INT	: break;
				case NUM_DEC	: l_part = NUM_SUFFIX; break;
				case NUM_SUFFIX	: break;
				}

				switch ( l_part )
				{
				case NUM_COLOR	: l_color += l_ch; break;
				case NUM_PREFIX	: l_prefix += l_ch; break;
				case NUM_INT	: l_sep = true; break;
				case NUM_DEC	: break;
				case NUM_SUFFIX	: l_suffix += l_ch; break;
				}
			}

			else
			if ( l_ch == '0' && !l_escape )
			{
				switch ( l_part )
				{
				case NUM_PRE	: l_part = NUM_INT; break;
				case NUM_COLOR	: break;
				case NUM_PREFIX	: l_part = NUM_INT; break;
				case NUM_INT	: break;
				case NUM_DEC	: if ( l_rightExt > 0 ) l_part = NUM_SUFFIX; break;
				case NUM_SUFFIX	: break;
				}

				switch ( l_part )
				{
				case NUM_COLOR	: l_color += l_ch; break;
				case NUM_PREFIX	: l_prefix += l_ch; break;
				case NUM_INT	: l_leftDig++; break;
				case NUM_DEC	: l_rightDig++; break;
				case NUM_SUFFIX	: l_suffix += l_ch; break;
				}
			}

			else
			if ( l_ch == '.' && !l_escape )
			{
				switch ( l_part )
				{
				case NUM_PRE	: l_part = NUM_DEC; break;
				case NUM_COLOR	: break;
				case NUM_PREFIX	:
				case NUM_INT	: l_part = NUM_DEC; break;
				case NUM_DEC	: l_part = NUM_SUFFIX; break;
				case NUM_SUFFIX	: break;
				}

				switch ( l_part )
				{
				case NUM_COLOR	: l_color += l_ch; break;
				case NUM_PREFIX	: l_prefix += l_ch; break;
				case NUM_INT	: break;
				case NUM_DEC	: break;
				case NUM_SUFFIX	: l_suffix += l_ch; break;
				}
			}

			else
			if ( l_ch != ';' || l_escape )
			{
				switch ( l_part )
				{
				case NUM_PRE	: l_part = NUM_PREFIX; break;
				case NUM_COLOR	: break;
				case NUM_PREFIX	: break;
				case NUM_INT	: l_part = NUM_SUFFIX; break;
				case NUM_DEC	: l_part = NUM_SUFFIX; break;
				case NUM_SUFFIX	: break;
				}

				switch ( l_part )
				{
				case NUM_COLOR	: l_color += l_ch; break;
				case NUM_PREFIX	: l_prefix += l_ch; break;
				case NUM_INT	: break;
				case NUM_DEC	: break;
				case NUM_SUFFIX	: l_suffix += l_ch; break;
				}

				if ( l_ch == '%' && !l_escape )
					l_percent = true;
			}

			l_pos++;

			if ( l_ch == ';' && !l_escape )
				break;
		}

		if ( !l_color.empty() ) {

			if ( l_color.length() > 2
			  && l_color[0] == '['
			  && l_color[ l_color.length() - 1 ] == ']' )
			{
				l_str = StrFunc::normalizeColor( l_color.substr( 1, l_color.length() - 2 ) );

				if(!l_str.empty() )
					l_color		= l_str;

				else
				{
					l_prefix	= l_color + l_prefix;
					l_color		= "";
				}
			}

			else 
			{
				l_prefix	= l_color + l_prefix;
				l_color		= "";
			}
		}

		l_prefix = StrFunc::leftTrim	( l_prefix );
		l_suffix = StrFunc::rightTrim	( l_suffix );

		if ( l_rightDig > 10 ) l_rightDig = 10;
		if ( l_rightExt > 10 ) l_rightExt = 10;

		if ( !l_posFormatSet )
		{
			p_numFormat ->m_posColor		= l_color;
			p_numFormat ->m_posPrefix		= l_prefix;
			p_numFormat ->m_posLeftDigCnt	= l_leftDig;
			p_numFormat ->m_posRightDigCnt	= l_rightDig;
			p_numFormat ->m_posRightExtCnt	= l_rightExt;
			p_numFormat ->m_posSuffix		= l_suffix;
			p_numFormat ->m_posSep			= l_sep;
			p_numFormat ->m_posPercent		= l_percent;

			l_posFormatSet = true;
		}

		else
		if ( !l_negFormatSet )
		{
			p_numFormat ->m_negColor		= l_color;
			p_numFormat ->m_negPrefix		= l_prefix;
			p_numFormat ->m_negLeftDigCnt	= l_leftDig;
			p_numFormat ->m_negRightDigCnt	= l_rightDig;
			p_numFormat ->m_negRightExtCnt	= l_rightExt;
			p_numFormat ->m_negSuffix		= l_suffix;
			p_numFormat ->m_negSep			= l_sep;
			p_numFormat ->m_negPercent		= l_percent;

			l_negFormatSet = true;
		}

		else
		if ( !l_zeroFormatSet )
		{
			p_numFormat ->m_zeroColor		= l_color;
			p_numFormat ->m_zeroPrefix		= l_prefix;
			p_numFormat ->m_zeroLeftDigCnt	= l_leftDig;
			p_numFormat ->m_zeroRightDigCnt	= l_rightDig;
			p_numFormat ->m_zeroSuffix		= l_suffix;
			p_numFormat ->m_zeroSep			= l_sep;

			l_zeroFormatSet = true;
		}

		if ( l_pos >= l_len )
			break;
	}

	if ( !l_negFormatSet )
	{
		p_numFormat ->m_negColor		= p_numFormat ->m_posColor;
		p_numFormat ->m_negPrefix		= "-" + p_numFormat ->m_posPrefix;
		p_numFormat ->m_negSuffix		= p_numFormat ->m_posSuffix;
		p_numFormat ->m_negLeftDigCnt	= p_numFormat ->m_posLeftDigCnt;
		p_numFormat ->m_negRightDigCnt	= p_numFormat ->m_posRightDigCnt;
		p_numFormat ->m_negSep			= p_numFormat ->m_posSep;
		p_numFormat ->m_negPercent		= p_numFormat ->m_posPercent;
	}

	if ( !l_zeroFormatSet )	
	{
		p_numFormat ->m_zeroColor		= p_numFormat ->m_posColor;
		p_numFormat ->m_zeroPrefix		= p_numFormat ->m_posPrefix;
		p_numFormat ->m_zeroSuffix		= p_numFormat ->m_posSuffix;
		p_numFormat ->m_zeroLeftDigCnt	= p_numFormat ->m_posLeftDigCnt;
		p_numFormat ->m_zeroRightDigCnt	= p_numFormat ->m_posRightDigCnt;
		p_numFormat ->m_zeroSep			= p_numFormat ->m_posSep;
	}
} // getNumFormat



pair<double, short> LocaleFunc::strToDateTime( const string& p_str )
{
	if ( !m_initialized )
		initialize();

	pair<double, short>	l_result;

	tm				l_dateTime;
	unsigned short	l_year		= 1970;
	unsigned short	l_month		= 1;
	unsigned short	l_day		= 1;
	unsigned short	l_hour		= 0;
	unsigned short	l_min		= 0;
	unsigned short	l_sec		= 0;
	double			l_secFrac	= 0;
	signed short	l_timeZone	= 0;

	unsigned short	l_val;
	unsigned short	l_val1;
	unsigned short	l_val2;
	unsigned short	l_val3;
	unsigned short	l_digits;
	char			l_nextChar;
	char			l_sep;

	string			l_str		= StrFunc::lc( p_str );
	size_t			l_len		= l_str.length();
	size_t			l_pos		= 0; 

	getNextNum( l_str, l_pos, l_len, l_val, l_digits, l_nextChar );

	if ( !l_digits )
		throw InvalidDateTimeStrException( l_str, __FILE__, __LINE__ );
	
	l_sep = l_nextChar;

	switch ( l_sep )
	{
		case '-':
		case '/':

			if ( l_digits == 4 )
			{
				l_year = l_val;

				getNextNum( l_str, l_pos, l_len, l_val, l_digits, l_nextChar );

				if (!l_digits 		
				  || l_val < 1
				  || l_val > 12
				  || l_nextChar != l_sep )
				
					throw InvalidDateTimeStrException( l_str, __FILE__, __LINE__ );

				l_month = l_val;
				
				getNextNum( l_str, l_pos, l_len, l_val, l_digits, l_nextChar );

				if (!l_digits 
				  || l_val < 1
				  || l_val > 31 )
								
					throw InvalidDateTimeStrException( l_str, __FILE__, __LINE__ );
				
				l_day = l_val;
			}

			else
			{
				l_val1 = l_val;

				getNextNum( l_str, l_pos, l_len, l_val, l_digits, l_nextChar );

				if ( !l_digits || l_nextChar != l_sep )
					throw InvalidDateTimeStrException( l_str, __FILE__, __LINE__ );

				l_val2 = l_val;

				getNextNum( l_str, l_pos, l_len, l_val, l_digits, l_nextChar );

				if ( !l_digits )
					throw InvalidDateTimeStrException( l_str, __FILE__, __LINE__ );

				l_val3 = l_val;

				if ( m_dateFormat == "dmy" )
				{
					l_day	= l_val1;
					l_month = l_val2;
					l_year	= l_val3;
				}

				else if ( m_dateFormat == "mdy" )
				{
					l_month = l_val1;
					l_day	= l_val2;
					l_year	= l_val3;
				}

				else if ( m_dateFormat == "ydm" )
				{
					l_year	= l_val1;
					l_day	= l_val2;
					l_month	= l_val3;
				}

				else // ymd
				{
					l_year	= l_val1;
					l_month = l_val2;
					l_day	= l_val3;
				}

				if ( l_year >= 0 && l_year <= 99 )
				{
					if( l_year >= m_yearOffset )
						l_year += 1900;
					else
						l_year += 2000;
				}

				if ( l_month < 1 || l_month > 12
				  || l_day < 1 || l_day > 31 )
					throw InvalidDateTimeStrException( l_str, __FILE__, __LINE__ );
			}

			if ( l_nextChar == 't'
			  || l_nextChar == ' ')
			{
				getNextNum( l_str, l_pos, l_len, l_val, l_digits, l_nextChar );

				if (!l_digits
				  || l_val > 23
				  || l_nextChar != ':')

					throw InvalidDateTimeStrException( l_str, __FILE__, __LINE__ );

				l_hour = l_val;

				getNextMinSecTimeZone( l_str, l_pos, l_len, l_min, l_sec, l_secFrac, l_timeZone );
			}

			break;

		case ':':

			l_hour = l_val;

			getNextMinSecTimeZone( l_str, l_pos, l_len, l_min, l_sec, l_secFrac, l_timeZone );

			break;

		default: 

			throw InvalidDateTimeStrException( l_str, __FILE__, __LINE__ );
	}

	l_dateTime.tm_year	= l_year - 1900;	// years since 1900
	l_dateTime.tm_mon	= l_month - 1;		// month since jan
	l_dateTime.tm_mday	= l_day;
	l_dateTime.tm_hour	= l_hour;
	l_dateTime.tm_min	= l_min;
	l_dateTime.tm_sec	= l_sec;

	TimeFunc::mkTime( &l_dateTime );
	
	l_result.first		= TimeFunc::getSecFromEpoch( &l_dateTime ) + l_secFrac;
	l_result.second		= l_timeZone;

	return l_result;
} // strToDateTime

string LocaleFunc::dateTimeToStr( double p_time, short p_timeZone, const list<DateTimeFormat*>* p_dateTimeFormat )
{
	if ( !m_initialized )
		initialize();

	return dateTimeToStrSub( 
		p_time,
		p_timeZone,
		p_dateTimeFormat,
		m_weekDayFullNames,
		m_weekDayShortNames,
		m_monthFullNames,
		m_monthShortNames );
} // dateTimeToStr

string LocaleFunc::dateTimeToStrUtf8( double p_time, short p_timeZone, const list<DateTimeFormat*>* p_dateTimeFormat )
{
	if ( !m_initialized )
		initialize();

	return dateTimeToStrSub(
		p_time,
		p_timeZone,
		p_dateTimeFormat,
		m_weekDayFullNamesUtf8,
		m_weekDayShortNamesUtf8,
		m_monthFullNamesUtf8,
		m_monthShortNamesUtf8 );
} // dateTimeToStrUtf8

void LocaleFunc::getDateTimeFormat( const string& p_format, list<DateTimeFormat*>* p_dateTimeFormat )
{
	if ( !m_initialized )
		initialize();

	DateTimeFormat*	l_dateTimeFormat	= NULL;
	DateTimeFormat*	l_hourPtr			= NULL;

	unsigned short	l_entryLen;
	char			l_entry;

	string			l_str;
	bool			l_foundMin;
	bool			l_foundAmPm;

	char			l_ch;
	size_t			l_len	= p_format.length();
	size_t			l_pos	= 0;

	p_dateTimeFormat ->clear();

	while ( l_pos < l_len )
	{
		l_ch = p_format[ l_pos ];

		if ( l_ch == '\\' )
		{
			if ( l_pos + 1 < l_len )
			{
				l_str	= l_str + p_format[ l_pos + 1 ];
				l_pos   += 2;
			}
		}

		else
		if ( tolower( l_ch ) == 'a' 
		  && ( l_pos + 4 < l_len && StrFunc::lc( p_format.substr( l_pos + 1, 4 ) ) == "m/pm"
			|| l_pos + 2 < l_len && StrFunc::lc( p_format.substr( l_pos + 1, 2 ) ) == "/p" ) )
		{
			l_entry		= l_ch;
			l_entryLen	= ( StrFunc::lc( p_format.substr( l_pos + 1, 2 ) ) == "/p" ? 3 : 5 );
		
			l_pos += l_entryLen;

			if ( l_hourPtr && !l_foundAmPm )
			{
				l_hourPtr ->m_entry = 'H';
				l_foundAmPm			= true;

				l_dateTimeFormat = new DateTimeFormat();
				l_dateTimeFormat ->m_str		= l_str;
				l_dateTimeFormat ->m_entry		= l_entry;
				l_dateTimeFormat ->m_entryLen	= l_entryLen;
				
				p_dateTimeFormat ->push_back( l_dateTimeFormat );
				l_str.clear();
			}
		}

		else
		if ( l_ch == '9' 
		  && l_pos + 7 < l_len
		  && ( p_format.substr( l_pos + 1, 7 ) == "9/99/99"
			|| p_format.substr( l_pos + 1, 7 ) == "9-99-99" ) )
		{
			l_entry      = ( p_format[ l_pos + 2 ] == '/' ? '9' : '-' );
			l_entryLen   = ( l_pos + 9 < l_len && p_format.substr( l_pos + 8, 2 ) == "99" ? 10 : 8 );

			l_pos += l_entryLen;
			
			l_dateTimeFormat = new DateTimeFormat();
			l_dateTimeFormat ->m_str		= l_str;			
			l_dateTimeFormat ->m_entry		= l_entry;
			l_dateTimeFormat ->m_entryLen	= l_entryLen;

			p_dateTimeFormat ->push_back( l_dateTimeFormat );
			l_str.clear();
		}

		else
		if ( l_ch == '[' 
		  && l_pos + 2 < l_len
		  && strchr( "hms", tolower( p_format[ l_pos + 1 ] ) )
		  && p_format[ l_pos + 2 ] == ']' )
		{
			l_entry		= tolower( p_format[ l_pos + 1 ] );
			l_entryLen	= 3;
			l_pos        += l_entryLen;

			l_dateTimeFormat = new DateTimeFormat();
			l_dateTimeFormat ->m_str		= l_str;
			l_dateTimeFormat ->m_entry		= l_entry;
			l_dateTimeFormat ->m_entryLen	= l_entryLen;

			p_dateTimeFormat ->push_back( l_dateTimeFormat );
			l_str.clear();

			if ( l_entry == 'h' )
			{
				l_hourPtr	= l_dateTimeFormat;
				l_foundMin	= false;
				l_foundAmPm = false;
			}
		}

		else
		if ( strchr( "dDmMyYwWqQhHsSzZ", l_ch ) )
		{
			l_entry		= tolower( l_ch );
			l_entryLen	= 1;
			l_pos		+= 1;

			while ( l_pos < l_len && tolower( p_format[ l_pos ] ) == l_entry )
			{
				++l_pos;
				++l_entryLen;
			}

			if ( l_entry == 'm' )
			{
				if ( l_hourPtr && !l_foundMin && l_entryLen <= 2 )
					l_foundMin = true;

				else
					l_entry = 'M';
			}

			switch ( l_entry )
			{
				case 'M':
				case 'd':

					l_entryLen = ( l_entryLen > 4 ? 4 : l_entryLen ); 
					break;
				
				case 'y':

					if (  l_entryLen >= 4 )
						l_entryLen = 4;

					else if ( l_entryLen >= 2 )
						l_entryLen = 2;

					break;

				case 'w':
				case 'h':
				case 's':
				
					l_entryLen = ( l_entryLen > 2 ? 2 : l_entryLen );
					break;

				case 'q':

					l_entryLen = ( l_entryLen > 1 ? 1 : l_entryLen );
					break;

				case 'z':

					if ( l_entryLen < 3 )
						l_entryLen = 3;

					else if ( l_entryLen < 5 )
						l_entryLen = 5;

					else if ( l_entryLen > 6 )
						l_entryLen = 6;

					break;
			}

			l_dateTimeFormat = new DateTimeFormat();
			l_dateTimeFormat ->m_str		= l_str;
			l_dateTimeFormat ->m_entry		= l_entry;
			l_dateTimeFormat ->m_entryLen	= l_entryLen;

			p_dateTimeFormat ->push_back( l_dateTimeFormat );
			l_str.clear();

			if ( l_entry == 'h' && l_entryLen <= 2 )
			{
				l_hourPtr	= l_dateTimeFormat;
				l_foundMin	= false;
				l_foundAmPm	= false;
			}

			if ( l_pos + 1 < l_len && StrFunc::lc( p_format.substr( l_pos, 2 ) ) == ".s" )
			{
				l_entry		= 'S';
				l_entryLen	= 1;
				l_pos		+= 2;

				while ( l_pos < l_len && tolower( p_format[ l_pos ] ) == 's' )
				{
					++l_entryLen;
					++l_pos;
				}

				l_dateTimeFormat = new DateTimeFormat();
				l_dateTimeFormat ->m_str		= l_str;
				l_dateTimeFormat ->m_entry		= l_entry;
				l_dateTimeFormat ->m_entryLen	= l_entryLen;

				p_dateTimeFormat ->push_back( l_dateTimeFormat );
				l_str.clear();
			}
		}

		else
		{
			l_str	+= l_ch;
			l_pos	+= 1;
		}
	}

	if ( !l_str.empty() )
	{
		l_dateTimeFormat = new DateTimeFormat();
		l_dateTimeFormat ->m_str		= l_str;
		l_dateTimeFormat ->m_entry		= char(0);
		l_dateTimeFormat ->m_entryLen	= 0;
		
		p_dateTimeFormat ->push_back( l_dateTimeFormat );
	}
} // getDateTimeFormat



string LocaleFunc::getSubStr( const string& p_str, size_t p_start, size_t p_len )
{
	if ( !m_initialized )
		initialize();

	boost::scoped_array<char>	l_buf;
	int32_t						l_bufSize;
	int32_t						l_bufLen;

	UErrorCode	l_err	= U_ZERO_ERROR;

	if ( p_str.empty() )
		return "";

	if ( m_singleByteCodePage )
		return p_str.substr( p_start, p_len );

	UnicodeString	l_ustr(
		p_str.c_str(),
		-1,
		m_cnv,
		l_err );

	if ( l_err != U_ZERO_ERROR )
		throw IcuException( "UnicodeString C'tor", l_err, __FILE__, __LINE__ );

	l_ustr.extractBetween( ( int32_t ) p_start, ( int32_t ) p_len, l_ustr );

	l_bufLen	= l_ustr.length();
	l_bufSize	= UCNV_GET_MAX_BYTES_FOR_STRING( l_bufLen, ucnv_getMaxCharSize( m_cnv ) );
	l_buf.reset( new char[ l_bufSize ] );

	l_ustr.extract( l_buf.get(), l_bufSize, m_cnv, l_err );

	if ( l_err != U_ZERO_ERROR )
		throw IcuException( "UnicodeString.extract", l_err, __FILE__, __LINE__ );

	return l_buf.get();
} // getSubStr

string LocaleFunc::getSubStrUtf8( const string& p_str, size_t p_start, size_t p_len )
{
	if ( !m_initialized )
		initialize();

	if ( p_str.empty() )
		return "";

	UnicodeString	l_ustr = UnicodeString::fromUTF8( p_str );
	string			l_str;

	l_ustr.extractBetween( ( int32_t ) p_start, ( int32_t ) p_len, l_ustr );
	l_ustr.toUTF8String( l_str );

	return l_str;
} // getSubStrUtf8



string LocaleFunc::convertToUtf8( const string& p_str )
{ 
	if ( !m_initialized )
		initialize();

	if ( p_str.empty() )
		return "";

	if ( ucnv_getType( m_cnv ) == UCNV_UTF8 )
		return p_str;

	boost::scoped_array<UChar>	l_ustr;
	boost::scoped_array<char>	l_str;

	l_ustr.reset( strToUChar( p_str.c_str(), p_str.length() ) );
	l_str.reset	( ucharToUtf8( l_ustr.get(), p_str.length() ) );

	return l_str.get();
} // convertToUtf8

string LocaleFunc::convertToUtf8( const wstring& p_wstr )
{
	if ( !m_initialized )
		initialize();

	if ( p_wstr.empty() )
		return "";

	boost::scoped_array<char>	l_str;
	boost::scoped_array<UChar>	l_buf;
	int32_t						l_bufSize;
	int32_t						l_bufLen;

	UErrorCode	l_err	= U_ZERO_ERROR;

	l_bufSize	= UCNV_GET_MAX_BYTES_FOR_STRING( p_wstr.length(), 2 );
	l_buf.reset( new UChar[ l_bufSize ] );

	u_strFromWCS(
		l_buf.get(),
		l_bufSize,
		&l_bufLen,

		p_wstr.c_str(),
		( int32_t ) p_wstr.length(),
		&l_err
	);

	if ( l_err != U_ZERO_ERROR )
		throw IcuException( "u_strFromWCS", l_err, __FILE__, __LINE__ );

	l_str.reset( ucharToUtf8( l_buf.get(), p_wstr.length() ) );
	
	return l_str.get();
} // convertToUtf8

wstring LocaleFunc::convertToUtf16( const string& p_str )
{
	if ( !m_initialized )
		initialize();

	return ( wchar_t* ) strToUChar( p_str.c_str(), p_str.length() );
} // convertToUtf16

string LocaleFunc::convertFromUtf8( const string& p_str )
{
	if ( !m_initialized )
		initialize();

	if ( p_str.empty() )
		return "";

	if ( ucnv_getType( m_cnv ) == UCNV_UTF8 )
		return p_str;

	boost::scoped_array<UChar>	l_ustr;
	boost::scoped_array<char>	l_str;

	l_ustr.reset( utf8ToUChar( p_str.c_str(), p_str.length() ) );
	l_str.reset	( ucharToStr( l_ustr.get(), p_str.length() ) );

	return l_str.get();
} // convertFromUtf8



void LocaleFunc::initialize()
{
	m_initialized = true;

	locale l_oldloc = locale::global( locale( "" ) );

	setCodePage( "UTF-8" );
	setNumFracSep();
	setNumThSep();

	setDateFormat();
	setFirstWeekDay();
	setYearOffset();

	setWeekDayNames();
	setMonthNames();

	locale::global( l_oldloc );
} // init

void LocaleFunc::setCnv()
{
	if ( !m_initialized )
		initialize();

	UErrorCode l_err = U_ZERO_ERROR;

	if ( m_cnv != NULL )
		ucnv_close( m_cnv );

	m_cnv = ucnv_open( m_codePage.c_str(), &l_err );

	if ( l_err != U_ZERO_ERROR )
		throw IcuException( "ucnv_open", l_err, __FILE__, __LINE__ );

	m_singleByteCodePage = ( ucnv_getMaxCharSize( m_cnv ) == 1 );
} // setCnv



string LocaleFunc::numToStrSingle(
	double			p_num,
	const string&	p_prefix,
	unsigned short	p_leftDigCnt,
	unsigned short	p_rightDigCnt,
	unsigned short	p_rightExtCnt, 
	const string&	p_suffix,
	bool			p_sep,
	bool			p_percent )
{
	const int		l_maxSize = 1024; // prefix(487)-30.1010suffix(486)
	char			l_buff	[ l_maxSize ];
	char			l_str	[ l_maxSize ];
	char			l_ch;

	long long		l_int;
	double			l_frac;
	double			l_val;

	unsigned short	l_rightDigTot	= p_rightDigCnt + p_rightExtCnt;
	unsigned short	l_dig;
	size_t			l_len			= 0;
	size_t			l_pos;

	if ( p_percent )		p_num *= 100.0f;
	if ( p_num < 0 )		p_num = -p_num;
	if ( p_num > 1.0e+30 )	return "?";

	l_rightDigTot = p_rightDigCnt + p_rightExtCnt;

	if ( l_rightDigTot > 0 )
	{
		double l_pow = pow( 10.0f, l_rightDigTot );
		p_num = p_num * l_pow;

		l_frac = modf( p_num, &l_val );
		if ( l_frac >= 0.5 ) l_val++;

		p_num = ( l_val + 0.01 ) / l_pow;

		l_frac	= modf( p_num, &l_val );
		l_int	= ( long long ) l_val;
	}

	else
	{
		l_frac	= modf( p_num, &l_val );
		if ( l_frac >= 0.5 ) l_val++;

		// not required because l_frac will not be used - l_frac	= 0.0f;
		l_int	= ( long long ) l_val;
	}



	if ( !p_prefix.empty() )
	{
		if ( p_prefix.length() > 487 )
		{
			strcpy( l_str + l_len, p_prefix.substr( 0, 487 ).c_str() );
			l_len += 10;
		}
		else
		{
			strcpy( l_str + l_len, p_prefix.c_str() );
			l_len += p_prefix.length();
		}
	}



	l_pos	= l_maxSize - 1;
	l_dig	= 0;
	l_buff[ l_pos ] = '\0';

	while ( l_int > 0 )
	{
		l_ch	= l_int % 10;
		l_int	= ( l_int - l_ch ) / 10;

		if ( p_sep )
		{
			if ( l_dig != 0 && l_dig % 3 == 0 )
			{
				l_pos--;
				l_buff[ l_pos ] = m_numThSep;
			}
		}

		l_pos--;
		l_dig++;
		l_buff[ l_pos ] = ( l_ch + '0' );
	}

	while ( l_dig < p_leftDigCnt )
	{
		if ( p_sep )
		{
			if ( l_dig != 0 && l_dig % 3 == 0 )
			{
				l_pos--;
				l_buff[ l_pos ] = m_numThSep;
			}
		}

		l_pos--;
		l_dig++;
		l_buff[ l_pos ] = '0';
	}

	strcpy( l_str + l_len, l_buff + l_pos );
	l_len += l_maxSize - l_pos - 1; 



	if ( l_rightDigTot > 0 )
	{
		l_pos = 0;

		while ( l_pos < p_rightDigCnt )
		{
			l_frac	= l_frac * 10;
			l_frac	= modf( l_frac, &l_val );

			l_buff[ l_pos ] = ( char ) l_val + '0';
			l_pos++;
		}

		if ( p_rightExtCnt > 0 )
		{
			size_t	l_lastPos;
			bool	l_zero = false;

			while ( l_pos < l_rightDigTot )
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
		}

		if ( l_pos > 0 )
		{
			l_buff[ l_pos ] = '\0';

			l_str[ l_len ] = m_numFracSep;
			l_len++;

			strcpy( l_str + l_len, l_buff );
			l_len += l_pos;
		}
	}



	if ( !p_suffix.empty() )
	{
		if ( p_suffix.length() > 486 )
		{
			strcpy( l_str + l_len, p_suffix.substr( 0, 486 ).c_str() );
			// not required because this is the last concat - l_len += 10;
		}

		else
		{
			strcpy( l_str + l_len, p_suffix.c_str() );
			// l_len += p_prefix.length();
		}
	}

	return l_str;
} // numToStrSingle



string LocaleFunc::dateTimeToStrSub(
	double							p_time,
	short							p_timeZone,
	const list<DateTimeFormat*>*	p_dateTimeFormat,
	const string*					p_weekDayFullNames,
	const string*					p_weekDayShortNames,
	const string*					p_monthFullNames,
	const string*					p_monthShortNames )
{
	if ( !m_initialized )
		initialize();

	list<DateTimeFormat*>::const_iterator	l_dateTimeIterator;
	DateTimeFormat*							l_dateTimeFormat;
	
	double			l_sec;
	double			l_secFrac	= modf( p_time, &l_sec );
	tm*				l_date		= TimeFunc::getTm( ( time_t )l_sec );

	string			l_result;
	string			l_str;
	char			l_entry;
	unsigned short	l_entryLen;

	string			l_yearStr;
	string			l_monthStr;
	string			l_dayStr;
	string			l_sep;

	time_t			l_hourCnt;
	time_t			l_minCnt;
	time_t			l_secCnt;

	unsigned short	i;
	unsigned short	j;

	FOR_EACH( l_dateTimeIterator, p_dateTimeFormat )
	{
		l_dateTimeFormat = *l_dateTimeIterator;

		if ( !l_dateTimeFormat ->m_str.empty() )
		l_result	+= l_dateTimeFormat ->m_str; 
		l_entry		 = l_dateTimeFormat ->m_entry;
		l_entryLen	 = l_dateTimeFormat ->m_entryLen;

		switch ( l_entry )
		{
			case 'M':

				i = l_date ->tm_mon;

				switch ( l_entryLen )
				{
					case 1: 
						l_result += StrFunc::intToStr( i + 1 ); 
						break;
					
					case 2: 
						l_str = StrFunc::intToStr( i + 1 );
						l_result += ( l_str.length() == 1 ? "0" + l_str : l_str );
						break;
					
					case 3:
						l_result += p_monthShortNames[i];
						break;

					case 4:
						l_result += p_monthFullNames[i];
						break;
				}

				break;

			case 'd':

				i = l_date ->tm_mday;

				switch ( l_entryLen )
				{
					case 1: 
						l_result += StrFunc::intToStr(i); 
						break;

					case 2: 
						l_str = StrFunc::intToStr(i);
						l_result += ( l_str.length() == 1 ? "0" + l_str : l_str );
						break;

					case 3:
						l_result += p_weekDayShortNames[ l_date ->tm_wday ];
						break;

					case 4:
						l_result += p_weekDayFullNames[ l_date ->tm_wday ];
						break;
				}

				break;

			case 'y':

				i = l_date ->tm_year + 1900;

				switch ( l_entryLen )
				{
					case 1: 
						l_result += StrFunc::intToStr(i); 
						break;

					case 2: 
						l_str = StrFunc::intToStr( i % 100 );
						l_result += ( l_str.length() == 1 ? "0" + l_str : l_str );
						break;

					case 4:
						l_result += StrFunc::intToStr(i);
						break;
				}

				break;

			case 'w':

				i = LocaleFunc::getWeekNum( l_date );

				switch ( l_entryLen )
				{
					case 1: 
						l_result += StrFunc::intToStr(i);
						break;

					case 2:
						l_str=  StrFunc::intToStr(i);
						l_result += ( l_str.length() == 1 ? "0" + l_str : l_str );
						break;
				}

				break;

			case 'q':

				l_result += StrFunc::intToStr( getQuarter( l_date ) );
				break;

			case '9': 
			case '-':

				// should it be '/' ???
				// l_sep		= ( l_entry == '9' ? '//' : '-' );
				l_sep		= ( l_entry == '9' ? '/' : '-' );

				l_str		= StrFunc::intToStr( l_date ->tm_mday );
				l_dayStr	= ( l_str.length() < 2 ? '0' + l_str : l_str );

				l_str		= StrFunc::intToStr( l_date ->tm_mon + 1 );
				l_monthStr	= ( l_str.length() < 2 ? '0' + l_str : l_str );

				switch ( l_entryLen )
				{
					case 8:
						l_str		= StrFunc::intToStr( ( l_date ->tm_year + 1900 ) % 100  );
						l_yearStr	= ( l_str.length() < 2 ? '0' + l_str : l_str );
						break;						
						
					case 10:						
						l_yearStr	= StrFunc::intToStr( l_date ->tm_year + 1900 );
						break;
				}

				if ( m_dateFormat == "dmy" )
					l_result += l_dayStr + l_sep + l_monthStr + l_sep + l_yearStr;

				else if ( m_dateFormat == "mdy" )
					l_result += l_monthStr + l_sep + l_dayStr + l_sep + l_yearStr;

				else if ( m_dateFormat == "ymd" )
					l_result += l_yearStr + l_sep + l_monthStr + l_sep + l_dayStr;

				else // ydm
					l_result += l_yearStr + l_sep + l_dayStr + l_sep + l_monthStr;

				break;

			case 'h':

				switch ( l_entryLen )
				{
					case 1:
						l_result	+= StrFunc::intToStr( l_date ->tm_hour );
						break;						

					case 2:	
						l_str		= StrFunc::intToStr( l_date ->tm_hour );
						l_result	+= ( l_str.length() < 2 ? '0' + l_str : l_str );  
						break;

					case 3:

						l_secCnt	= TimeFunc::getSecFromEpoch( l_date );
						l_hourCnt	= ( l_secCnt - l_secCnt % 3600 ) / 3600;

						if ( l_hourCnt == 0 )
							l_result += "0";

						else
						l_result += numToStrSingle( 
							( double ) l_hourCnt,
							( l_hourCnt < 0 ? "-" : "" ),	// prefix
							0,								// leftDigCnt
							0,								// rightDigCnt
							0,								// rightExtCnt
							"",								// suffix,
							true,							// separator	
							false );						// percent 

						break;
				}

				break;

			case 'H':

				i = ( l_date ->tm_hour > 12 ? l_date ->tm_hour - 12 : l_date ->tm_hour );
				
				switch ( l_entryLen )
				{
					case 1:
						l_result += StrFunc::intToStr(i);
						break;

					case 2:
						l_str		= StrFunc::intToStr(i);
						l_result	+= ( l_str.length() < 2 ? '0' + l_str : l_str );
				}
			
				break;

			case 'm':

				switch ( l_entryLen )
				{
					case 1:

						l_result += StrFunc::intToStr( l_date ->tm_min );
						break;

					case 2:

						l_str		= StrFunc::intToStr( l_date ->tm_min );
						l_result	+= ( l_str.length() < 2 ? '0' + l_str : l_str );
						break;

					case 3:

						l_secCnt = TimeFunc::getSecFromEpoch( l_date );
						l_minCnt = ( l_secCnt - l_secCnt % 60 ) / 60;

						if ( l_minCnt == 0 )
							l_result += "0";

						else
						l_result += numToStrSingle( 
							( double ) l_minCnt,
							( l_minCnt < 0 ? "-" : "" ),	// prefix
							0,								// leftDigCnt
							0,								// rightDigCnt
							0,								// rightExtCnt
							"",								// suffix,
							true,							// separator	
							false );						// percent 

						break;
				}

				break;

			case 's':

				switch ( l_entryLen )
				{
					case 1:

						l_result += StrFunc::intToStr( l_date ->tm_sec );
						break;

					case 2:

						l_str		= StrFunc::intToStr( l_date ->tm_sec );
						l_result	+= ( l_str.length() < 2 ? '0' + l_str : l_str );
						break;

					case 3:

						l_secCnt = TimeFunc::getSecFromEpoch ( l_date );
					
						if ( l_secCnt == 0 )
							l_result += "0";

						else
						l_result += numToStrSingle( 
							( double ) l_secCnt,
							( l_secCnt < 0 ? "-" : "" ),	// prefix
							0,								// leftDigCnt
							0,								// rightDigCnt
							0,								// rightExtCnt
							"",								// suffix,
							true,							// separator	
							false );						// percent 

						break;
				}

				break;

			case 'S':
				
				l_str = numToStrSingle( 
					l_secFrac,
					"",			// prefix
					0,			// leftDigCnt
					l_entryLen,	// rightDigCnt
					0,			// rightExtCnt
					"",			// suffix,
					true,		// separator	
					false );	// percent
		
				l_str		= l_str.substr( l_str.find( m_numFracSep ) );
				l_result	+= l_str;

				break;

			case 'a':

				switch ( l_entryLen )
				{
					case 3: 
						l_result += ( l_date ->tm_hour < 12 ? "a" : "p" );
						break;

					case 5:
						l_result += ( l_date ->tm_hour < 12 ? "am" : "pm" );
						break;
				}

				break;

			case 'A':

				switch ( l_entryLen )
				{
					case 3: 
						l_result += ( l_date ->tm_hour < 12 ? "A" : "P" );
						break;

					case 5:
						l_result += ( l_date ->tm_hour < 12 ? "AM" : "PM" );
						break;
				}

				break;

			case 'z':

				switch ( l_entryLen )
				{
					case 3:
						l_str		= StrFunc::intToStr( ( abs( p_timeZone ) - abs( p_timeZone ) % 60 ) % 24 );
						l_str		= ( l_str.length() < 2 ? '0' + l_str : l_str );

						l_result	+= ( p_timeZone < 0 ? "-" : "+" ) + l_str;

						break;

					case 5: 
					case 6: 

						i =	abs( p_timeZone ) % 60;
						j = ( abs( p_timeZone ) - i ) % 24;

						l_str		= StrFunc::intToStr( j );
						l_str		= ( l_str.length() < 2 ? '0' + l_str : l_str );

						l_result	+= ( p_timeZone < 0 ? "-" : "+" ) + l_str;

						l_str		= StrFunc::intToStr( i );
						l_str		= ( l_str.length() < 2 ? '0' + l_str : l_str );

						l_result	+= ( l_entryLen == 6 ? ":" : "") + l_str;

						break;
				}

				break;
		}
	}

	return l_result;
} // dateTimeToStrSub



// used in strToDateTime
void LocaleFunc::getNextMinSecTimeZone( const string& p_str, size_t& p_pos, size_t p_len, unsigned short& p_min, unsigned short& p_sec, double& p_secFrac, short& p_timeZone )
{
	unsigned short	l_digits;
	unsigned short	l_val;
	char			l_nextChar;
	bool			l_neg;

	p_min		= 0;
	p_sec		= 0;
	p_secFrac	= 0;
	p_timeZone	= 0;

	getNextNum( p_str, p_pos, p_len, l_val, l_digits, l_nextChar );

	if ( !l_digits || l_nextChar != ':' )
		throw InvalidDateTimeStrException ( p_str, __FILE__, __LINE__ );

	p_min = l_val;

	getNextNum( p_str, p_pos, p_len, l_val, l_digits, l_nextChar );

	if ( !l_digits )
		return;

	p_sec = l_val;

	if ( l_nextChar == '.' )
	{
		getNextNum( p_str, p_pos, p_len, l_val, l_digits, l_nextChar );

		if ( !l_digits )
			return;

		p_secFrac += ( double )l_val / pow( 10.0f, l_digits );
	}

	if ( l_nextChar == '+' || l_nextChar == '-' )
	{
		l_neg = ( l_nextChar == '-' );

		getNextNum( p_str, p_pos, p_len,l_val, l_digits, l_nextChar );

		if ( !l_digits || l_val > 23 )
			return;

		p_timeZone = l_val * 60;

		if ( l_nextChar == ':' )
		{
			getNextNum( p_str, p_pos, p_len,l_val, l_digits, l_nextChar );
			
			if ( !l_digits || l_val > 59 )
				return;
			
			p_timeZone += l_val;
		}

		if ( l_neg )
			p_timeZone = -p_timeZone;
	}
} // getNextMinSecTimeZone

void LocaleFunc::getNextNum( const string& p_str, size_t& p_pos, size_t p_len, unsigned short& p_val, unsigned short& p_digits, char& p_nextChar )
{
	char l_ch;

	p_val		= 0;
	p_digits	= 0;

	while ( p_pos < p_len )
	{
		l_ch = p_str[ p_pos ];
		p_pos++;

		if ( !IS_DIGIT( l_ch ) )
		{
			p_nextChar = l_ch;
			break;
		}

		p_val = p_val * 10 + ( l_ch - 48 );
		p_digits++;
	}
} // getNextNum



UChar* LocaleFunc::utf8ToUChar( const char* p_str, size_t p_len )
{
	if ( !m_initialized )
		initialize();

	int32_t		l_bufLen;
	int32_t		l_bufSize	= UCNV_GET_MAX_BYTES_FOR_STRING( p_len, 2 );
	UChar*		l_buf		= new UChar[ l_bufSize ];

	UErrorCode	l_err		= U_ZERO_ERROR;
	
	try
	{
		u_strFromUTF8(
			l_buf,
			l_bufSize,
			&l_bufLen,

			p_str,
			( int32_t ) p_len,
			&l_err );
			
		if ( l_err != U_ZERO_ERROR )
			throw IcuException( "u_strFromUTF8", l_err, __FILE__, __LINE__ );
	}

	catch ( ... )
	{
		if ( l_buf )
			delete[] l_buf;

		throw;
	}

	return l_buf;
} // utf8ToUChar

UChar* LocaleFunc::strToUChar( const char* p_str, size_t p_len )
{
	if ( !m_initialized )
		initialize();

	UChar*		l_buf	= NULL;
	int32_t		l_bufLen;
	int32_t		l_bufSize;

	UErrorCode	l_err	= U_ZERO_ERROR;

	try
	{
		UnicodeString l_ustr(	
			p_str,
			-1,
			m_cnv,
			l_err );

		if ( l_err != U_ZERO_ERROR )
			throw IcuException( "UnicodeString C'tor", l_err, __FILE__, __LINE__ );

		l_bufLen	= l_ustr.length();
		l_bufSize	= UCNV_GET_MAX_BYTES_FOR_STRING( l_bufLen, 2 );
		l_buf		= new UChar[ l_bufSize ];

		l_ustr.extract( l_buf, l_bufSize, l_err );

		if ( l_err != U_ZERO_ERROR )
			throw IcuException( "UnicodeString.extract", l_err, __FILE__, __LINE__ );
	}

	catch ( ... )
	{
		if ( l_buf )
			delete[] l_buf;

		throw;
	}

	return l_buf;
} // strToUChar

char* LocaleFunc::ucharToUtf8( const UChar* p_ustr, size_t p_len )
{
	if ( !m_initialized )
		initialize();

	int32_t		l_bufLen;
	int32_t		l_bufSize	= UCNV_GET_MAX_BYTES_FOR_STRING( p_len, 4 );
	char*		l_buf		= new char[ l_bufSize ];

	UErrorCode	l_err		= U_ZERO_ERROR;
	
	try
	{
		u_strToUTF8(
			l_buf,
			l_bufSize,
			&l_bufLen,

			p_ustr,
			( int32_t ) p_len,
			&l_err );

		if ( l_err != U_ZERO_ERROR )
			throw IcuException( "u_strToUTF8", l_err, __FILE__, __LINE__ );
	}

	catch ( ... )
	{
		if ( l_buf )
			delete[] l_buf;

		throw;
	}

	return l_buf;
} // ucharToUtf8

char* LocaleFunc::ucharToStr( const UChar* p_ustr, size_t p_len )
{
	if ( !m_initialized )
		initialize();

	char*		l_buf	= NULL;
	int32_t		l_bufSize;
	int32_t		l_bufLen;

	UErrorCode	l_err	= U_ZERO_ERROR;

	try
	{
		UnicodeString	l_ustr( p_ustr );

		l_bufSize	= UCNV_GET_MAX_BYTES_FOR_STRING( p_len, ucnv_getMaxCharSize( m_cnv ) );
		l_buf		= new char[ l_bufSize ];
		l_bufLen	= l_ustr.extract( l_buf, l_bufSize, m_cnv, l_err );

		if ( l_err != U_ZERO_ERROR )
			throw IcuException( "UnicodeString.extract", l_err, __FILE__, __LINE__ );
	}

	catch ( ... )
	{
		if ( l_buf )
			delete[] l_buf;

		throw;
	}

	return l_buf;
} // ucharToStr
