
#ifndef __DOCXFACTORY_DOCX_COMPILER_CHART_FIELD_X_Y_SIZE_H__
#define __DOCXFACTORY_DOCX_COMPILER_CHART_FIELD_X_Y_SIZE_H__

#include "DocxFactory/DocxCompiler/DocxCompilerChartField.h"



namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class OpcPart;
	class DocxCompilerChartFieldXYSize : public DocxCompilerChartField
	{
	public:
		DocxCompilerChartFieldXYSize(
			DocxCompilerItem*		p_item,
			const string&			p_name,
			xercesc::DOMElement*	p_placeHolderNode,
			xercesc::DOMElement*	p_drawingNode,
			OpcPart*				p_chartPart );

		virtual ~DocxCompilerChartFieldXYSize();

		virtual void serialize( ZipFile* p_zipFile );

	protected:

	private:
		DocxCompilerChartFieldXYSize( const DocxCompilerChartFieldXYSize& p_other );
		DocxCompilerChartFieldXYSize& operator = ( const DocxCompilerChartFieldXYSize& p_other );

		list<pair<string, char>> m_chartStrings;

	};
};

#endif
