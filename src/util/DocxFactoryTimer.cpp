
#include "DocxFactory/util/DocxFactoryTimer.h"

using namespace DocxFactory;
using namespace std;



DocxFactoryTimer::DocxFactoryTimer()
{
	m_start = clock();
} // c'tor

DocxFactoryTimer::~DocxFactoryTimer()
{

} // d'tor

DocxFactoryTimer& DocxFactoryTimer::getInstance()
{
	static DocxFactoryTimer s_instance;
	return s_instance;
} // getInstance

void DocxFactoryTimer::showTime( const string& p_str )
{
	double l_diff;

	l_diff = ( clock() - m_start ) / ( double ) CLOCKS_PER_SEC;
	cout << p_str << ": " << l_diff << endl;
} // showTime
