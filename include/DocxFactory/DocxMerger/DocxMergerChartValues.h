
#ifndef __DOCXFACTORY_DOCX_MERGER_CHART_VALUES_H__
#define __DOCXFACTORY_DOCX_MERGER_CHART_VALUES_H__

#include <string>



namespace DocxFactory
{
	using namespace std;

	class DocxMergerPasteChartValues;
	class DocxMergerChartValues
	{
	public:
		DocxMergerChartValues();
		virtual ~DocxMergerChartValues();

		virtual void	setValue( DocxMergerPasteChartValues* p_pasteValues, size_t p_series, size_t p_category, const string& p_value );
		virtual void	setValue( DocxMergerPasteChartValues* p_pasteValues, size_t p_series, size_t p_category, double p_value );
		virtual string	getValue( DocxMergerPasteChartValues* p_pasteValues, size_t p_series, size_t p_category ) const;

	protected:

	private:
		DocxMergerChartValues( const DocxMergerChartValues& p_other );
		DocxMergerChartValues& operator = ( const DocxMergerChartValues& p_other );

	};
};

#endif
