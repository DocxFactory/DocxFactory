
#ifndef __DOCXFACTORY_DOCX_MERGER_PASTE_ALT_CHUNK_FIELD_H__
#define __DOCXFACTORY_DOCX_MERGER_PASTE_ALT_CHUNK_FIELD_H__

#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"
#include "DocxFactory/DocxMerger/DocxMergerAltChunkField.h"



namespace DocxFactory
{
	using namespace std;

	class DocxMergerField;
	class DocxMergerPasteAltChunkField : public DocxMergerPasteField
	{
	public:
		DocxMergerPasteAltChunkField( DocxMergerField* p_field, const string& p_value, DocxMergerAltChunkField::AltChunkType p_type );
		virtual ~DocxMergerPasteAltChunkField();

		const string*							getValue() const;
		DocxMergerAltChunkField::AltChunkType	getType() const;

	protected:

	private:
		DocxMergerPasteAltChunkField( const DocxMergerPasteAltChunkField& p_other );
		DocxMergerPasteAltChunkField operator = ( const DocxMergerPasteAltChunkField& p_other );

		string									m_value;
		DocxMergerAltChunkField::AltChunkType	m_type;

	};
};

#endif
