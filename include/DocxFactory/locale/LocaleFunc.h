
#ifndef __DOCXFACTORY_LOCALE_FUNC_H__
#define __DOCXFACTORY_LOCALE_FUNC_H__

#include "DocxFactory/util/DocxFactoryDefs.h"

#include "unicode/ucnv.h"

#include <list>
#include <string>



struct UConverter;

namespace DocxFactory
{
	using namespace std;

	struct NumFormat;
	struct DateTimeFormat;

	class XmlFunc;
	class LocaleFunc
	{
	public:
		enum FirstWeekDay
		{
			SUNDAY = 1,
			MONDAY
		};

		static void setCodePage();
		static void	setCodePage( const string& p_codePage );

		static void	setNumFracSep( char p_frac = 0 );
		static void	setNumThSep( char p_th = 0 );

		static void	setDateFormat( const string& p_format = "" );
		static void	setYearOffset( unsigned short p_year = 0 );
		static void setFirstWeekDay();
		static void	setFirstWeekDay( FirstWeekDay p_weekday );

		static void setWeekDayNames();
		static void setWeekDayNames(
			const string& p_weekDay1,
			const string& p_weekDay2,
			const string& p_weekDay3,
			const string& p_weekDay4,
			const string& p_weekDay5,
			const string& p_weekDay6,
			const string& p_weekDay7 );

		static void setWeekDayNames(
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
			const string& p_shortName7 );

		static void setWeekDayNamesUtf8();
		static void setWeekDayNamesUtf8(
			const string& p_weekDay1,
			const string& p_weekDay2,
			const string& p_weekDay3,
			const string& p_weekDay4,
			const string& p_weekDay5,
			const string& p_weekDay6,
			const string& p_weekDay7 );

		static void setWeekDayNamesUtf8(
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
			const string& p_shortName7 );

		static void setMonthNames();
		static void setMonthNames(
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
			const string& p_month12 );

		static void setMonthNames(
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
			const string& p_shortName12 );

		static void setMonthNamesUtf8();
		static void setMonthNamesUtf8(
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
			const string& p_month12 );

		static void setMonthNamesUtf8(
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
			const string& p_shortName12 );

		static string			getCodePage();
		static char				getNumFracSep();
		static char				getNumThSep();

		static string			getDateFormat();
		static unsigned short	getYearOffset();
		static unsigned short	getFirstWeekDay();

		static string*			getWeekDayFullNames();
		static string*			getWeekDayShortNames();
		static string*			getWeekDayFullNamesUtf8();
		static string*			getWeekDayShortNamesUtf8();

		static string*			getMonthFullNames();
		static string*			getMonthShortNames();
		static string*			getMonthFullNamesUtf8();
		static string*			getMonthShortNamesUtf8();

		static unsigned short	getWeekNum( tm* p_date );		
		static unsigned short	getQuarter( tm* p_date );

		static double				strToNum			( const string& p_str );
		static string				numToStr			( double p_num,	const NumFormat* p_format );
		static string				numToStr			( double p_num,	unsigned short p_rightDigCnt = 0, unsigned short p_rightExtCnt = 3, bool p_sep = false );
		static void					getNumFormat		( const string& p_format, NumFormat* p_numFormat );

		static pair<double, short>	strToDateTime		( const string& p_str ); 
		static string				dateTimeToStr		( double p_time, short p_timeZone, const list<DateTimeFormat*>* p_dateTimeFormat );
		static string				dateTimeToStrUtf8	( double p_time, short p_timeZone, const list<DateTimeFormat*>* p_dateTimeFormat );	
		static void					getDateTimeFormat	( const string&	p_format, list<DateTimeFormat*>* p_dateTimeFormat );

		static string	getSubStr		( const string& p_str, size_t p_start, size_t p_len );
		static string	getSubStrUtf8	( const string& p_str, size_t p_start, size_t p_len );

		static string	convertToUtf8	( const string& p_str );
		static string	convertToUtf8	( const wstring& p_wstr ); // it is assumed that wstring is utf16 or utf32
		static wstring	convertToUtf16	( const string& p_str );
		static string	convertFromUtf8	( const string& p_str );

		// i made XmlFunc a friend of LocaleFunc
		// because it needs access to low-level conversion functions
		// that i did not want to make public.
		friend class XmlFunc;

	protected:

	private:
		LocaleFunc();
		LocaleFunc( const LocaleFunc& p_other );
		LocaleFunc& operator = ( const LocaleFunc& p_other );

		static void	initialize();
		static void	setCnv();

		static string numToStrSingle(
			double			p_num,
			const string&	p_prefix,
			unsigned short	p_leftDigCnt,
			unsigned short	p_rightDigCnt,
			unsigned short	p_rightExtCnt, 
			const string&	p_suffix,
			bool			p_sep,
			bool			p_percent );

		static string dateTimeToStrSub(
			double							p_time,
			short							p_timeZone,
			const list<DateTimeFormat*>*	p_dateTimeFormat,
			const string*					p_weekDayFullNames,
			const string*					p_weekDayShortNames,
			const string*					p_monthFullNames,
			const string*					p_monthShortNames );

		// used in strToDateTime
		static void getNextMinSecTimeZone	( const string& p_str, size_t& p_pos, size_t p_len, unsigned short& p_min, unsigned short& p_sec, double& p_secFrac, short& p_timeZone );
		static void getNextNum				( const string& p_str, size_t& p_pos, size_t p_len, unsigned short& p_val, unsigned short& p_digits, char& p_nextChar );
		
		// low-level conversion functions also used by other libraries like XmlFunc
		static UChar*	utf8ToUChar	( const char*	p_str,	size_t p_len );
		static UChar*	strToUChar	( const char*	p_str,	size_t p_len );
		static char*	ucharToUtf8	( const UChar*	p_ustr, size_t p_len );
		static char*	ucharToStr	( const UChar*	p_ustr,	size_t p_len );

		static bool				m_initialized;
		static string			m_codePage;
		static bool				m_singleByteCodePage;
		static UConverter*		m_cnv;

		static char				m_numFracSep;
		static char				m_numThSep;

		static string			m_dateFormat;
		static unsigned short	m_yearOffset;
		static FirstWeekDay		m_firstWeekDay;

		static string			m_weekDayFullNames[7];
		static string			m_weekDayFullNamesUtf8[7];
		static string			m_weekDayShortNames[7];
		static string			m_weekDayShortNamesUtf8[7];

		static string			m_monthFullNames[12];
		static string			m_monthFullNamesUtf8[12];
		static string			m_monthShortNames[12];
		static string			m_monthShortNamesUtf8[12];
	};
};

#endif
