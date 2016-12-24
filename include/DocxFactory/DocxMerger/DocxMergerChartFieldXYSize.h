
#ifndef __DOCXFACTORY_DOCX_MERGER_CHART_FIELD_X_Y_SIZE_H__
#define __DOCXFACTORY_DOCX_MERGER_CHART_FIELD_X_Y_SIZE_H__

#include "DocxFactory/DocxMerger/DocxMergerChartField.h"



namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class UnzipFile;

	class DocxMergerPasteFieldGroup;
	class DocxMergerChartFieldXYSize : public DocxMergerChartField
	{
	public:
		DocxMergerChartFieldXYSize();
		virtual ~DocxMergerChartFieldXYSize();

		virtual void setChartValue( const string&	p_series, const string&	p_category, double p_value );
		virtual void setChartValue( const string&	p_series, double		p_category, double p_value );
		virtual void setChartValue( double			p_series, double		p_category, double p_value );

		virtual void deserialize( UnzipFile* p_unzipFile );

		const list<pair<string, char>>* getChartStrings() const;

	protected:

	private:
		DocxMergerChartFieldXYSize( const DocxMergerChartFieldXYSize& p_other );
		DocxMergerChartFieldXYSize& operator = ( const DocxMergerChartFieldXYSize& p_other );

		void setPasteChartValue( double p_x, double p_y, double p_size );

		list<pair<string, char>> m_chartStrings;

	};
};

#endif
