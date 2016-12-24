
#ifndef __DOCXFACTORY_DOCX_MERGER_TEXT_FIELD_H__
#define __DOCXFACTORY_DOCX_MERGER_TEXT_FIELD_H__

#include "DocxFactory/DocxMerger/DocxMergerField.h"



namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class UnzipFile;

	class DocxMergerPasteFieldGroup;
	class DocxMergerTextField : public DocxMergerField
	{
	public:
		DocxMergerTextField();
		virtual ~DocxMergerTextField();

		virtual void save( DocxMergerPasteFieldGroup* p_pasteFieldGroup );

		virtual void setClipboardValue( const string& p_value );
		virtual void setClipboardValue( double p_value );

		virtual void deserialize( UnzipFile* p_unzipFile );

	protected:

	private:
		DocxMergerTextField( const DocxMergerTextField& p_other );
		DocxMergerTextField operator = ( const DocxMergerTextField );

		string encodeT(
			const string& p_str,
			const string& p_wordMlPrefix );

	};
};

#endif
