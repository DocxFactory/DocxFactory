
#ifndef __DOCXFACTORY_DOCX_MERGER_PASTE_CHART_AXIS_NUMBER_H__
#define __DOCXFACTORY_DOCX_MERGER_PASTE_CHART_AXIS_NUMBER_H__

#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxis.h"

#include <map>
#include <vector>



namespace DocxFactory
{
	using namespace std;

	class DocxMergerPasteChartAxisNumber : public DocxMergerPasteChartAxis
	{
	public:
		DocxMergerPasteChartAxisNumber();
		virtual ~DocxMergerPasteChartAxisNumber();

		virtual size_t setValue( double p_value );

		const vector<double>* getValues() const;

	protected:

	private:
		DocxMergerPasteChartAxisNumber( const DocxMergerPasteChartAxisNumber& p_other );
		DocxMergerPasteChartAxisNumber& operator = ( const DocxMergerPasteChartAxisNumber& p_other );

		map<double, size_t>	m_index;
		vector<double>		m_values;

	};
};

#endif
