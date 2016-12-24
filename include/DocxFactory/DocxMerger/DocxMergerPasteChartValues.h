
#ifndef __DOCXFACTORY_DOCX_MERGER_PASTE_CHART_VALUES_H__
#define __DOCXFACTORY_DOCX_MERGER_PASTE_CHART_VALUES_H__

#include <vector>



namespace DocxFactory
{
	using namespace std;

	class DocxMergerPasteChartValues
	{
	public:
		DocxMergerPasteChartValues();
		DocxMergerPasteChartValues( size_t p_serLength, size_t p_catLength );
		virtual ~DocxMergerPasteChartValues();

		virtual void setValue( size_t p_series, size_t p_category, double p_value );

		const vector<vector<double>>* getValues() const;

	protected:

	private:
		DocxMergerPasteChartValues( const DocxMergerPasteChartValues& p_other );
		DocxMergerPasteChartValues& operator = ( const DocxMergerPasteChartValues& p_other );

		vector<vector<double>> m_values;

	};
};

#endif
