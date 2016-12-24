
#ifndef __DOCXFACTORY_WORD_PROCESSING_MERGER_H__
#define __DOCXFACTORY_WORD_PROCESSING_MERGER_H__

#include <map>
#include <set>
#include <string>



namespace DocxFactory
{
	using namespace std;

	class WordProcessingMerger
	{
	public:
		// word processing merger functions
		static WordProcessingMerger& getInstance();
		virtual ~WordProcessingMerger();

		void	load	( const string& p_fileName );
		void	save	( const string& p_fileName = "" );
		void	print	( const string& p_printerName, unsigned short p_copyCnt = 1 );
		void	close	();

		void	merge				( const string& p_data );
		void	setChartValue		( const string& p_itemName, const string& p_fieldName, const string&	p_series, const string&	p_category, double p_value );
		void	setChartValue		( const string& p_itemName, const string& p_fieldName, const string&	p_series, double		p_category, double p_value );
		void	setChartValue		( const string& p_itemName, const string& p_fieldName, double			p_series, double		p_category, double p_value );		
		void	setClipboardValue	( const string& p_itemName, const string& p_fieldName, const string& p_value );
		void	setClipboardValue	( const string& p_itemName, const string& p_fieldName, double p_value );
		void	paste				( const string& p_itemName = "" );

		void			setUpdateTocMethod( unsigned char p_method );
		unsigned char	getUpdateTocMethod() const;

		string	getFields() const;
		string	getItems() const;
		string	getItemParent	( const string& p_itemName ) const;
		string	getItemFields	( const string& p_itemName ) const;

		// locale functions
		void setCodePage();
		void setCodePage( const string& p_codePage );

		void setNumFracSep( char p_frac = 0 );
		void setNumThSep( char p_th = 0 );

		void setDateFormat( const string& p_format = "" );
		void setYearOffset( unsigned short p_year = 0 );
		void setFirstWeekDay();
		void setFirstWeekDay( unsigned short p_weekday );

		void setWeekDayNames();
		void setWeekDayNames(
			const string& p_weekDay1,
			const string& p_weekDay2,
			const string& p_weekDay3,
			const string& p_weekDay4,
			const string& p_weekDay5,
			const string& p_weekDay6,
			const string& p_weekDay7 );

		void setWeekDayNames(
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

		void setMonthNames();
		void setMonthNames(
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

		void setMonthNames(
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

		string 			getCodePage();
		char 			getNumFracSep();
		char 			getNumThSep();

		string 			getDateFormat();
		unsigned short 	getYearOffset();
		unsigned short 	getFirstWeekDay();

		string 			getWeekDayFullNames();
		string			getWeekDayShortNames();
		string			getMonthFullNames();
		string			getMonthShortNames();

		// os functions
		void	setTempDir();
		void	setTempDir( const string& p_dir );

		string	getWorkDir() const;
		string	getTempDir() const;

	protected:

	private:
		WordProcessingMerger();
		WordProcessingMerger( const WordProcessingMerger& p_other );
		WordProcessingMerger& operator = ( const WordProcessingMerger& p_other );

	};
};

#endif
