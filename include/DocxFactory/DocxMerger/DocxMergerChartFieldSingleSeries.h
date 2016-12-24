
#ifndef __DOCXFACTORY_DOCX_MERGER_CHART_FIELD_SINGLE_SERIES_H__
#define __DOCXFACTORY_DOCX_MERGER_CHART_FIELD_SINGLE_SERIES_H__

#include "DocxFactory/DocxMerger/DocxMergerChartField.h"



namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class UnzipFile;

	class DocxMergerPasteFieldGroup;
	class DocxMergerChartAxis;
	class DocxMergerChartAxisText;
	class DocxMergerChartValues;
	class DocxMergerChartFieldSingleSeries : public DocxMergerChartField
	{
	public:
		DocxMergerChartFieldSingleSeries();
		virtual ~DocxMergerChartFieldSingleSeries();

		virtual void setChartValue( const string&	p_series, const string&	p_category, double p_value );
		virtual void setChartValue( const string&	p_series, double		p_category, double p_value );
		virtual void setChartValue( double			p_series, double		p_category, double p_value );

		virtual void deserialize( UnzipFile* p_unzipFile );

		const list<pair<string, char>>*	getChartStrings() const;
		const DocxMergerChartAxis*		getCatAxis() const;
		DocxMergerField::FieldType		getCatType() const;
		const DocxMergerChartValues*	getVal() const;

	protected:

	private:
		DocxMergerChartFieldSingleSeries( const DocxMergerChartFieldSingleSeries& p_other );
		DocxMergerChartFieldSingleSeries& operator = ( const DocxMergerChartFieldSingleSeries& p_other );

		template <class Category_T>
		void setPasteChartValue( Category_T p_category, double p_value );

		list<pair<string, char>>	m_chartStrings;
		DocxMergerChartAxis*		m_catAxis;
		DocxMergerField::FieldType	m_catType;
		DocxMergerChartValues*		m_val;

	};
};

#endif
