
#ifndef __DOCXFACTORY_DOCX_MERGER_ITEM_H__
#define __DOCXFACTORY_DOCX_MERGER_ITEM_H__

#include <map>
#include <list>
#include <string>



namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class UnzipFile;

	class DocxMergerItemFile;
	class DocxMergerItemGroup;
	class DocxMergerItemLayout;
	class DocxMergerField;
	class DocxMergerXmlString;
	class DocxMergerPasteItem;
	class DocxMergerItem
	{
	public:
		DocxMergerItem();
		virtual ~DocxMergerItem();

		void save( DocxMergerPasteItem* p_pasteItem );

		void deserialize( UnzipFile* p_unzipFile );
		void link		( UnzipFile* p_unzipFile );

		DocxMergerItemFile*							getItemFile() const;
		DocxMergerItemGroup*						getItemGroup() const;
		string										getName() const;

		const list<DocxMergerItem*>*				getPath() const;
		const list<DocxMergerItemGroup*>*			getChildItemGroups() const;
		const multimap<string, DocxMergerField*>*	getFieldsByName() const;
		const list<DocxMergerXmlString*>*			getXmlStrings() const;

		bool										isTopLevel() const;
		DocxMergerItem*								getFillItemParent() const;
		DocxMergerItemGroup*						getFillItemGroup() const;

		DocxMergerItemLayout*						getItemLayout() const;
		double										getPageSize() const;
		bool										isPageBottom() const;
		bool										isKeepTogether() const;
		bool										isKeepWithPrev() const;

	protected:

	private:
		DocxMergerItem( const DocxMergerItem& p_other );
		DocxMergerItem operator = ( const DocxMergerItem& p_other );

		void fill( DocxMergerPasteItem* p_pasteItem );

		DocxMergerItemFile*					m_itemFile;
		DocxMergerItemGroup*				m_itemGroup;
		string								m_name;

		list<DocxMergerItem*>				m_path;
		list<DocxMergerItemGroup*>			m_childItemGroups;
		multimap<string, DocxMergerField*>	m_fieldsByName;
		list<DocxMergerXmlString*>			m_xmlStrings;

		bool								m_topLevel;
		DocxMergerItem*						m_fillItemParent;
		DocxMergerItemGroup*				m_fillItemGroup;

		DocxMergerItemLayout*				m_itemLayout;
		double								m_pageSize;
		bool								m_pageBottom;
		bool								m_keepTogether;
		bool								m_keepWithPrev;

	};
};

#endif
