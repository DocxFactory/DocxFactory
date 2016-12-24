
#ifndef __DOCXFACTORY_H__
#define __DOCXFACTORY_H__

#include <wchar.h>



#if defined( __unix__ ) || defined( SWIG )
	#define DOCXFACTORY_DLL
#else
	#ifdef  DOCXFACTORY_DLL_EXPORT
	#define DOCXFACTORY_DLL __declspec(dllexport)
	#else
	#define DOCXFACTORY_DLL __declspec(dllimport)
	#endif
#endif



#ifdef __cplusplus
extern "C" {
#endif

DOCXFACTORY_DLL void			dfw_load	( const char* p_fileName );
DOCXFACTORY_DLL void			dfw_save	( const char* p_fileName );
DOCXFACTORY_DLL void			dfw_print	( const char* p_printerName, unsigned short p_copyCnt );
DOCXFACTORY_DLL void			dfw_close	();

DOCXFACTORY_DLL void			dfw_merge						( const char*		p_data );
DOCXFACTORY_DLL void			dfw_mergeW						( const wchar_t*	p_data );
DOCXFACTORY_DLL void			dfw_setChartValueByStrStr		( const char*		p_itemName, const char*		p_fieldName, const char*	p_series, const char*		p_category, double p_value );
DOCXFACTORY_DLL void			dfw_setChartValueByStrStrW		( const wchar_t*	p_itemName, const wchar_t*	p_fieldName, const wchar_t*	p_series, const wchar_t*	p_category, double p_value );
DOCXFACTORY_DLL void			dfw_setChartValueByStrDouble	( const char*		p_itemName, const char*		p_fieldName, const char*	p_series, double			p_category, double p_value );
DOCXFACTORY_DLL void			dfw_setChartValueByStrDoubleW	( const wchar_t*	p_itemName, const wchar_t*	p_fieldName, const wchar_t*	p_series, double			p_category, double p_value );
DOCXFACTORY_DLL void			dfw_setChartValueByDoubleDouble	( const char*		p_itemName, const char*		p_fieldName, double			p_series, double			p_category, double p_value );
DOCXFACTORY_DLL void			dfw_setClipboardValueByStr		( const char*		p_itemName, const char*		p_fieldName, const char*	p_value );
DOCXFACTORY_DLL void			dfw_setClipboardValueByStrW		( const wchar_t*	p_itemName, const wchar_t*	p_fieldName, const wchar_t*	p_value );
DOCXFACTORY_DLL void			dfw_setClipboardValueByDouble	( const char*		p_itemName, const char*		p_fieldName, double			p_value );
DOCXFACTORY_DLL void			dfw_paste						( const char*		p_itemName );

DOCXFACTORY_DLL void			dfw_setUpdateTocMethod( unsigned char p_method );
DOCXFACTORY_DLL unsigned char	dfw_getUpdateTocMethod();

DOCXFACTORY_DLL const char*		dfw_getFields();
DOCXFACTORY_DLL const char*		dfw_getItems();
DOCXFACTORY_DLL const char*		dfw_getItemParent	( const char* p_itemName );
DOCXFACTORY_DLL const char*		dfw_getItemFields	( const char* p_itemName );

DOCXFACTORY_DLL void			dfw_setCodePage		( const char* p_codePage );
DOCXFACTORY_DLL void			dfw_setNumFracSep	( char p_frac );
DOCXFACTORY_DLL void			dfw_setNumThSep		( char p_th );

DOCXFACTORY_DLL void			dfw_setDateFormat	( const char* p_format );
DOCXFACTORY_DLL void			dfw_setYearOffset	( unsigned short p_year );
DOCXFACTORY_DLL void			dfw_setFirstWeekDay	( unsigned short p_weekday );

DOCXFACTORY_DLL void			dfw_setWeekDayNames0();
DOCXFACTORY_DLL void			dfw_setWeekDayNames7(
	const char* p_weekDay1,
	const char* p_weekDay2,
	const char* p_weekDay3,
	const char* p_weekDay4,
	const char* p_weekDay5,
	const char* p_weekDay6,
	const char* p_weekDay7 );

DOCXFACTORY_DLL void			dfw_setWeekDayNames7W(
	const wchar_t* p_weekDay1,
	const wchar_t* p_weekDay2,
	const wchar_t* p_weekDay3,
	const wchar_t* p_weekDay4,
	const wchar_t* p_weekDay5,
	const wchar_t* p_weekDay6,
	const wchar_t* p_weekDay7 );

DOCXFACTORY_DLL void			dfw_setWeekDayNames14(
	const char* p_fullName1,
	const char* p_fullName2,
	const char* p_fullName3,
	const char* p_fullName4,
	const char* p_fullName5,
	const char* p_fullName6,
	const char* p_fullName7,

	const char* p_shortName1,
	const char* p_shortName2,
	const char* p_shortName3,
	const char* p_shortName4,
	const char* p_shortName5,
	const char* p_shortName6,
	const char* p_shortName7 );

DOCXFACTORY_DLL void			dfw_setWeekDayNames14W(
	const wchar_t* p_fullName1,
	const wchar_t* p_fullName2,
	const wchar_t* p_fullName3,
	const wchar_t* p_fullName4,
	const wchar_t* p_fullName5,
	const wchar_t* p_fullName6,
	const wchar_t* p_fullName7,

	const wchar_t* p_shortName1,
	const wchar_t* p_shortName2,
	const wchar_t* p_shortName3,
	const wchar_t* p_shortName4,
	const wchar_t* p_shortName5,
	const wchar_t* p_shortName6,
	const wchar_t* p_shortName7 );

DOCXFACTORY_DLL void			dfw_setMonthNames0();
DOCXFACTORY_DLL void			dfw_setMonthNames12(
	const char* p_month1,
	const char* p_month2,
	const char* p_month3,
	const char* p_month4,
	const char* p_month5,
	const char* p_month6,
	const char* p_month7,
	const char* p_month8,
	const char* p_month9,
	const char* p_month10,
	const char* p_month11,
	const char* p_month12 );

DOCXFACTORY_DLL void			dfw_setMonthNames12W(
	const wchar_t* p_month1,
	const wchar_t* p_month2,
	const wchar_t* p_month3,
	const wchar_t* p_month4,
	const wchar_t* p_month5,
	const wchar_t* p_month6,
	const wchar_t* p_month7,
	const wchar_t* p_month8,
	const wchar_t* p_month9,
	const wchar_t* p_month10,
	const wchar_t* p_month11,
	const wchar_t* p_month12 );

DOCXFACTORY_DLL void			dfw_setMonthNames24(
	const char* p_fullName1,
	const char* p_fullName2,
	const char* p_fullName3,
	const char* p_fullName4,
	const char* p_fullName5,
	const char* p_fullName6,
	const char* p_fullName7,
	const char* p_fullName8,
	const char* p_fullName9,
	const char* p_fullName10,
	const char* p_fullName11,
	const char* p_fullName12,

	const char* p_shortName1,
	const char* p_shortName2,
	const char* p_shortName3,
	const char* p_shortName4,
	const char* p_shortName5,
	const char* p_shortName6,
	const char* p_shortName7,
	const char* p_shortName8,
	const char* p_shortName9,
	const char* p_shortName10,
	const char* p_shortName11,
	const char* p_shortName12 );

DOCXFACTORY_DLL void			dfw_setMonthNames24W(
	const wchar_t* p_fullName1,
	const wchar_t* p_fullName2,
	const wchar_t* p_fullName3,
	const wchar_t* p_fullName4,
	const wchar_t* p_fullName5,
	const wchar_t* p_fullName6,
	const wchar_t* p_fullName7,
	const wchar_t* p_fullName8,
	const wchar_t* p_fullName9,
	const wchar_t* p_fullName10,
	const wchar_t* p_fullName11,
	const wchar_t* p_fullName12,

	const wchar_t* p_shortName1,
	const wchar_t* p_shortName2,
	const wchar_t* p_shortName3,
	const wchar_t* p_shortName4,
	const wchar_t* p_shortName5,
	const wchar_t* p_shortName6,
	const wchar_t* p_shortName7,
	const wchar_t* p_shortName8,
	const wchar_t* p_shortName9,
	const wchar_t* p_shortName10,
	const wchar_t* p_shortName11,
	const wchar_t* p_shortName12 );

DOCXFACTORY_DLL const char*		dfw_getCodePage();
DOCXFACTORY_DLL char 			dfw_getNumFracSep();
DOCXFACTORY_DLL char 			dfw_getNumThSep();

DOCXFACTORY_DLL const char* 	dfw_getDateFormat();
DOCXFACTORY_DLL unsigned short 	dfw_getYearOffset();
DOCXFACTORY_DLL unsigned short 	dfw_getFirstWeekDay();

DOCXFACTORY_DLL const char* 	dfw_getWeekDayFullNames();
DOCXFACTORY_DLL const wchar_t*	dfw_getWeekDayFullNamesW();
DOCXFACTORY_DLL const char*		dfw_getWeekDayShortNames();
DOCXFACTORY_DLL const wchar_t*	dfw_getWeekDayShortNamesW();
DOCXFACTORY_DLL const char*		dfw_getMonthFullNames();
DOCXFACTORY_DLL const wchar_t*	dfw_getMonthFullNamesW();
DOCXFACTORY_DLL const char*		dfw_getMonthShortNames();
DOCXFACTORY_DLL const wchar_t*	dfw_getMonthShortNamesW();

DOCXFACTORY_DLL void			dfw_setTempDir( const char* p_dir );
DOCXFACTORY_DLL const char*		dfw_getTempDir();
DOCXFACTORY_DLL const char*		dfw_getWorkDir();

DOCXFACTORY_DLL const char*		dfw_getErrorWhat();
DOCXFACTORY_DLL const char*		dfw_getErrorWhere();
DOCXFACTORY_DLL unsigned char	dfw_getErrorFlag();

#ifdef __cplusplus
}
#endif

#ifdef DOCXFACTORY_DLL
#undef DOCXFACTORY_DLL
#endif
#endif
