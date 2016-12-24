
#ifndef __DOCXFACTORY_DOCX_MERGER_PASTE_NUMBER_FIELD_H__
#define __DOCXFACTORY_DOCX_MERGER_PASTE_NUMBER_FIELD_H__

#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"



namespace DocxFactory
{
	using namespace std;

	class DocxMergerField;
	class DocxMergerPasteNumberField : public DocxMergerPasteField
	{
	public:
		DocxMergerPasteNumberField( DocxMergerField* p_field, double p_value );
		virtual ~DocxMergerPasteNumberField();

		double getValue() const;

	private:
		DocxMergerPasteNumberField( const DocxMergerPasteNumberField& p_other );
		DocxMergerPasteNumberField operator = ( const DocxMergerPasteNumberField& p_other );

		double m_value;

	};
};

#endif
