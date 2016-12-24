
#ifndef __DOCXFACTORY_DOCX_COMPILER_PIC_FIELD_H__
#define __DOCXFACTORY_DOCX_COMPILER_PIC_FIELD_H__

#include "DocxFactory/DocxCompiler/DocxCompilerField.h"



namespace DocxFactory
{
	using namespace std;

	class ZipFile;

	class DocxCompilerPicField : public DocxCompilerField
	{
	public:
		enum UseSize
		{
			USE_IMAGE_FILE_SIZE,
			USE_PLACE_HOLDER_WIDTH,
			USE_PLACE_HOLDER_HEIGHT,
			USE_PLACE_HOLDER_SIZE
		};

		static void getFormat( 
			string&						p_format,
			map<FieldParam, string>*	p_valueByParam );



		DocxCompilerPicField(
			DocxCompilerItem*				p_item,
			const string&					p_name,
			const string&					p_format,
			const map<FieldParam, string>*	p_valueByParam,
			xercesc::DOMElement*			p_placeHolderNode,
			xercesc::DOMElement*			p_drawingNode );

		virtual ~DocxCompilerPicField();

		virtual void serialize( ZipFile* p_zipFile );

		string getFileRId() const;

	protected:

	private:
		DocxCompilerPicField( const DocxCompilerPicField& p_other );
		DocxCompilerPicField& operator = ( const DocxCompilerPicField& p_other );

		unsigned int	m_picId;
		unsigned int	m_emuWidth;
		unsigned int	m_emuHeight;
		UseSize			m_useSize;

		string						m_fileRId;
		list<pair<string, char>>	m_picStrings;

	};
};

#endif
