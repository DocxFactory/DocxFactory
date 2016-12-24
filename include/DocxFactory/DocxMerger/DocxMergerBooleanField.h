
#ifndef __DOCXFACTORY_DOCX_MERGER_BOOLEAN_FIELD_H__
#define __DOCXFACTORY_DOCX_MERGER_BOOLEAN_FIELD_H__

#include "DocxFactory/DocxMerger/DocxMergerField.h"



namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class UnzipFile;

	class DocxMergerPasteFieldGroup;
	class DocxMergerBooleanField : public DocxMergerField
	{
	public:
		enum BooleanValue
		{
			BOOLEAN_FALSE		= 0,
			BOOLEAN_TRUE		= 1,
			BOOLEAN_UNDEFINED
		};

		DocxMergerBooleanField();
		virtual ~DocxMergerBooleanField();

		virtual void save( DocxMergerPasteFieldGroup* p_pasteFieldGroup );

		virtual void setClipboardValue( const string& p_value );
		virtual void setClipboardValue( double p_value );

		virtual void deserialize( UnzipFile* p_unzipFile );

	protected:

	private:
		DocxMergerBooleanField( const DocxMergerBooleanField& p_other );
		DocxMergerBooleanField operator = ( const DocxMergerBooleanField& p_other );

		string			m_booleanString;
		BooleanValue	m_value;

	};
};

#endif
