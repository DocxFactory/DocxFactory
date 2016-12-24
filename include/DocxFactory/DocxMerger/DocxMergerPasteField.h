
#ifndef __DOCXFACTORY_DOCX_MERGER_PASTE_FIELD_H__
#define __DOCXFACTORY_DOCX_MERGER_PASTE_FIELD_H__

#include "DocxFactory/util/DocxFactoryDefs.h"



namespace DocxFactory
{
	using namespace std;

	class DocxMergerField;
	class DocxMergerPasteField
	{
	public:
		DocxMergerPasteField( DocxMergerField* p_field );
		virtual ~DocxMergerPasteField();

		const DocxMergerField* getField() const;

	protected:
		DocxMergerField* m_field;

	private:
		DocxMergerPasteField( const DocxMergerPasteField& p_other );
		DocxMergerPasteField operator = ( const DocxMergerPasteField& p_other );

	};
};

#endif
