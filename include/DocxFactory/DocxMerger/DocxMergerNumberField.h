
#ifndef __DOCXFACTORY_DOCX_MERGER_NUMBER_FIELD_H__
#define __DOCXFACTORY_DOCX_MERGER_NUMBER_FIELD_H__

#include "DocxFactory/DocxMerger/DocxMergerField.h"



namespace DocxFactory
{
	using namespace std;

	class	ZipFile;
	class	UnzipFile;
	struct	NumFormat;

	class DocxMergerPasteFieldGroup;
	class DocxMergerNumberField : public DocxMergerField
	{
	public:
		DocxMergerNumberField();
		virtual ~DocxMergerNumberField();

		virtual void save( DocxMergerPasteFieldGroup* p_pasteFieldGroup );

		virtual void setClipboardValue( const string& p_value );
		virtual void setClipboardValue( double p_value );

		virtual void deserialize( UnzipFile* p_unzipFile );

	protected:

	private:
		DocxMergerNumberField( const DocxMergerNumberField& p_other );
		DocxMergerNumberField operator = ( const DocxMergerNumberField );

		NumFormat*	m_numFormat;

	};
};

#endif
