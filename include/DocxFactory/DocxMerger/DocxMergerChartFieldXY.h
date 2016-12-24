
#ifndef __DOCXFACTORY_DOCX_MERGER_CHART_FIELD_X_Y_H__
#define __DOCXFACTORY_DOCX_MERGER_CHART_FIELD_X_Y_H__

#include "DocxFactory/DocxMerger/DocxMergerChartField.h"



namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class UnzipFile;

	class DocxMergerPasteFieldGroup;
	class DocxMergerChartFieldXY : public DocxMergerChartField
	{
	public:
		DocxMergerChartFieldXY();
		virtual ~DocxMergerChartFieldXY();

		virtual void setChartValue( const string&	p_series, const string&	p_category, double p_value );
		virtual void setChartValue( const string&	p_series, double		p_category, double p_value );
		virtual void setChartValue( double			p_series, double		p_category, double p_value );

		virtual void deserialize( UnzipFile* p_unzipFile );

		const list<pair<string, char>>* getChartStrings() const;

	protected:

	private:
		DocxMergerChartFieldXY( const DocxMergerChartFieldXY& p_other );
		DocxMergerChartFieldXY& operator = ( const DocxMergerChartFieldXY& p_other );

		void setPasteChartValue( double p_x, double p_y );

		list<pair<string, char>> m_chartStrings;

	};
};

#endif
