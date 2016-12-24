
#ifndef __DOCXFACTORY_DOCX_MERGER_PASTE_TEXT_FIELD_H__
#define __DOCXFACTORY_DOCX_MERGER_PASTE_TEXT_FIELD_H__

#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"



namespace DocxFactory
{
	using namespace std;

	class DocxMergerField;
	class DocxMergerPasteTextField : public DocxMergerPasteField
	{
	public:
		DocxMergerPasteTextField( DocxMergerField* p_field, const string& p_value );
		virtual ~DocxMergerPasteTextField();

		const string* getValue() const;

	private:
		DocxMergerPasteTextField( const DocxMergerPasteTextField& p_other );
		DocxMergerPasteTextField operator = ( const DocxMergerPasteTextField& p_other );

		string m_value;

	};
};

#endif
