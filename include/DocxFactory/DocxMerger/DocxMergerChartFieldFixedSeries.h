
#ifndef __DOCXFACTORY_DOCX_MERGER_CHART_FIELD_FIXED_SERIES_H__
#define __DOCXFACTORY_DOCX_MERGER_CHART_FIELD_FIXED_SERIES_H__

#include "DocxFactory/DocxMerger/DocxMergerChartField.h"

#include <vector>



namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class UnzipFile;

	class DocxMergerPasteFieldGroup;
	class DocxMergerPasteChartFieldFixedSeries;
	class DocxMergerChartAxis;
	class DocxMergerChartAxisText;
	class DocxMergerChartValues;
	class DocxMergerChartFieldFixedSeries : public DocxMergerChartField
	{
	public:
		DocxMergerChartFieldFixedSeries();
		virtual ~DocxMergerChartFieldFixedSeries();

		virtual void setChartValue( const string&	p_series, const string&	p_category, double p_value );
		virtual void setChartValue( const string&	p_series, double		p_category, double p_value );
		virtual void setChartValue( double			p_series, double		p_category, double p_value );

		virtual void deserialize( UnzipFile* p_unzipFile );

		const list<pair<string, char>>*			getChartStrings() const;
		const vector<list<pair<string, char>>>*	getSeriesStrings() const;
		const map<string, char>*				getSeriesIdx() const;

		const DocxMergerChartAxis*				getCatAxis() const;
		DocxMergerField::FieldType				getCatType() const;
		const DocxMergerChartValues*			getVal() const;

	protected:

	private:
		DocxMergerChartFieldFixedSeries( const DocxMergerChartFieldFixedSeries& p_other );
		DocxMergerChartFieldFixedSeries& operator = ( const DocxMergerChartFieldFixedSeries& p_other );

		template <class Category_T>
		void setPasteChartValue( const string&	p_series, Category_T p_category, double p_value );

		DocxMergerPasteChartFieldFixedSeries* createPasteChartField();

		list<pair<string, char>>			m_chartStrings;
		vector<list<pair<string, char>>>	m_seriesStrings;
		map<string, char>					m_seriesIdx;

		DocxMergerChartAxis*				m_catAxis;
		DocxMergerField::FieldType			m_catType;
		DocxMergerChartValues*				m_val;

	};
};

#endif
