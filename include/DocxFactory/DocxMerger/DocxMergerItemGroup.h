
#ifndef __DOCXFACTORY_DOCX_MERGER_ITEM_GROUP_H__
#define __DOCXFACTORY_DOCX_MERGER_ITEM_GROUP_H__

#include <map>
#include <list>
#include <string>



namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class UnzipFile;

	class DocxMergerPasteItemGroup;
	class DocxMergerItemFile;
	class DocxMergerItem;
	class DocxMergerItemGroup
	{
	public:
		DocxMergerItemGroup();
		virtual ~DocxMergerItemGroup();

		void save( DocxMergerPasteItemGroup* p_pasteItemGroup );

		void deserialize( UnzipFile* p_unzipFile );
		void link		( UnzipFile* p_unzipFile );

		DocxMergerItem*						getItem() const;
		const map<string, DocxMergerItem*>*	getChildItemsByName() const;

		DocxMergerItem*						getSpacerItem() const;
		double								getSpacerTrCnt() const;

		bool								getFillItemEqual() const;
		double								getFillItemTrCnt() const;

	protected:

	private:
		DocxMergerItemGroup( const DocxMergerItemGroup& p_other );
		DocxMergerItemGroup operator = ( const DocxMergerItemGroup& p_other );

		void fill(
			DocxMergerPasteItemGroup*	p_pasteItemGroup,
			bool&						p_shadeFlag );

		DocxMergerItemFile*				m_itemFile;
		DocxMergerItem*					m_item;
		map<string, DocxMergerItem*>	m_childItemsByName;

		DocxMergerItem*					m_fillItemTop;
		DocxMergerItem*					m_fillItemMiddle;
		DocxMergerItem*					m_fillItemBottom;
		DocxMergerItem*					m_fillItemSingle;
		bool							m_fillItemEqual;
		double							m_fillItemTrCnt;

		DocxMergerItem*					m_spacerItem;
		double							m_spacerTrCnt;

		bool							m_striped;
		string							m_shade1;
		string							m_shade2;

	};
};

#endif
