
#ifndef __DOCXFACTORY_DOCX_MERGER_CHART_AXIS_H__
#define __DOCXFACTORY_DOCX_MERGER_CHART_AXIS_H__

#include "DocxFactory/DocxMerger/DocxMergerField.h"



namespace DocxFactory
{
	using namespace std;

	class DocxMergerPasteChartAxis;
	class DocxMergerChartAxis
	{
	public:
		DocxMergerChartAxis( DocxMergerField::FieldType p_axisType );
		virtual ~DocxMergerChartAxis();

		virtual size_t setValue	( DocxMergerPasteChartAxis* p_pasteAxis, const string& p_value );
		virtual size_t setValue	( DocxMergerPasteChartAxis* p_pasteAxis, double p_value );
		virtual string getValue	( DocxMergerPasteChartAxis* p_pasteAxis, size_t p_index ) const;
		virtual size_t getLength( DocxMergerPasteChartAxis* p_pasteAxis ) const;

		DocxMergerField::FieldType getAxisType() const;

	protected:
		DocxMergerField::FieldType m_axisType;

	private:
		DocxMergerChartAxis( const DocxMergerChartAxis& p_other );
		DocxMergerChartAxis& operator = ( const DocxMergerChartAxis& p_other );

	};
};

#endif
