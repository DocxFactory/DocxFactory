
#ifndef __DOCXFACTORY_DOCX_MERGER_FILE_H__
#define __DOCXFACTORY_DOCX_MERGER_FILE_H__

#include "DocxFactory/opc/OpcPackage.h"

#include "xercesc/util/XercesDefs.hpp"
#include "rapidjson/document.h"

#include <map>
#include <set>
#include <list>
#include <string>



XERCES_CPP_NAMESPACE_BEGIN

class DOMElement;

XERCES_CPP_NAMESPACE_END

namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class UnzipFile;
	class OpcPart;

	class DocxMergerSettingsFile;
	class DocxMergerItemFile;
	class DocxMergerItemGroup;
	class DocxMergerItem;
	class DocxMergerItemLayout;
	class DocxMergerField;
	class DocxMergerChartField;
	class DocxMergerXmlString;
	class DocxMergerId;
	class DocxMergerPasteItemGroup;
	class DocxMergerPasteItem;
	class DocxMergerPasteItemLayout;
	class DocxMergerPasteFieldGroup;
	class DocxMergerPasteField;
	class DocxMergerPasteAltChunkField;
	class DocxMergerFile : public OpcPackage
	{
	public:
		DocxMergerFile();
		virtual ~DocxMergerFile();

		virtual void load( const string& p_fileName );
		virtual void save( const string& p_fileName, bool p_updateToc = false );

		void merge				( const string& p_data );
		void setChartValue		( const string& p_itemName, const string& p_fieldName, const string&	p_series, const string&	p_category, double p_value );
		void setChartValue		( const string& p_itemName, const string& p_fieldName, const string&	p_series, double		p_category, double p_value );
		void setChartValue		( const string& p_itemName, const string& p_fieldName, double			p_series, double		p_category, double p_value );
		void setClipboardValue	( const string& p_itemName, const string& p_fieldName, const string& p_value );
		void setClipboardValue	( const string& p_itemName, const string& p_fieldName, double p_value );
		void paste				( const string& p_itemName );

		void			insertPasteFieldGroup( DocxMergerPasteFieldGroup* p_pasteFieldGroup );

		unsigned int	nextAltChunkSeq();
		string			getAltChunkDir() const;
		unsigned int	nextChartSeq();
		string			getChartDir() const;
		size_t			nextIdSeq();
		bool			hasToc() const;

		const map<string, DocxMergerItem*>*			getItemsByName() const;
		const multimap<string, DocxMergerField*>*	getFieldsByName() const;
		const multimap<string, DocxMergerField*>*	getHeaderFieldsByName() const;

	protected:

	private:
		DocxMergerFile( const DocxMergerFile& p_other );
		DocxMergerFile operator = ( const DocxMergerFile& p_other );

		void mergeByXml				( const string& p_xml );
		void mergeItemByXml			( xercesc::DOMElement*	p_node );
		void mergeFieldByXml		( xercesc::DOMElement*	p_node,		DocxMergerField*		p_field );
		void mergeChartFieldByXml	( xercesc::DOMElement*	p_node,		DocxMergerChartField*	p_chartField );

		void mergeByJson			( const string& p_json );
		void mergeItemByJson		( rapidjson::Value*		p_value,	DocxMergerItem*			p_item );
		void mergeFieldByJson		( rapidjson::Value*		p_value,	DocxMergerField*		p_field );
		void mergeChartFieldByJson	( rapidjson::Value*		p_value,	DocxMergerChartField*	p_chartField );

		template<class Series_T, class Category_T>
		void setChartValueImpl( const string& p_itemName, const string& p_fieldName, Series_T p_series, Category_T p_category, double p_value );

		template<class T>
		void setClipboardValueImpl( const string& p_itemName, const string& p_fieldName, T p_value );

		void deserialize			( UnzipFile* p_unzipFile );
		void deserializeSettingsFile( UnzipFile* p_unzipFile );
		void deserializeItemFiles	( UnzipFile* p_unzipFile );
		void deserializeItemGroups	( UnzipFile* p_unzipFile );
		void deserializeItems		( UnzipFile* p_unzipFile );
		void deserializeFields		( UnzipFile* p_unzipFile );
		void deserializeXmlStrings	( UnzipFile* p_unzipFile );
		void deserializeIds			( UnzipFile* p_unzipFile );

		void link					( UnzipFile* p_unzipFile );
		void linkSettingsFile		( UnzipFile* p_unzipFile );
		void linkItemFiles			( UnzipFile* p_unzipFile );
		void linkItemGroups			( UnzipFile* p_unzipFile );
		void linkItems				( UnzipFile* p_unzipFile );
		void linkFields				( UnzipFile* p_unzipFile );
		void linkXmlStrings			( UnzipFile* p_unzipFile );
		void linkIds				( UnzipFile* p_unzipFile );

		string								m_altChunkDir;
		unsigned int						m_altChunkSeq;
		string								m_chartDir;
		unsigned int						m_chartSeq;
		size_t								m_idSeq;
		bool								m_hasToc;

		DocxMergerSettingsFile*				m_settingsFile;
		list<DocxMergerItemFile*>			m_itemFiles;
		list<DocxMergerItemGroup*>			m_itemGroups;
		map<string, DocxMergerItem*>		m_itemsByName;

		multimap<string, DocxMergerField*>	m_fieldsByName;
		multimap<string, DocxMergerField*>	m_headerFieldsByName;
		list<DocxMergerPasteFieldGroup*>	m_pasteFieldGroups;
		list<DocxMergerXmlString*>			m_xmlStrings;
		list<DocxMergerId*>					m_ids;

	};
};

#endif
