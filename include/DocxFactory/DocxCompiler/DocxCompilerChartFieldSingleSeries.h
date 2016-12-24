
#ifndef __DOCXFACTORY_DOCX_COMPILER_CHART_FIELD_SINGLE_SERIES_H__
#define __DOCXFACTORY_DOCX_COMPILER_CHART_FIELD_SINGLE_SERIES_H__

#include "DocxFactory/DocxCompiler/DocxCompilerChartField.h"



namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class OpcPart;
	class DocxCompilerChartFieldSingleSeries : public DocxCompilerChartField
	{
	public:
		DocxCompilerChartFieldSingleSeries(
			DocxCompilerItem*		p_item,
			const string&			p_name,
			xercesc::DOMElement*	p_placeHolderNode,
			xercesc::DOMElement*	p_drawingNode,
			OpcPart*				p_chartPart );

		virtual ~DocxCompilerChartFieldSingleSeries();

		virtual void serialize( ZipFile* p_zipFile );

	protected:

	private:
		DocxCompilerChartFieldSingleSeries( const DocxCompilerChartFieldSingleSeries& p_other );
		DocxCompilerChartFieldSingleSeries& operator = ( const DocxCompilerChartFieldSingleSeries& p_other );

		list<pair<string, char>>		m_chartStrings;
		DocxCompilerField::FieldType	m_catType;

	};
};

#endif
