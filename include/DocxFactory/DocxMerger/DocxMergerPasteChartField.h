
#ifndef __DOCXFACTORY_DOCX_MERGER_PASTE_CHART_FIELD_H__
#define __DOCXFACTORY_DOCX_MERGER_PASTE_CHART_FIELD_H__

#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"



namespace DocxFactory
{
	using namespace std;

	class DocxMergerField;
	class DocxMergerPasteChartField : public DocxMergerPasteField
	{
	public:
		DocxMergerPasteChartField( DocxMergerField* p_field );
		virtual ~DocxMergerPasteChartField();

		virtual string* getValue();

	protected:
		string m_value;

	private:
		DocxMergerPasteChartField( const DocxMergerPasteChartField& p_other );
		DocxMergerPasteChartField& operator = ( const DocxMergerPasteChartField& p_other );

	};
};

#endif
