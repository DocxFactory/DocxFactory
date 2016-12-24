
#ifndef __DOCXFACTORY_DOCX_MERGER_PASTE_BOOLEAN_FIELD_H__
#define __DOCXFACTORY_DOCX_MERGER_PASTE_BOOLEAN_FIELD_H__

#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"



namespace DocxFactory
{
	using namespace std;

	class DocxMergerField;
	class DocxMergerPasteBooleanField : public DocxMergerPasteField
	{
	public:
		DocxMergerPasteBooleanField( DocxMergerField* p_field, bool p_value );
		virtual ~DocxMergerPasteBooleanField();

		bool getValue() const;

	protected:

	private:
		DocxMergerPasteBooleanField( const DocxMergerPasteBooleanField& p_other );
		DocxMergerPasteBooleanField operator = ( const DocxMergerPasteBooleanField& p_other );

		bool m_value;

	};
};

#endif
