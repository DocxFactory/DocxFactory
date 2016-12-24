
#ifndef __DOCXFACTORY_TIME_FUNC_H__
#define __DOCXFACTORY_TIME_FUNC_H__

#include "DocxFactory/util/DocxFactoryDefs.h"

#include <ctime>



namespace DocxFactory
{
	using namespace std;

	class TimeFunc
	{
	public:
		static tm*		getTm			( time_t p_time );
		static void		mkTime			( tm* p_date );
		static time_t	getSecFromEpoch	( tm* p_date );
	protected:

	private:
		TimeFunc();
		TimeFunc( const TimeFunc& p_other );
		TimeFunc& operator = ( const TimeFunc& p_other );
		
		static bool isLeapYear( short p_year );

	};
};

#endif
