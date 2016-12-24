
#ifndef __DOCXFACTORY_DOCX_COMPILER_XML_STRING_H__
#define __DOCXFACTORY_DOCX_COMPILER_XML_STRING_H__

#include <string>



namespace DocxFactory
{
	using namespace std;

	class ZipFile;

	class DocxCompilerItemFile;
	class DocxCompilerItem;
	class DocxCompilerXmlString
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

		DocxCompilerXmlString(
			DocxCompilerItem*	p_item,
			XmlStringType		p_type, 
			const string&		p_str,
			void*				p_ptr );

		virtual ~DocxCompilerXmlString();

		void serialize( ZipFile* p_zipFile );

		void concatStr( const string& p_str );

		DocxCompilerItemFile*	getItemFile() const;
		DocxCompilerItem*		getItem() const;
		XmlStringType			getType() const;
		const string*			getStr() const;
		void*					getPtr() const;

	protected:

	private:
		DocxCompilerXmlString( const DocxCompilerXmlString& p_other );
		DocxCompilerXmlString& operator = ( const DocxCompilerXmlString& p_other );

		DocxCompilerItemFile*	m_itemFile;
		DocxCompilerItem*		m_item;
		XmlStringType			m_type;
		string					m_str;
		void*					m_ptr;

	};
};

#endif
