
#ifndef __DOCXFACTORY_DOCX_MERGER_PASTE_CHART_AXIS_H__
#define __DOCXFACTORY_DOCX_MERGER_PASTE_CHART_AXIS_H__

#include <string>



namespace DocxFactory
{
	using namespace std;

	class DocxMergerPasteChartAxis
	{
	public:
		DocxMergerPasteChartAxis();
		virtual ~DocxMergerPasteChartAxis();

	protected:

	private:
		DocxMergerPasteChartAxis( const DocxMergerPasteChartAxis& p_other );
		DocxMergerPasteChartAxis& operator = ( const DocxMergerPasteChartAxis& p_other );

	};
};

#endif
