
#ifndef __DOCXFACTORY_DOCX_MERGER_PASTE_CHART_AXIS_TEXT_H__
#define __DOCXFACTORY_DOCX_MERGER_PASTE_CHART_AXIS_TEXT_H__

#include "DocxFactory/DocxMerger/DocxMergerPasteChartAxis.h"

#include <map>
#include <vector>



namespace DocxFactory
{
	using namespace std;

	class DocxMergerPasteChartAxisText : public DocxMergerPasteChartAxis
	{
	public:
		DocxMergerPasteChartAxisText();
		virtual ~DocxMergerPasteChartAxisText();

		virtual size_t setValue( const string& p_value );

		const vector<string>* getValues() const;

	protected:

	private:
		DocxMergerPasteChartAxisText( const DocxMergerPasteChartAxisText& p_other );
		DocxMergerPasteChartAxisText& operator = ( const DocxMergerPasteChartAxisText& p_other );

		map<string, size_t>	m_index;
		vector<string>		m_values;

	};
};

#endif
