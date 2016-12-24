
#ifndef __DOCXFACTORY_DOCX_MERGER_PASTE_ITEM_H__
#define __DOCXFACTORY_DOCX_MERGER_PASTE_ITEM_H__

#include <map>



namespace DocxFactory
{
	using namespace std;

	class DocxMergerItemGroup;
	class DocxMergerItem;
	class DocxMergerPasteFieldGroup;
	class DocxMergerPasteItemGroup;
	class DocxMergerPasteItem
	{
	public:
		DocxMergerPasteItem( DocxMergerItem* p_item, DocxMergerPasteFieldGroup* p_pasteFieldGroup );
		virtual ~DocxMergerPasteItem();

		void pushBackChildPasteItem	( DocxMergerPasteItem*		p_pasteItem );
		void popBackChildPasteItem	( DocxMergerPasteItemGroup*	p_pasteItemGroup );

		void pushFrontChildPasteItem( DocxMergerPasteItem*		p_pasteItem );
		void popFrontChildPasteItem	( DocxMergerPasteItemGroup*	p_pasteItemGroup );

		DocxMergerPasteItemGroup*	insertChildPasteItemGroup( DocxMergerItemGroup* p_itemGroup );
		void						removeChildPasteItemGroup( DocxMergerItemGroup* p_itemGroup );

		void setPasteItemGroup		( DocxMergerPasteItemGroup* p_pasteItemGroup );
		void setFillPasteItemParent	( DocxMergerPasteItem* p_pasteItem );
		void setTrCnt();

		DocxMergerItem*												getItem() const;
		DocxMergerPasteItemGroup*									getPasteItemGroup() const;
		DocxMergerPasteFieldGroup*									getPasteFieldGroup() const;
		const map<DocxMergerItemGroup*, DocxMergerPasteItemGroup*>* getChildPasteItemGroupsByGroup() const;
		DocxMergerPasteItem*										getFillPasteItemParent() const;
		double														getTrCnt() const;

	protected:

	private:
		DocxMergerPasteItem( const DocxMergerPasteItem& p_other );
		DocxMergerPasteItem operator = ( const DocxMergerPasteItem& p_other );

		DocxMergerItem*											m_item;
		DocxMergerPasteItemGroup*								m_pasteItemGroup;
		DocxMergerPasteFieldGroup*								m_pasteFieldGroup;
		map<DocxMergerItemGroup*, DocxMergerPasteItemGroup*>	m_childPasteItemGroupsByGroup;
		DocxMergerPasteItem*									m_fillPasteItemParent;
		double													m_trCnt;

	};
};

#endif
