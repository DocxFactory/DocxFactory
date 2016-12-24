
#ifndef __DOCXFACTORY_DOCX_MERGER_CHART_FIELD_SERIES_H__
#define __DOCXFACTORY_DOCX_MERGER_CHART_FIELD_SERIES_H__

#include "DocxFactory/DocxMerger/DocxMergerChartField.h"

#include <vector>



namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class UnzipFile;

	class DocxMergerPasteFieldGroup;
	class DocxMergerChartAxis;
	class DocxMergerChartAxisText;
	class DocxMergerChartValues;
	class DocxMergerChartFieldSeries : public DocxMergerChartField
	{
	public:
		DocxMergerChartFieldSeries();
		virtual ~DocxMergerChartFieldSeries();

		virtual void setChartValue( const string&	p_series, const string&	p_category, double p_value );
		virtual void setChartValue( const string&	p_series, double		p_category, double p_value );
		virtual void setChartValue( double			p_series, double		p_category, double p_value );

		virtual void deserialize( UnzipFile* p_unzipFile );

		const list<pair<string, char>>*			getChartStrings() const;
		const vector<list<pair<string, char>>>*	getSeriesStrings() const;

		const DocxMergerChartAxisText*			getSerAxis() const;
		const DocxMergerChartAxis*				getCatAxis() const;
		DocxMergerField::FieldType				getCatType() const;
		const DocxMergerChartValues*			getVal() const;

	protected:

	private:
		DocxMergerChartFieldSeries( const DocxMergerChartFieldSeries& p_other );
		DocxMergerChartFieldSeries& operator = ( const DocxMergerChartFieldSeries& p_other );

		template <class Series_T, class Category_T>
		void setPasteChartValue( Series_T p_series, Category_T p_category, double p_value );

		list<pair<string, char>>			m_chartStrings;
		vector<list<pair<string, char>>>	m_seriesStrings;

		DocxMergerChartAxisText*			m_serAxis;
		DocxMergerChartAxis*				m_catAxis;
		DocxMergerField::FieldType			m_catType;
		DocxMergerChartValues*				m_val;

	};
};

#endif
