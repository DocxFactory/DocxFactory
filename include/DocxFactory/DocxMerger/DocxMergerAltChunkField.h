
#ifndef __DOCXFACTORY_DOCX_MERGER_ALT_CHUNK_FIELD_H__
#define __DOCXFACTORY_DOCX_MERGER_ALT_CHUNK_FIELD_H__

#include "DocxFactory/DocxMerger/DocxMergerField.h"



namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class UnzipFile;

	class DocxMergerPasteFieldGroup;
	class DocxMergerAltChunkField : public DocxMergerField
	{
	public:
		enum AltChunkType
		{
			TYPE_MHTML,
			TYPE_HTML,
			TYPE_RTF
		};

		DocxMergerAltChunkField();
		virtual ~DocxMergerAltChunkField();

		virtual void save( DocxMergerPasteFieldGroup* p_pasteFieldGroup );

		virtual void setClipboardValue( const string& p_value );
		virtual void setClipboardValue( double p_value );

		virtual void deserialize( UnzipFile* p_unzipFile );

		AltChunkType getAltChunkType() const;

	protected:

	private:
		DocxMergerAltChunkField( const DocxMergerAltChunkField& p_other );
		DocxMergerAltChunkField operator = ( const DocxMergerAltChunkField& p_other );

		void insertAltChunk		( const string* p_value, AltChunkType p_type, string& p_fileRId );
		void createPasteField	( const string& p_value, AltChunkType p_type );

		void getMultiPart		( const string& p_str, string& m_multiPart );
		bool getAttr			( const string& p_str, const size_t& p_len, size_t& p_pos, string& p_name, string& p_value );
		bool getWord			( const string& p_str, const size_t& p_len, size_t& p_pos, string& p_keyword );
		bool skipSpace			( const string& p_str, const size_t& p_len, size_t& p_pos );

		AltChunkType	m_altChunkType;
		string			m_altChunkString1;
		string			m_altChunkString2;

	};
};

#endif
