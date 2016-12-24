
#ifndef __DOCXFACTORY_DOCX_MERGER_PASTE_CHART_FIELD_FIXED_SERIES_H__
#define __DOCXFACTORY_DOCX_MERGER_PASTE_CHART_FIELD_FIXED_SERIES_H__

#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartField.h"



namespace DocxFactory
{
	using namespace std;

	class DocxMergerField;
	class DocxMergerPasteChartAxis;
	class DocxMergerPasteChartValues;
	class DocxMergerPasteChartFieldFixedSeries : public DocxMergerPasteChartField
	{
	public:
		DocxMergerPasteChartFieldFixedSeries( DocxMergerField* p_field, size_t p_serLength );
		virtual ~DocxMergerPasteChartFieldFixedSeries();

		virtual string* getValue();

		const DocxMergerPasteChartAxis*		getPasteCatAxis() const;
		const DocxMergerPasteChartValues*	getPasteVal() const;

	protected:

	private:
		DocxMergerPasteChartFieldFixedSeries( const DocxMergerPasteChartFieldFixedSeries& p_other );
		DocxMergerPasteChartFieldFixedSeries& operator = ( const DocxMergerPasteChartFieldFixedSeries& p_other );

		DocxMergerPasteChartAxis*		m_pasteCatAxis;
		DocxMergerPasteChartValues*		m_pasteVal;

	};
};

#endif
