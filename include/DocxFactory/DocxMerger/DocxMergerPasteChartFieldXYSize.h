
#ifndef __DOCXFACTORY_DOCX_MERGER_PASTE_CHART_FIELD_X_Y_SIZE_H__
#define __DOCXFACTORY_DOCX_MERGER_PASTE_CHART_FIELD_X_Y_SIZE_H__

#include "DocxFactory/DocxMerger/DocxMergerPasteChartField.h"

#include <vector>



namespace DocxFactory
{
	using namespace std;

	class DocxMergerField;
	class DocxMergerPasteChartAxisNumber;
	class DocxMergerPasteChartFieldXYSize : public DocxMergerPasteChartField
	{
	public:
		DocxMergerPasteChartFieldXYSize( DocxMergerField* p_field );
		virtual ~DocxMergerPasteChartFieldXYSize();

		virtual string* getValue();

		void pushBack( double p_x, double p_y, double p_size );

		const vector<double>*	getPasteXAxis() const;
		const vector<double>*	getPasteYAxis() const;
		const vector<double>*	getPasteSizeAxis() const;

	protected:

	private:
		DocxMergerPasteChartFieldXYSize( const DocxMergerPasteChartFieldXYSize& p_other );
		DocxMergerPasteChartFieldXYSize& operator = ( const DocxMergerPasteChartFieldXYSize& p_other );

		vector<double> m_pasteXAxis;
		vector<double> m_pasteYAxis;
		vector<double> m_pasteSizeAxis;

	};
};

#endif
