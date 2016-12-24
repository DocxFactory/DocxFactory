
#ifndef __DOCXFACTORY_DOCX_MERGER_PASTE_BARCODE_FIELD_H__
#define __DOCXFACTORY_DOCX_MERGER_PASTE_BARCODE_FIELD_H__

#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"



namespace DocxFactory
{
	using namespace std;

	class DocxMergerField;
	class DocxMergerPasteBarcodeField : public DocxMergerPasteField
	{
	public:
		DocxMergerPasteBarcodeField( DocxMergerField* p_field, const string& p_value );
		virtual ~DocxMergerPasteBarcodeField();

		const string* getValue();

	private:
		DocxMergerPasteBarcodeField( const DocxMergerPasteBarcodeField& p_other );
		DocxMergerPasteBarcodeField operator = ( const DocxMergerPasteBarcodeField& p_other );

		string m_value;

	};
};

#endif
