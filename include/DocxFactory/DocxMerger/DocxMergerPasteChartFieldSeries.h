
#ifndef __DOCXFACTORY_DOCX_MERGER_PASTE_CHART_FIELD_SERIES_H__
#define __DOCXFACTORY_DOCX_MERGER_PASTE_CHART_FIELD_SERIES_H__

#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartField.h"



namespace DocxFactory
{
	using namespace std;

	class DocxMergerField;
	class DocxMergerPasteChartAxis;
	class DocxMergerPasteChartAxisText;
	class DocxMergerPasteChartValues;
	class DocxMergerPasteChartFieldSeries : public DocxMergerPasteChartField
	{
	public:
		DocxMergerPasteChartFieldSeries( DocxMergerField* p_field );
		virtual ~DocxMergerPasteChartFieldSeries();

		virtual string* getValue();

		const DocxMergerPasteChartAxisText*	getPasteSerAxis() const;
		const DocxMergerPasteChartAxis*		getPasteCatAxis() const;
		const DocxMergerPasteChartValues*	getPasteVal() const;

	protected:

	private:
		DocxMergerPasteChartFieldSeries( const DocxMergerPasteChartFieldSeries& p_other );
		DocxMergerPasteChartFieldSeries& operator = ( const DocxMergerPasteChartFieldSeries& p_other );

		DocxMergerPasteChartAxisText*	m_pasteSerAxis;
		DocxMergerPasteChartAxis*		m_pasteCatAxis;
		DocxMergerPasteChartValues*		m_pasteVal;

	};
};

#endif
