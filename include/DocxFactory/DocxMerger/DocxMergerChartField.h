
#ifndef __DOCXFACTORY_DOCX_MERGER_CHART_FIELD_H__
#define __DOCXFACTORY_DOCX_MERGER_CHART_FIELD_H__

#include "DocxFactory/DocxMerger/DocxMergerField.h"

#include "boost/tuple/tuple.hpp"



namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class UnzipFile;
	class DocxMergerChartField : public DocxMergerField
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

		DocxMergerChartField( ChartType p_chartType );
		virtual ~DocxMergerChartField();

		virtual void save( DocxMergerPasteFieldGroup* p_pasteFieldGroup );

		virtual void setChartValue( const string&	p_series, const string&	p_category, double p_value );
		virtual void setChartValue( const string&	p_series, double		p_category, double p_value );
		virtual void setChartValue( double			p_series, double		p_category, double p_value );

		virtual void deserialize( UnzipFile* p_unzipFile );

		ChartType												getChartType() const;
		string													getChartMlPrefix() const;
		string													getRelMlPrefix() const;
		const list<pair<string, char>>*							getDrawingStrings() const;
		const list<boost::tuple<string, string, string, bool>>*	getChartRelationships() const;

	protected:
		void saveChart( ZipFile* p_zipFile, const string* p_value );

		template <class Field_T>
		Field_T* createPasteChartField();

		ChartType											m_chartType;
		string												m_chartMlPrefix;
		string												m_relMlPrefix;
		list<pair<string, char>>							m_drawingStrings;
		list<boost::tuple<string, string, string, bool>>	m_chartRelationships; // id, type, part/external path, external

	private:
		DocxMergerChartField( const DocxMergerChartField& p_other );
		DocxMergerChartField& operator = ( const DocxMergerChartField& p_other );

	};

	template <class Field_T>
	Field_T* DocxMergerChartField::createPasteChartField()
	{
		DocxMergerPasteFieldGroup*										l_group;
		map<DocxMergerField*, DocxMergerPasteField*>::const_iterator	l_iterator;
		Field_T*														l_pasteField;

		l_group		= m_itemFile ->getPasteFieldGroup();
		l_iterator	= l_group ->getPasteFieldsByField() ->find( this );

		if( l_iterator != l_group ->getPasteFieldsByField() ->cend() )
			l_pasteField = ( Field_T* ) l_iterator ->second;

		else
		{
			l_pasteField = new Field_T( this );
			l_group ->insertPasteField( this, l_pasteField );
		}

		return l_pasteField;
	} // createPasteField
};

#endif
