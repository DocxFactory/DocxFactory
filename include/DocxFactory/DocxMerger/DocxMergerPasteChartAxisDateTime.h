
#ifndef __DOCXFACTORY_DOCX_MERGER_PASTE_CHART_AXIS_DATE_TIME_H__
#define __DOCXFACTORY_DOCX_MERGER_PASTE_CHART_AXIS_DATE_TIME_H__

#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxis.h"

#include <map>
#include <vector>



namespace DocxFactory
{
	using namespace std;

	class DocxMergerPasteChartAxisDateTime : public DocxMergerPasteChartAxis
	{
	public:
		DocxMergerPasteChartAxisDateTime();
		virtual ~DocxMergerPasteChartAxisDateTime();

		virtual size_t setValue( double p_value );

		const vector<double>* getValues() const;

	protected:

	private:
		DocxMergerPasteChartAxisDateTime( const DocxMergerPasteChartAxisDateTime& p_other );
		DocxMergerPasteChartAxisDateTime& operator = ( const DocxMergerPasteChartAxisDateTime& p_other );

		map<double, size_t>	m_index;
		vector<double>		m_values;

	};
};

#endif
