
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteDateTimeField.h"

using namespace DocxFactory;
using namespace std;



DocxMergerPasteDateTimeField::DocxMergerPasteDateTimeField( DocxMergerField* p_field, double p_time, short p_timeZone ) : DocxMergerPasteField( p_field )
{
	m_time		= p_time;
	m_timeZone	= p_timeZone;
} // c'tor

DocxMergerPasteDateTimeField::~DocxMergerPasteDateTimeField()
{

} // d'tor



double DocxMergerPasteDateTimeField::getTime() const
{
	return m_time;
} // getTime

signed short DocxMergerPasteDateTimeField::getTimeZone() const
{
	return m_timeZone;
} // getTimeZone
