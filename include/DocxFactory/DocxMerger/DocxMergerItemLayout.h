
#ifndef __DOCXFACTORY_DOCX_MERGER_ITEM_LAYOUT_H__
#define __DOCXFACTORY_DOCX_MERGER_ITEM_LAYOUT_H__

#include <vector>
#include <list>
#include <string>



namespace DocxFactory
{
	using namespace std;

	class UnzipFile;

	class DocxMergerPasteItem;
	class DocxMergerItemGroup;
	class DocxMergerItemLayout
	{
	public:
		~DocxMergerItemLayout();

		static DocxMergerItemLayout* deserializeItemLayout( UnzipFile* p_unzipFile );
		void link( UnzipFile* p_unzipFile );

		double calcTrCnt( DocxMergerPasteItem* p_pasteItem ) const;

	protected:

	private:
		DocxMergerItemLayout();
		DocxMergerItemLayout( const DocxMergerItemLayout& p_other );
		DocxMergerItemLayout& operator = ( const DocxMergerItemLayout& p_other );

		void deserialize( UnzipFile* p_unzipFile );

		double													m_fixedTrCnt;
		list<list<pair<unsigned short, DocxMergerItemLayout*>>>	m_trList;
		list<DocxMergerItemGroup*>								m_childItemGroups;

	};
};

#endif
