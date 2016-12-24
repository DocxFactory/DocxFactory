
#ifndef __DOCXFACTORY_DOCX_COMPILER_BARCODE_FIELD_H__
#define __DOCXFACTORY_DOCX_COMPILER_BARCODE_FIELD_H__

#include "DocxFactory/DocxCompiler/DocxCompilerField.h"



namespace DocxFactory
{
	using namespace std;

	class	ZipFile;
	struct	BarcodeFormat;

	class DocxCompilerBarcodeField : public DocxCompilerField
	{
	public:
		DocxCompilerBarcodeField(
			DocxCompilerItem*				p_item,
			const string&					p_name,
			const string&					p_format,
			const map<FieldParam, string>*	p_valueByParam,
			xercesc::DOMElement*			p_placeHolderNode );

		virtual ~DocxCompilerBarcodeField();

		virtual void serialize( ZipFile* p_zipFile );

	protected:

	private:
		DocxCompilerBarcodeField( const DocxCompilerBarcodeField& p_other );
		DocxCompilerBarcodeField& operator = ( const DocxCompilerBarcodeField& p_other );
		
		BarcodeFormat* m_barcodeFormat;

	};
};

#endif
