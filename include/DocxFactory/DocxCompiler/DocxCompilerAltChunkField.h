
#ifndef __DOCXFACTORY_DOCX_COMPILER_ALT_CHUNK_FIELD_H__
#define __DOCXFACTORY_DOCX_COMPILER_ALT_CHUNK_FIELD_H__

#include "DocxFactory/DocxCompiler/DocxCompilerField.h"



namespace DocxFactory
{
	using namespace std;

	class ZipFile;

	class DocxCompilerAltChunkField : public DocxCompilerField
	{
	public:
		enum AltChunkType
		{
			TYPE_MHTML,
			TYPE_HTML,
			TYPE_RTF
		};

		DocxCompilerAltChunkField(
			DocxCompilerItem*				p_item,
			const string&					p_name,
			const string&					p_format,
			const map<FieldParam, string>*	p_valueByParam,
			xercesc::DOMElement*			p_placeHolderNode );

		virtual ~DocxCompilerAltChunkField();

		virtual void serialize( ZipFile* p_zipFile );

	protected:

	private:
		DocxCompilerAltChunkField( const DocxCompilerAltChunkField& p_other );
		DocxCompilerAltChunkField& operator = ( const DocxCompilerAltChunkField& p_other );

		AltChunkType	m_type;

		// the altchunk rid attr value goes between altChunkString1 and altChunkString2
		string			m_altChunkString1;
		string			m_altChunkString2;
	};
};

#endif
