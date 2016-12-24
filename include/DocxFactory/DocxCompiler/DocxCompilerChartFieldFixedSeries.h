
#ifndef __DOCXFACTORY_DOCX_COMPILER_CHART_FIELD_FIXED_SERIES_H__
#define __DOCXFACTORY_DOCX_COMPILER_CHART_FIELD_FIXED_SERIES_H__

#include "DocxFactory/DocxCompiler/DocxCompilerChartField.h"

#include <map>
#include <vector>



namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class OpcPart;
	class DocxCompilerChartFieldFixedSeries : public DocxCompilerChartField
	{
	public:
		DocxCompilerChartFieldFixedSeries(
			DocxCompilerItem*		p_item,
			const string&			p_name,
			xercesc::DOMElement*	p_placeHolderNode,
			xercesc::DOMElement*	p_drawingNode,
			OpcPart*				p_chartPart );

		virtual ~DocxCompilerChartFieldFixedSeries();

		virtual void serialize( ZipFile* p_zipFile );

	protected:

	private:
		DocxCompilerChartFieldFixedSeries( const DocxCompilerChartFieldFixedSeries& p_other );
		DocxCompilerChartFieldFixedSeries& operator = ( const DocxCompilerChartFieldFixedSeries& p_other );

		void loadSeries(
			xercesc::DOMElement*	p_seriesNode,
			char					p_seriesIdx );

		list<pair<string, char>>			m_chartStrings;
		vector<list<pair<string, char>>>	m_seriesStrings;
		map<string, char>					m_seriesIdx;
		DocxCompilerField::FieldType		m_catType;

	};
};

#endif
