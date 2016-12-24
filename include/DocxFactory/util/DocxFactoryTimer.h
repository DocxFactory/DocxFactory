
#ifndef __DOCXFACTORY_TIMER_H__
#define __DOCXFACTORY_TIMER_H__

#include <ctime>
#include <iostream>
#include <string>



namespace DocxFactory
{
	using namespace std;

	class DocxFactoryTimer
	{
	public:
		static DocxFactoryTimer& getInstance();

		virtual ~DocxFactoryTimer();

		void showTime( const string& p_str );

	protected:

	private:
		DocxFactoryTimer();
		DocxFactoryTimer( const DocxFactoryTimer& p_other );
		DocxFactoryTimer operator = ( const DocxFactoryTimer& p_other );

		clock_t m_start;
	};
};

#endif
