
#ifndef __DOCXFACTORY_DOCX_MERGER_PASTE_ITEM_GROUP_H__
#define __DOCXFACTORY_DOCX_MERGER_PASTE_ITEM_GROUP_H__

#include <list>



namespace DocxFactory
{
	using namespace std;

	class DocxMergerItemGroup;
	class DocxMergerPasteItem;
	class DocxMergerPasteItemGroup
	{
	public:
		DocxMergerPasteItemGroup( DocxMergerItemGroup* p_itemGroup, DocxMergerPasteItem* p_pasteItem );
		virtual ~DocxMergerPasteItemGroup();

		void pushBackChildPasteItem( DocxMergerPasteItem* p_pasteItem );
		void popBackChildPasteItem();

		void pushFrontChildPasteItem( DocxMergerPasteItem* p_pasteItem );
		void popFrontChildPasteItem();

		void setTrCnt	( double p_trCnt );
		void setFill	( unsigned int p_fill );

		DocxMergerItemGroup*				getItemGroup() const;
		DocxMergerPasteItem*				getPasteItem() const;
		const list<DocxMergerPasteItem*>*	getChildPasteItems() const;
		double								getTrCnt() const;
		unsigned int						getFill() const;

	protected:

	private:
		DocxMergerPasteItemGroup( const DocxMergerPasteItemGroup& p_other );
		DocxMergerPasteItemGroup operator = ( const DocxMergerPasteItemGroup& p_other );

		DocxMergerItemGroup*		m_itemGroup;
		DocxMergerPasteItem*		m_pasteItem;
		list<DocxMergerPasteItem*>	m_childPasteItems;
		double						m_trCnt;
		unsigned int				m_fill;

	};
};

#endif
