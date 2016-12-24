
#include "DocxFactory/time/TimeFunc.h"

#include <cstdlib>

using namespace DocxFactory;
using namespace std;



tm* TimeFunc::getTm( time_t p_time )
{
	const int l_MonthDays[12]			= { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	const int l_MonthDaysLeapYear[12]	= { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	int		l_year;
	int		l_month;
	int		l_monthDay;
	int		l_hours;
	int		l_min;
	int		l_sec;
	int		l_yearDay;
	int		l_weekDay;

	tm*		l_date;
	bool	l_leapYear;
	int		i;

	if ( p_time >= 0 )
	{
		l_weekDay = ( p_time - p_time % 86400 ) / 86400 % 7 + 5; 

		if ( l_weekDay > 7 )
			l_weekDay = l_weekDay - 7;

		l_year = 1970;

		while ( true )
		{
			l_leapYear = isLeapYear( l_year );

			if ( l_leapYear )
				i = 31622400; // 366 * 86400

			else
				i = 31536000; // 365 * 86400

			if ( p_time - i < 0 )
				break;

			p_time -= i;
			l_year++;
		}

		l_yearDay = int( ( p_time - p_time % 86400 ) / 86400 + 1 );



		l_month = 0;

		while ( true )
		{
			if ( l_leapYear )
				i = l_MonthDaysLeapYear[ l_month ] * 86400;

			else
				i = l_MonthDays[ l_month ] * 86400;

			if ( p_time - i < 0 )
				break;

			l_month++;
			p_time -= i;
		}



		l_monthDay	= int( ( p_time - p_time % 86400 ) / 86400 + 1 );
		p_time		= p_time % 86400;

		l_hours		= int( ( p_time - p_time % 3600 ) / 3600 );
		p_time		= p_time % 3600;

		l_min		= int( ( p_time - p_time % 60 ) / 60 );
		p_time		= p_time % 60;

		l_sec		= int( p_time );
	}

	else
	{
		p_time = abs( p_time ) - 1;

		l_weekDay = 4 - ( p_time - p_time % 86400 ) / 86400 % 7;		

		if ( l_weekDay <= 0 )
			l_weekDay = l_weekDay + 7;

		l_year = 1969;

		while ( true )
		{
			l_leapYear = isLeapYear( l_year );

			if ( l_leapYear )
				i = 31622400; // 366 * 86400

			else 
				i = 31536000; // 365 * 86400

			if ( p_time - i < 0 )
				break;

			l_year--;
			p_time -= i;
		}

		l_yearDay = int( ( l_leapYear ? 366 : 365 ) - ( p_time - p_time % 86400 ) / 86400 );



		l_month	= 11;

		while ( true )
		{
			if ( l_leapYear )
				i = l_MonthDaysLeapYear [ l_month ] * 86400;

			else
				i = l_MonthDays[ l_month ] * 86400;

			if ( p_time - i < 0 )
				break;

			l_month--;
			p_time -= i;
		}

		l_monthDay	= int( ( l_leapYear ? l_MonthDaysLeapYear[ l_month ] : l_MonthDays[ l_month ] ) - ( p_time - p_time % 86400 ) / 86400 ); 
		p_time		= p_time % 86400;

		l_hours		= int( 23 - ( p_time - p_time % 3600 ) / 3600 );
		p_time		= p_time % 3600;

		l_min		= int( 59 - ( p_time - p_time % 60 ) / 60 );
		p_time		= p_time % 60;

		l_sec		= int( 59 - p_time );
	}

	l_date = new tm;
	l_date ->tm_year	= l_year - 1900;	// years since 1900	
	l_date ->tm_mon		= l_month;			// 0-11
	l_date ->tm_mday	= l_monthDay;		// 1-31
	l_date ->tm_hour	= l_hours;			// 0-59
	l_date ->tm_min		= l_min;			// 0-59	
	l_date ->tm_sec		= l_sec;			// 0-59
	l_date ->tm_yday	= l_yearDay - 1;	// 0-365
	l_date ->tm_wday	= l_weekDay - 1;	// 0-6

	return l_date;
} // getTm

void TimeFunc::mkTime( tm* p_date )
{
	const int l_monthDays[12]			= { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	const int l_monthDaysLeapYear[12]	= { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	
	int		l_year		= p_date ->tm_year + 1900;
	int		l_month		= p_date ->tm_mon;
	int		l_monthDay	= p_date ->tm_mday;
	int		l_hours		= p_date ->tm_hour;
	int		l_min		= p_date ->tm_min;
	int		l_sec		= p_date ->tm_sec;
	int		l_yearDay;
	int		l_weekDay;

	bool	l_leapYear;
	int		i;
	int		j;

	if ( l_sec > 59 )
	{
		l_min	= l_min + ( l_sec - l_sec % 60 ) / 60;
		l_sec	= l_sec % 60;
	}

	else if ( l_sec < 0 )
	{
		l_sec	= abs( l_sec ) - 1;
		
		l_min	= l_min - ( l_sec - l_sec % 60 ) / 60 - 1;
		l_sec	= 59 - l_sec % 60;
	}

	if ( l_min > 59 )
	{
		l_hours	= l_hours + ( l_min - l_min % 60 ) / 60;
		l_min	= l_min % 60;
	}

	else if ( l_min < 0 )
	{
		l_min   = abs( l_min ) - 1;

		l_hours	= l_hours - ( l_min - l_min % 60 ) / 60 - 1;
		l_min	= 59 - l_min % 60;
	}

	if ( l_hours > 23 ) 
	{
		l_monthDay  = l_monthDay + ( l_hours - l_hours % 24 ) / 24;
		l_hours		= l_hours % 24;
	}

	else if ( l_hours < 0 )
	{
		l_hours  = abs( l_hours ) - 1;

		l_monthDay  = l_monthDay - ( l_hours - l_hours % 24 ) / 24 - 1;
		l_hours		= 23 - l_hours % 24;
	}

	if ( l_month > 11 )
	{
		l_year = l_year + ( l_month - l_month % 12 ) / 12;
		l_month = l_month % 12;
	}

	else if ( l_month < 0 )
	{
		l_month = abs( l_month ) - 1;

		l_year  = l_year - ( l_month - l_month % 12 ) / 12 - 1;
		l_month = 11 - l_month % 12;
	}

	l_leapYear = isLeapYear( l_year );



	if ( l_monthDay > ( l_leapYear ? l_monthDaysLeapYear[ l_month ] : l_monthDays[ l_month ] ) )
	{
		while ( true )
		{
			l_monthDay  = l_monthDay - ( l_leapYear ? l_monthDaysLeapYear[ l_month ] : l_monthDays[ l_month ] );
			l_month		= l_month + 1;

			if ( l_month > 11 )
			{
				l_year		= l_year + 1;
				l_leapYear	= isLeapYear( l_year );
				l_month		= 0;
			}

			if ( !( l_monthDay > ( l_leapYear ? l_monthDaysLeapYear[ l_month ] : l_monthDays[ l_month ] ) ) )
				break;
		}
	}

	else if ( l_monthDay < 0 ) 
	{
		l_monthDay = abs( l_monthDay ) - 1;

		while ( true )
		{
			l_month = l_month - 1;

			if ( l_month < 0 )
			{
				l_year		= l_year - 1; 
				l_leapYear	= isLeapYear( l_year );
				l_month		= 11;
			}

			if ( l_monthDay < ( l_leapYear ? l_monthDaysLeapYear[ l_month ] : l_monthDays[ l_month ] ) )
				break;

			l_monthDay = l_monthDay - ( l_leapYear ? l_monthDaysLeapYear[ l_month ] : l_monthDays[ l_month ] );
		}
		l_monthDay = ( l_leapYear ? l_monthDaysLeapYear[ l_month ] : l_monthDays[ l_month ] ) - l_monthDay;
	}



	if ( l_month > 11 )
	{
		l_year  = l_year + ( l_month - l_month % 12 ) / 12;
		l_month = l_month % 12;
	}

	else if ( l_month < 0 )
	{
		l_month = abs( l_month ) - 1;

		l_year  = l_year - ( l_month - l_month % 12 ) / 12 - 1;
		l_month = 11 - l_month % 12;
	}

	l_yearDay = 0;

	for ( i = 0; i < l_month; i++ )

		l_yearDay += ( l_leapYear ? l_monthDaysLeapYear[i] : l_monthDays[i] );

	l_yearDay += l_monthDay;



	if ( l_year >= 1970 )
	{
		j = 0;

		for ( i = 1970; i < l_year; i++ )
		{
			l_leapYear = isLeapYear(i);

			j = j + ( l_leapYear ? 366 : 365 );
		}

		j = j + l_yearDay;

		l_weekDay = j % 7 + 5; 

		if ( l_weekDay > 7 )
			l_weekDay = l_weekDay - 7;
	}

	else
	{
		j = 0;

		for ( i = 1969; i > l_year; i-- )
		{
			l_leapYear = isLeapYear(i);

			j = j + ( l_leapYear ? 366 : 365 );
		}

		l_leapYear = isLeapYear( l_year );

		j = j + ( l_leapYear ? 366 : 365 ) - ( l_yearDay + 1 );

		l_weekDay = 4 - j % 7;

		if ( l_weekDay <= 0 )
			l_weekDay = l_weekDay + 7;
	}

	p_date ->tm_year	= l_year - 1900;
	p_date ->tm_mon		= l_month;
	p_date ->tm_mday	= l_monthDay;
	p_date ->tm_hour	= l_hours;
	p_date ->tm_min		= l_min;
	p_date ->tm_sec		= l_sec;
	p_date ->tm_yday	= l_yearDay - 1;
	p_date ->tm_wday	= l_weekDay - 1;
} // mkTime

time_t TimeFunc::getSecFromEpoch( tm* p_date )
{
	time_t	l_retVal;
	int		l_year	= p_date ->tm_year + 1900;
	bool	l_leapYear;
	int		i;
	int		j;

	if ( l_year >= 1970 )
	{
		j = 0;

		for ( i = 1970; i < l_year; i++ )
		{
			l_leapYear = isLeapYear(i);

			j = j + ( l_leapYear ? 366 : 365 );
		}

		j = j + p_date ->tm_yday;

		l_retVal = j * 86400 + p_date ->tm_hour * 3600 + p_date ->tm_min * 60 + p_date ->tm_sec; 
	}

	else
	{
		j = 0;

		for ( i = 1969; i > l_year; i-- )
		{
			l_leapYear = isLeapYear(i);

			j = j + ( l_leapYear ? 366 : 365 );
		}

		l_leapYear = isLeapYear( l_year );

		j = j + ( l_leapYear ? 366 : 365 ) - ( p_date ->tm_yday + 1 );

		l_retVal = -1 * ( j * 86400 + ( 23 - p_date ->tm_hour ) * 3600 + ( 59 - p_date ->tm_min ) * 60 + ( 60 - p_date ->tm_sec ) );
	}

	return l_retVal;
} // getSecFromEpoch

bool TimeFunc::isLeapYear( short p_year )
{
	return ( p_year % 400 == 0 || p_year % 100 != 0 && p_year % 4 == 0 );
} // isLeapYear
