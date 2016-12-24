
#ifndef __DOCXFACTORY_DOCX_COMPILER_CHART_FIELD_H__
#define __DOCXFACTORY_DOCX_COMPILER_CHART_FIELD_H__

#include "DocxFactory/DocxCompiler/DocxCompilerField.h"

#include "boost/tuple/tuple.hpp"



namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class OpcPart;
	class DocxCompilerChartField : public DocxCompilerField
	{
	public:
		enum ChartType
		{
			CHART_SERIES,
			CHART_SINGLE_SERIES,
			CHART_FIXED_SERIES,
			CHART_X_Y,
			CHART_X_Y_SIZE
		};

		static ChartType getChartType( OpcPart* p_part );



		DocxCompilerChartField(
			DocxCompilerItem*		p_item,
			const string&			p_name,
			ChartType				p_chartType,
			xercesc::DOMElement*	p_placeHolderNode,
			xercesc::DOMElement*	p_drawingNode,
			OpcPart*				p_chartPart );

		virtual ~DocxCompilerChartField();

		virtual void serialize( ZipFile* p_zipFile );

		ChartType												getChartType() const;
		string													getChartMlPrefix() const;
		string													getRelMlPrefix() const;
		const list<pair<string, char>>*							getDrawingStrings() const;
		const list<boost::tuple<string, string, string, bool>>*	getChartRelationships() const;

	protected:
		ChartType											m_chartType;
		string												m_chartMlPrefix;
		string												m_relMlPrefix;
		list<pair<string, char>>							m_drawingStrings;
		list<boost::tuple<string, string, string, bool>>	m_chartRelationships; // id, type, part/external path, external

	private:
		DocxCompilerChartField( const DocxCompilerChartField& p_other );
		DocxCompilerChartField& operator = ( const DocxCompilerChartField& p_other );

	};
};

#endif
