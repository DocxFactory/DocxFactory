
#ifndef __DOCXFACTORY_DOCX_COMPILER_CHART_FIELD_SERIES_H__
#define __DOCXFACTORY_DOCX_COMPILER_CHART_FIELD_SERIES_H__

#include "DocxFactory/DocxCompiler/DocxCompilerChartField.h"

#include <vector>



namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class OpcPart;
	class DocxCompilerChartFieldSeries : public DocxCompilerChartField
	{
	public:
		DocxCompilerChartFieldSeries(
			DocxCompilerItem*		p_item,
			const string&			p_name,
			xercesc::DOMElement*	p_placeHolderNode,
			xercesc::DOMElement*	p_drawingNode,
			OpcPart*				p_chartPart );

		virtual ~DocxCompilerChartFieldSeries();

		virtual void serialize( ZipFile* p_zipFile );

	protected:

	private:
		DocxCompilerChartFieldSeries( const DocxCompilerChartFieldSeries& p_other );
		DocxCompilerChartFieldSeries& operator = ( const DocxCompilerChartFieldSeries& p_other );

		void loadSeries(
			xercesc::DOMElement*	p_seriesNode,
			char					p_seriesIdx );

		list<pair<string, char>>			m_chartStrings;
		vector<list<pair<string, char>>>	m_seriesStrings;
		DocxCompilerField::FieldType		m_catType;

	};
};

#endif
