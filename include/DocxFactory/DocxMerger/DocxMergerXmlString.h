
#ifndef __DOCXFACTORY_DOCX_MERGER_XML_STRING_H__
#define __DOCXFACTORY_DOCX_MERGER_XML_STRING_H__

#include <list>
#include <string>



namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class UnzipFile;

	class DocxMergerItemFile;
	class DocxMergerItem;
	class DocxMergerPasteFieldGroup;
	class DocxMergerXmlString
	{

	public:
		enum XmlStringType
		{
			TYPE_XML,
			TYPE_EMPTY_TABLE_START,
			TYPE_EMPTY_TABLE_END,
			TYPE_TC_START,
			TYPE_TC_END,
			TYPE_P_START,
			TYPE_ID,
			TYPE_ID_REF,
			TYPE_SHADE,
			TYPE_PAGE_BREAK,
			TYPE_SECTION,
			TYPE_ITEM_GROUP,
			TYPE_TEXT_FIELD,
			TYPE_NUMBER_FIELD,
			TYPE_DATE_TIME_FIELD,
			TYPE_BOOLEAN_FIELD,
			TYPE_ALT_CHUNK_FIELD,
			TYPE_PIC_FIELD,
			TYPE_BARCODE_FIELD,
			TYPE_CHART_FIELD
		};

		DocxMergerXmlString();
		virtual ~DocxMergerXmlString();

		void save( DocxMergerPasteFieldGroup* p_pasteFieldGroup );

		void deserialize( UnzipFile* p_unzipFile );
		void link		( UnzipFile* p_unzipFile );

		XmlStringType	getType() const;
		const string*	getStr() const;
		void*			getPtr() const;

	protected:

	private:
		DocxMergerXmlString( const DocxMergerXmlString& p_other );
		DocxMergerXmlString operator = ( const DocxMergerXmlString& p_other );
		
		DocxMergerItemFile*	m_itemFile;
		DocxMergerItem*		m_item;
		XmlStringType		m_type;
		string				m_str;
		void*				m_ptr;

	};
};

#endif
