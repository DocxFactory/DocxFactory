
#ifndef __DOCXFACTORY_DOCX_COMPILER_DATE_TIME_FIELD_H__
#define __DOCXFACTORY_DOCX_COMPILER_DATE_TIME_FIELD_H__

#include "DocxFactory/DocxCompiler/DocxCompilerField.h"



namespace DocxFactory
{
	using namespace std;

	struct DateTimeFormat;

	class ZipFile;

	class DocxCompilerDateTimeField : public DocxCompilerField
	{
	public:
		DocxCompilerDateTimeField(
			DocxCompilerItem*				p_item,
			const string&					p_name,
			const string&					p_format,
			const map<FieldParam, string>*	p_valueByParam,
			xercesc::DOMElement*			p_placeHolderNode );

		virtual ~DocxCompilerDateTimeField();

		virtual void serialize( ZipFile* p_zipFile );

	protected:

	private:
		DocxCompilerDateTimeField( const DocxCompilerDateTimeField& p_other );
		DocxCompilerDateTimeField& operator = ( const DocxCompilerDateTimeField );

		list<DateTimeFormat*> m_dateTimeFormat;

	};
};

#endif
