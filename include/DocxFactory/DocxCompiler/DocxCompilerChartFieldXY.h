
#ifndef __DOCXFACTORY_DOCX_COMPILER_CHART_FIELD_X_Y_H__
#define __DOCXFACTORY_DOCX_COMPILER_CHART_FIELD_X_Y_H__

#include "DocxFactory/DocxCompiler/DocxCompilerChartField.h"



namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class OpcPart;
	class DocxCompilerChartFieldXY : public DocxCompilerChartField
	{
	public:
		DocxCompilerChartFieldXY(
			DocxCompilerItem*		p_item,
			const string&			p_name,
			xercesc::DOMElement*	p_placeHolderNode,
			xercesc::DOMElement*	p_drawingNode,
			OpcPart*				p_chartPart );

		virtual ~DocxCompilerChartFieldXY();

		virtual void serialize( ZipFile* p_zipFile );

	protected:

	private:
		DocxCompilerChartFieldXY( const DocxCompilerChartFieldXY& p_other );
		DocxCompilerChartFieldXY& operator = ( const DocxCompilerChartFieldXY& p_other );

		list<pair<string, char>> m_chartStrings;

	};
};

#endif
