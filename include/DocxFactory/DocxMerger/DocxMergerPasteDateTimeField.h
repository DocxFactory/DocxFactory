
#ifndef __DOCXFACTORY_DOCX_MERGER_PASTE_DATE_TIME_FIELD_H__
#define __DOCXFACTORY_DOCX_MERGER_PASTE_DATE_TIME_FIELD_H__

#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"



namespace DocxFactory
{
	using namespace std;

	class DocxMergerField;
	class DocxMergerPasteDateTimeField : public DocxMergerPasteField
	{
	public:
		DocxMergerPasteDateTimeField( DocxMergerField* p_field, double p_time, short p_timeZone );
		virtual ~DocxMergerPasteDateTimeField();

		double	getTime() const;
		short	getTimeZone() const;

	private:
		DocxMergerPasteDateTimeField( const DocxMergerPasteDateTimeField& p_other );
		DocxMergerPasteDateTimeField operator = ( const DocxMergerPasteDateTimeField& p_other );

		double	m_time;
		short	m_timeZone;

	};
};

#endif
