
#ifndef __DOCXFACTORY_DOCX_COMPILER_ITEM_GROUP_H__
#define __DOCXFACTORY_DOCX_COMPILER_ITEM_GROUP_H__

#include <map>
#include <string>



namespace DocxFactory
{
	using namespace std;

	class ZipFile;

	class DocxCompilerItemFile;
	class DocxCompilerItem;
	class DocxCompilerItemGroup
	{
	public:
		DocxCompilerItemGroup( DocxCompilerItemFile* p_itemFile, DocxCompilerItem* p_parent );
		virtual ~DocxCompilerItemGroup();

		void loadStripes();
		void createStripePlaceHolders();
		void serialize( ZipFile* p_zipFile );

		void setItem			( DocxCompilerItem* p_parent );
		void insertChildItem	( DocxCompilerItem* p_item );
		void setFillItem		( DocxCompilerItem* p_fillItem );
		void setSpacerItem		( DocxCompilerItem* p_spacerItem );
		void setCalcTrCnt		( bool p_calcTrCnt );

		DocxCompilerItem*						getItem() const;
		const map<string,DocxCompilerItem*>*	getChildItemsByName() const;
		const list<DocxCompilerItem*>*			getChildItems() const;

		DocxCompilerItem*						getFillItemTop() const;
		DocxCompilerItem*						getFillItemMiddle() const;
		DocxCompilerItem*						getFillItemBottom() const;
		DocxCompilerItem*						getFillItemSingle() const;
		bool									getFillItemEqual() const;
		double									getFillItemTrCnt() const;

		DocxCompilerItem*						getSpacerItem() const;
		double									getSpacerTrCnt() const;

		bool									isStriped() const;
		string									getShade1() const;
		string									getShade2() const;

	protected:

	private:	
		DocxCompilerItemGroup( const DocxCompilerItemGroup& p_other );
		DocxCompilerItemGroup& operator = ( const DocxCompilerItemGroup& p_other );

		bool isFieldExistsByItem( DocxCompilerItem* p_item );
		void createStripePlaceHoldersByGroup( DocxCompilerItemGroup* p_itemGroup );

		DocxCompilerItemFile*			m_itemFile;
		DocxCompilerItem*				m_item;

		map<string, DocxCompilerItem*>	m_childItemsByName;
		list<DocxCompilerItem*>			m_childItems;

		DocxCompilerItem*				m_fillItemTop;
		DocxCompilerItem*				m_fillItemMiddle;
		DocxCompilerItem*				m_fillItemBottom;
		DocxCompilerItem*				m_fillItemSingle;
		bool							m_fillItemEqual;
		double							m_fillItemTrCnt;

		DocxCompilerItem*				m_spacerItem;
		double							m_spacerTrCnt;

		string							m_shade1;
		string							m_shade2;
		bool							m_striped;

	};
};

#endif
