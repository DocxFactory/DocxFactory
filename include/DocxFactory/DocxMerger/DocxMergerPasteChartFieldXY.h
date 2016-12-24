
#ifndef __DOCXFACTORY_DOCX_MERGER_PASTE_CHART_FIELD_X_Y_H__
#define __DOCXFACTORY_DOCX_MERGER_PASTE_CHART_FIELD_X_Y_H__

#include "DocxFactory/DocxMerger/DocxMergerPasteChartField.h"

#include <vector>



namespace DocxFactory
{
	using namespace std;

	class DocxMergerField;
	class DocxMergerPasteChartAxisNumber;
	class DocxMergerPasteChartFieldXY : public DocxMergerPasteChartField
	{
	public:
		DocxMergerPasteChartFieldXY( DocxMergerField* p_field );
		virtual ~DocxMergerPasteChartFieldXY();

		virtual string* getValue();

		void pushBack( double p_x, double p_y );

		const vector<double>*	getPasteXAxis() const;
		const vector<double>*	getPasteYAxis() const;

	protected:

	private:
		DocxMergerPasteChartFieldXY( const DocxMergerPasteChartFieldXY& p_other );
		DocxMergerPasteChartFieldXY& operator = ( const DocxMergerPasteChartFieldXY& p_other );

		vector<double> m_pasteXAxis;
		vector<double> m_pasteYAxis;

	};
};

#endif
