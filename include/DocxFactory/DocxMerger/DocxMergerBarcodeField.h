
#ifndef __DOCXFACTORY_DOCX_MERGER_BARCODE_FIELD_H__
#define __DOCXFACTORY_DOCX_MERGER_BARCODE_FIELD_H__

#include "DocxFactory/DocxMerger/DocxMergerField.h"



namespace DocxFactory
{
	using namespace std;

	class	ZipFile;
	class 	UnzipFile;

	struct	BarcodeFormat;
	struct	NumFormat;

	class DocxMergerPasteFieldGroup;
	class DocxMergerBarcodeField : public DocxMergerField
	{
	public:
		DocxMergerBarcodeField();
		virtual ~DocxMergerBarcodeField();

		virtual void save( DocxMergerPasteFieldGroup* p_pasteFieldGroup );

		virtual void setClipboardValue( const string& p_value );
		virtual void setClipboardValue( double p_value );

		virtual void deserialize( UnzipFile* p_unzipFile );

	protected:

	private:
		DocxMergerBarcodeField( const DocxMergerBarcodeField& p_other );
		DocxMergerBarcodeField operator = ( const DocxMergerBarcodeField& p_other );

		BarcodeFormat*		m_barcodeFormat;
		static NumFormat*	m_numFormat;

	};
};

#endif
