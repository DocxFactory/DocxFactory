
#ifndef __DOCXFACTORY_DOCX_COMPILER_TEXT_FIELD_H__
#define __DOCXFACTORY_DOCX_COMPILER_TEXT_FIELD_H__

#include "DocxFactory/DocxCompiler/DocxCompilerField.h"



namespace DocxFactory
{
	using namespace std;

	class ZipFile;

	class DocxCompilerTextField : public DocxCompilerField
	{
	public:
		DocxCompilerTextField(
			DocxCompilerItem*				p_item,
			const string&					p_name,
			const string&					p_format,
			const map<FieldParam, string>*	p_valueByParam,
			xercesc::DOMElement*			p_placeHolderNode );

		virtual ~DocxCompilerTextField();

		virtual void serialize( ZipFile* p_zipFile );

	protected:

	private:
		DocxCompilerTextField( const DocxCompilerTextField& p_other );
		DocxCompilerTextField& operator = ( const DocxCompilerTextField );

	};
};

#endif
