
#ifndef __DOCXFACTORY_DOCX_MERGER_DATE_TIME_FIELD_H__
#define __DOCXFACTORY_DOCX_MERGER_DATE_TIME_FIELD_H__

#include "DocxFactory/DocxMerger/DocxMergerField.h"



namespace DocxFactory
{
	using namespace std;

	class	ZipFile;
	class	UnzipFile;
	struct	DateTimeFormat;

	class DocxMergerPasteFieldGroup;
	class DocxMergerDateTimeField : public DocxMergerField
	{
	public:
		DocxMergerDateTimeField();
		virtual ~DocxMergerDateTimeField();

		virtual void save( DocxMergerPasteFieldGroup* p_pasteFieldGroup );

		virtual void setClipboardValue( const string& p_value );
		virtual void setClipboardValue( double p_value );

		virtual void deserialize( UnzipFile* p_unzipFile );

	protected:

	private:
		DocxMergerDateTimeField( const DocxMergerDateTimeField& p_other );
		DocxMergerDateTimeField operator = ( const DocxMergerDateTimeField );

		void createPasteField( double p_time, short p_timeZone );

		list<DateTimeFormat*>	m_dateTimeFormat;

	};
};

#endif
