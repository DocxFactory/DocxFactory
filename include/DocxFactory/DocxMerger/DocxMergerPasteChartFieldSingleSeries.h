
#ifndef __DOCXFACTORY_DOCX_MERGER_PASTE_CHART_FIELD_SINGLE_SERIES_H__
#define __DOCXFACTORY_DOCX_MERGER_PASTE_CHART_FIELD_SINGLE_SERIES_H__

#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartField.h"



namespace DocxFactory
{
	using namespace std;

	class DocxMergerField;
	class DocxMergerPasteChartAxis;
	class DocxMergerPasteChartValues;
	class DocxMergerPasteChartFieldSingleSeries : public DocxMergerPasteChartField
	{
	public:
		DocxMergerPasteChartFieldSingleSeries( DocxMergerField* p_field );
		virtual ~DocxMergerPasteChartFieldSingleSeries();

		virtual string* getValue();

		const DocxMergerPasteChartAxis*		getPasteCatAxis() const;
		const DocxMergerPasteChartValues*	getPasteVal() const;

	protected:

	private:
		DocxMergerPasteChartFieldSingleSeries( const DocxMergerPasteChartFieldSingleSeries& p_other );
		DocxMergerPasteChartFieldSingleSeries& operator = ( const DocxMergerPasteChartFieldSingleSeries& p_other );

		DocxMergerPasteChartAxis*		m_pasteCatAxis;
		DocxMergerPasteChartValues*		m_pasteVal;

	};
};

#endif
