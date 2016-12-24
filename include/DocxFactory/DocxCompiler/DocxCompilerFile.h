
#ifndef __DOCXFACTORY_DOCX_COMPILER_FILE_H__
#define __DOCXFACTORY_DOCX_COMPILER_FILE_H__

#include "DocxFactory/opc/OpcPackage.h"

#include "DocxFactory/util/DocxFactoryDefs.h"

#include <map>
#include <list>
#include <string>



namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class OpcPart;

	class DocxCompilerStyleFile;
	class DocxCompilerSettingsFile;
	class DocxCompilerItemFile;
	class DocxCompilerStyleFile;
	class DocxCompilerBookmark;
	class DocxCompilerItemGroup;
	class DocxCompilerItem;
	class DocxCompilerField;
	class DocxCompilerXmlString;
	class DocxCompilerId;
	class DocxCompilerFile : public OpcPackage
	{
	public:
		DocxCompilerFile();
		virtual ~DocxCompilerFile();

		void compile( const string& p_srcFileName, const string& p_dstFileName );

		unsigned int nextHeaderSeq();
		unsigned int nextFooterSeq();

		void			insertBookmark	( DocxCompilerBookmark* p_bookmark );
		void			insertItemGroup	( DocxCompilerItemGroup* p_itemGroup );
		void			insertItem		( DocxCompilerItem* p_item );
		void			insertField		( DocxCompilerField* p_field );
		void			insertXmlString	( DocxCompilerXmlString* p_xmlString );
		DocxCompilerId*	insertId		( DocxCompilerItem*	p_item, const string& p_tag, const string& p_id );
		void			setToc			( bool p_hasToc );

		DocxCompilerStyleFile*						getStyleFile() const;
		DocxCompilerSettingsFile*					getSettingsFile() const;
		const map<string, DocxCompilerBookmark*>*	getBookmarksByName() const;
		const map<string, DocxCompilerItem*>*		getItemsByName() const;
		const multimap<string, DocxCompilerField*>*	getFieldsByName() const;
		const multimap<string, DocxCompilerField*>*	getHeaderFieldsByName() const;
		const map<string, DocxCompilerId*>*			getIdsById() const;
		bool										hasToc() const;

	protected:

	private:
		DocxCompilerFile( const DocxCompilerFile& p_other );
		DocxCompilerFile& operator = ( const DocxCompilerFile& p_other );

		virtual void load( const string& p_fileName );
		virtual void save( const string& p_fileName );

		void loadStyleFile();
		void loadSettingsFile();
		void loadItemFiles();
		void loadBookmarks();
		void loadItems();
		void loadFields();
		void loadStripes();
		void loadXmlStrings();

		void serialize				( ZipFile* p_zipFile );
		void serializeSettingsFile	( ZipFile* p_zipFile );
		void serializeItemFiles		( ZipFile* p_zipFile );
		void serializeItems			( ZipFile* p_zipFile );
		void serializeItemGroups	( ZipFile* p_zipFile );
		void serializeFields		( ZipFile* p_zipFile );
		void serializeXmlStrings	( ZipFile* p_zipFile );
		void serializeIds			( ZipFile* p_zipFile );

		OpcPart*								m_documentPart;
		DocxCompilerStyleFile*					m_styleFile;
		DocxCompilerSettingsFile*				m_settingsFile;
		string									m_altChunkDir;
		string									m_chartDir;
		bool									m_hasToc;

		unsigned int							m_headerSeq;
		unsigned int							m_footerSeq;

		list<DocxCompilerItemFile*>				m_itemFiles;
		map<string, DocxCompilerBookmark*>		m_bookmarksByName;
		map<string, DocxCompilerItem*>			m_itemsByName;
		list<DocxCompilerItemGroup*>			m_itemGroups;

		multimap<string, DocxCompilerField*>	m_fieldsByName;
		multimap<string, DocxCompilerField*>	m_headerFieldsByName;
		list<DocxCompilerXmlString*>			m_xmlStrings;
		map<string, DocxCompilerId*>			m_idsById;

	};
};

#endif
