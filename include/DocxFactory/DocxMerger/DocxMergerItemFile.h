
#ifndef __DOCXFACTORY_DOCX_MERGER_ITEM_FILE_H__
#define __DOCXFACTORY_DOCX_MERGER_ITEM_FILE_H__

#include "xercesc/util/XercesDefs.hpp"

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

	class UnzipFile;
	class ZipFile;
	class OpcPart;

	class DocxMergerFile;
	class DocxMergerItemGroup;
	class DocxMergerItem;
	class DocxMergerField;
	class DocxMergerAltChunkField;
	class DocxMergerXmlString;
	class DocxMergerPasteItemGroup;
	class DocxMergerPasteItem;
	class DocxMergerPasteFieldGroup;
	class DocxMergerPasteAltChunkField;
	class DocxMergerItemFile
	{
	public:
		enum ItemFileType
		{
			DOCUMENT,
			HEADER,
			FOOTER
		};

		DocxMergerItemFile();
		virtual ~DocxMergerItemFile();

		void save( ZipFile* p_zipFile );
		void paste( DocxMergerItem* p_item );

		void deserialize( UnzipFile* p_unzipFile );
		void link		( UnzipFile* p_unzipFile );

		void						pushBackOpenTc( bool p_openTc );
		void						popBackOpenTc();
		void						pushBackShade( const string& p_shade );
		void						popBackShade();

		void						setOpenTcBack( bool p_openTc );
		void						setShadeBack( const string& p_shade );
		void						setFirstPage( bool p_firstPage );
		void						setSect( DocxMergerXmlString* p_sect );

		DocxMergerFile*				getFile() const;
		DocxMergerItemGroup*		getRootItemGroup() const;
		OpcPart*					getPart() const;
		ItemFileType				getType() const;

		string						getWordMlPrefix() const;
		string						getRelMlPrefix() const;
		string						getVmlPrefix() const;

		DocxMergerPasteFieldGroup*	getPasteFieldGroup() const;

		ZipFile*					getZipFile() const;
		bool						getOpenTcBack() const;
		string						getShadeBack() const;
		bool						getFirstPage() const;
		DocxMergerXmlString*		getSect() const;

	protected:

	private:
		DocxMergerItemFile( const DocxMergerItemFile& p_other );
		DocxMergerItemFile operator = ( const DocxMergerItemFile& p_other );
		
		void pasteItem(
			DocxMergerItem*				p_item,
			DocxMergerPasteFieldGroup*	p_pasteFieldGroup,
			DocxMergerItem*				p_keepWithParentItem,
			DocxMergerPasteItem*		p_keepWithParentPasteItem );

		void closeItem();

		DocxMergerFile*					m_file;
		OpcPart*						m_part;
		ItemFileType					m_type;

		string							m_body1;
		string							m_body2;
		string							m_wordMlPrefix;
		string							m_relMlPrefix;
		string							m_vmlPrefix;

		DocxMergerItemGroup*			m_rootItemGroup;
		DocxMergerPasteItemGroup*		m_rootPasteItemGroup;
		DocxMergerItem*					m_topLevelItem;
		DocxMergerPasteItem*			m_topLevelPasteItem;
		DocxMergerItem*					m_fillItemParent;
		DocxMergerPasteItem*			m_fillPasteItemParent;
		DocxMergerItem*					m_keepTogetherItem;
		DocxMergerPasteItem*			m_keepTogetherPasteItem;

		list<DocxMergerItem*>			m_itemPath;
		list<DocxMergerPasteItem*>		m_pasteItemPath;
		DocxMergerPasteFieldGroup*		m_pasteFieldGroup;

		list<DocxMergerItemGroup*>		m_itemGroups;
		list<DocxMergerItem*>			m_items;
		list<DocxMergerField*>			m_fields;
		list<DocxMergerXmlString*>		m_sects;

		ZipFile*						m_zipFile;
		list<bool>						m_openTc;
		list<string>					m_shade;
		bool							m_firstPage;
		DocxMergerXmlString*			m_sect;

	};
};

#endif
