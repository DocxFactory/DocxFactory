
#ifndef __DOCXFACTORY_DOCX_COMPILER_ITEM_FILE_H__
#define __DOCXFACTORY_DOCX_COMPILER_ITEM_FILE_H__

#include "xercesc/util/XercesDefs.hpp"

#include <map>
#include <list>
#include <vector>
#include <utility>
#include <string>



XERCES_CPP_NAMESPACE_BEGIN

class DOMElement;

XERCES_CPP_NAMESPACE_END

namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class OpcPart;

	class DocxCompilerFile;
	class DocxCompilerBookmark;
	class DocxCompilerItemGroup;
	class DocxCompilerItem;
	class DocxCompilerItemLayout;
	class DocxCompilerField;
	class DocxCompilerXmlString;
	class DocxCompilerItemFile
	{
	public:
		enum ItemFileType
		{
			DOCUMENT,
			HEADER,
			FOOTER
		};

		DocxCompilerItemFile(
			DocxCompilerFile*	p_file,
			OpcPart*			p_part,
			ItemFileType		p_type );

		virtual ~DocxCompilerItemFile();

		void load();
		void loadBookmarks();
		void loadItems();
		void loadFields();
		void loadStripes();
		void loadXmlStrings();

		void serialize( ZipFile* p_zipFile );

		void insertItem	( DocxCompilerItem*	p_item );
		void insertField( DocxCompilerField* p_field );

		DocxCompilerFile*		getFile() const;
		OpcPart*				getPart() const;
		ItemFileType			getType() const;

		xercesc::DOMElement*	getBodyNode() const;
		string					getWordMlPrefix() const;
		string					getRelMlPrefix() const;
		string					getVmlPrefix() const;

		const vector<DocxCompilerXmlString*>* getSects() const;

	protected:

	private:
		DocxCompilerItemFile( const DocxCompilerItemFile& p_other );
		DocxCompilerItemFile& operator = ( const DocxCompilerItemFile& p_other );

		void loadItemsByBookmark();
		void loadItemsByBookmark( DocxCompilerBookmark* p_bookmark, list<DocxCompilerItem*>* p_itemPath );
		void loadItemLayout		( DocxCompilerItem* p_item, DocxCompilerItem*& p_fillItem );
		void setItemsPath		( DocxCompilerItemGroup* p_itemGroup );

		void normalize();
		void normalizeRuns();
		void normalizeTableBorders();
		void normalizeBookmarksStartEnd();
		void normalizeBookmarksBr();
		void normalizeBookmarksHierarchy( DocxCompilerBookmark* p_bookmark = NULL );

		void trimItems();

		DocxCompilerFile*	m_file;
		OpcPart*			m_part;
		ItemFileType		m_type;

		xercesc::DOMElement*	m_bodyNode;
		string					m_body1;
		string					m_body2;
		string					m_wordMlPrefix;
		string					m_relMlPrefix;
		string					m_vmlPrefix;
		size_t					m_groupIdSeq;
		size_t					m_sectLen;

		list<DocxCompilerBookmark*>			m_rootBookmarks;
		map<size_t, DocxCompilerBookmark*>	m_bookmarksById;
		DocxCompilerItemGroup*				m_rootItemGroup;
		list<DocxCompilerItemGroup*>		m_itemGroups;
		list<DocxCompilerItem*>				m_items;
		list<DocxCompilerField*>			m_fields;
		vector<DocxCompilerXmlString*>		m_sects;

	};
};

#endif
