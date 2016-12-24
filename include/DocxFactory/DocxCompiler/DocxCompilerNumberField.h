
#ifndef __DOCXFACTORY_DOCX_COMPILER_NUMBER_FIELD_H__
#define __DOCXFACTORY_DOCX_COMPILER_NUMBER_FIELD_H__

#include "DocxFactory/DocxCompiler/DocxCompilerField.h"



namespace DocxFactory
{
	using namespace std;

	struct NumFormat;

	class ZipFile;

	class DocxCompilerNumberField : public DocxCompilerField
	{
	public:
		DocxCompilerNumberField(
			DocxCompilerItem*				p_item,
			const string&					p_name,
			const string&					p_format,
			const map<FieldParam, string>*	p_valueByParam,
			xercesc::DOMElement*			p_placeHolderNode );

		virtual ~DocxCompilerNumberField();

		virtual void serialize( ZipFile* p_zipFile );

	protected:

	private:
		DocxCompilerNumberField( const DocxCompilerNumberField& p_other );
		DocxCompilerNumberField& operator = ( const DocxCompilerNumberField );

		NumFormat* m_numFormat;

	};
};

#endif
