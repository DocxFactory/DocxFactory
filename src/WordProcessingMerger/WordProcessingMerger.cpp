
#include "WordProcessingMerger.h"

#include "dfw.h"

#include <stdexcept>

using namespace DocxFactory;
using namespace std;



WordProcessingMerger& WordProcessingMerger::getInstance()
{
	static WordProcessingMerger s_instance;
	return s_instance;
} // getInstance

WordProcessingMerger::WordProcessingMerger()
{

} // c'tor

WordProcessingMerger::~WordProcessingMerger()
{

} // d'tor



void WordProcessingMerger::load( const string& p_fileName )
{
	dfw_load( p_fileName.c_str() );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // load

void WordProcessingMerger::save( const string& p_fileName )
{
	dfw_save( p_fileName.c_str() );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // save

void WordProcessingMerger::print( const string& p_printerName, unsigned short p_copyCnt )
{
	dfw_print( p_printerName.c_str(), p_copyCnt );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // print

void WordProcessingMerger::close()
{
	dfw_close();

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // close



void WordProcessingMerger::merge( const string& p_data )
{
	dfw_merge( p_data.c_str() );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // merge

void WordProcessingMerger::setChartValue( const string& p_itemName, const string& p_fieldName, const string& p_series, const string& p_category, double p_value )
{
	dfw_setChartValueByStrStr( p_itemName.c_str(), p_fieldName.c_str(), p_series.c_str(), p_category.c_str(), p_value );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // setChartValue

void WordProcessingMerger::setChartValue( const string& p_itemName, const string& p_fieldName, const string& p_series, double p_category, double p_value )
{
	dfw_setChartValueByStrDouble( p_itemName.c_str(), p_fieldName.c_str(), p_series.c_str(), p_category, p_value );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // setChartValue

void WordProcessingMerger::setChartValue( const string& p_itemName, const string& p_fieldName, double p_series, double p_category, double p_value )
{
	dfw_setChartValueByDoubleDouble( p_itemName.c_str(), p_fieldName.c_str(), p_series, p_category, p_value );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // setChartValue

void WordProcessingMerger::setClipboardValue( const string& p_itemName, const string& p_fieldName, const string& p_value )
{
	dfw_setClipboardValueByStr( p_itemName.c_str(), p_fieldName.c_str(), p_value.c_str() );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // setClipboardValue

void WordProcessingMerger::setClipboardValue( const string& p_itemName, const string& p_fieldName, double p_value )
{
	dfw_setClipboardValueByDouble( p_itemName.c_str(), p_fieldName.c_str(), p_value );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // setClipboardValue

void WordProcessingMerger::paste( const string& p_itemName /* = "" */ )
{
	dfw_paste( p_itemName.c_str() );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // paste



void WordProcessingMerger::setUpdateTocMethod( unsigned char p_method )
{
	dfw_setUpdateTocMethod( p_method );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // setUpdateTocMethod

unsigned char WordProcessingMerger::getUpdateTocMethod() const
{
	return dfw_getUpdateTocMethod();
} // getUpdateTocMethod



string WordProcessingMerger::getFields() const
{
	return dfw_getFields();
} // getFields

string WordProcessingMerger::getItems() const
{
	return dfw_getItems();
} // getItems

string WordProcessingMerger::getItemParent( const string& p_itemName ) const
{
	return dfw_getItemParent( p_itemName.c_str() );
} // getItemParent

string WordProcessingMerger::getItemFields( const string& p_itemName ) const
{
	return dfw_getItemFields( p_itemName.c_str() );
} // getItemFields



// locale functions

void WordProcessingMerger::setCodePage()
{
	dfw_setCodePage( "" );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // setCodePage

void WordProcessingMerger::setCodePage( const string& p_codePage )
{
	dfw_setCodePage( p_codePage.c_str() );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // setCodePage

void WordProcessingMerger::setNumFracSep( char p_frac )
{
	dfw_setNumFracSep( p_frac );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // setNumFracSep

void WordProcessingMerger::setNumThSep( char p_th )
{
	dfw_setNumThSep( p_th );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // setNumThSep

void WordProcessingMerger::setDateFormat( const string& p_format )
{
	dfw_setDateFormat( p_format.c_str() );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // setDateFormat

void WordProcessingMerger::setYearOffset( unsigned short p_year )
{
	dfw_setYearOffset( p_year );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // setYearOffset

void WordProcessingMerger::setFirstWeekDay()
{
	dfw_setFirstWeekDay(0);

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // setFirstWeekDay

void WordProcessingMerger::setFirstWeekDay( unsigned short p_weekday )
{
	dfw_setFirstWeekDay( p_weekday );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // setFirstWeekDay

void WordProcessingMerger::setWeekDayNames()
{
	dfw_setWeekDayNames0();

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // setWeekDayNames

void WordProcessingMerger::setWeekDayNames(
	const string& p_weekDay1,
	const string& p_weekDay2,
	const string& p_weekDay3,
	const string& p_weekDay4,
	const string& p_weekDay5,
	const string& p_weekDay6,
	const string& p_weekDay7 )
{
	dfw_setWeekDayNames7(
		p_weekDay1.c_str(),
		p_weekDay2.c_str(),
		p_weekDay3.c_str(),
		p_weekDay4.c_str(),
		p_weekDay5.c_str(),
		p_weekDay6.c_str(),
		p_weekDay7.c_str() );
		
	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // setWeekDayNames

void WordProcessingMerger::setWeekDayNames(
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
	dfw_setWeekDayNames14(
		p_fullName1.c_str(),
		p_fullName2.c_str(),
		p_fullName3.c_str(),
		p_fullName4.c_str(),
		p_fullName5.c_str(),
		p_fullName6.c_str(),
		p_fullName7.c_str(),

		p_shortName1.c_str(),
		p_shortName2.c_str(),
		p_shortName3.c_str(),
		p_shortName4.c_str(),
		p_shortName5.c_str(),
		p_shortName6.c_str(),
		p_shortName7.c_str() );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // setWeekDayNames

void WordProcessingMerger::setMonthNames()
{
	dfw_setMonthNames0();

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // setMonthNames

void WordProcessingMerger::setMonthNames(
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
	dfw_setMonthNames12(
		p_month1.c_str(),
		p_month2.c_str(),
		p_month3.c_str(),
		p_month4.c_str(),
		p_month5.c_str(),
		p_month6.c_str(),
		p_month7.c_str(),
		p_month8.c_str(),
		p_month9.c_str(),
		p_month10.c_str(),
		p_month11.c_str(),
		p_month12.c_str() );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // setMonthNames

void WordProcessingMerger::setMonthNames(
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
	dfw_setMonthNames24(
		p_fullName1.c_str(),
		p_fullName2.c_str(),
		p_fullName3.c_str(),
		p_fullName4.c_str(),
		p_fullName5.c_str(),
		p_fullName6.c_str(),
		p_fullName7.c_str(),
		p_fullName8.c_str(),
		p_fullName9.c_str(),
		p_fullName10.c_str(),
		p_fullName11.c_str(),
		p_fullName12.c_str(),

		p_shortName1.c_str(),
		p_shortName2.c_str(),
		p_shortName3.c_str(),
		p_shortName4.c_str(),
		p_shortName5.c_str(),
		p_shortName6.c_str(),
		p_shortName7.c_str(),
		p_shortName8.c_str(),
		p_shortName9.c_str(),
		p_shortName10.c_str(),
		p_shortName11.c_str(),
		p_shortName12.c_str() );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // setMonthNames

string WordProcessingMerger::getCodePage()
{
	return dfw_getCodePage();
} // getCodePage

char WordProcessingMerger::getNumFracSep()
{
	return dfw_getNumFracSep();
} // getNumFracSep

char WordProcessingMerger::getNumThSep()
{
	return dfw_getNumThSep();
} // getNumThSep

string WordProcessingMerger::getDateFormat()
{
	return dfw_getDateFormat();
} // getDateFormat

unsigned short WordProcessingMerger::getYearOffset()
{
	return dfw_getYearOffset();
} // getYearOffset

unsigned short WordProcessingMerger::getFirstWeekDay()
{
	return dfw_getFirstWeekDay();
} // getFirstWeekDay

string WordProcessingMerger::getWeekDayFullNames()
{
	return dfw_getWeekDayFullNames();
} // getWeekDayFullNames

string WordProcessingMerger::getWeekDayShortNames()
{
	return dfw_getWeekDayShortNames();
} // getWeekDayShortNames

string WordProcessingMerger::getMonthFullNames()
{
	return dfw_getMonthFullNames();
} // getMonthFullNames

string WordProcessingMerger::getMonthShortNames()
{
	return dfw_getMonthShortNames();
} // getMonthShortNames



// os functions
void WordProcessingMerger::setTempDir()
{
	dfw_setTempDir( "" );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // setTempDir()

void WordProcessingMerger::setTempDir( const string& p_dir )
{
	dfw_setTempDir( p_dir.c_str() );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // setTempDir

string WordProcessingMerger::getWorkDir() const
{
	return dfw_getWorkDir();
} // getWorkDir

string WordProcessingMerger::getTempDir() const
{
	return dfw_getTempDir();
} // getTempDir
