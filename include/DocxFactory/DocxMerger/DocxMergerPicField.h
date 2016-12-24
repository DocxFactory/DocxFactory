
#ifndef __DOCXFACTORY_DOCX_MERGER_PIC_FIELD_H__
#define __DOCXFACTORY_DOCX_MERGER_PIC_FIELD_H__

#include "DocxFactory/DocxMerger/DocxMergerField.h"

#include "DocxFactory/opc/OpcImageFile.h"



namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class UnzipFile;

	class DocxMergerPasteFieldGroup;
	class DocxMergerPicField : public DocxMergerField
	{
	public:
		enum UseSize
		{
			USE_IMAGE_FILE_SIZE,
			USE_PLACE_HOLDER_WIDTH,
			USE_PLACE_HOLDER_HEIGHT,
			USE_PLACE_HOLDER_SIZE
		};

		DocxMergerPicField();
		virtual ~DocxMergerPicField();

		virtual void save( DocxMergerPasteFieldGroup* p_pasteFieldGroup );

		virtual void setClipboardValue( const string& p_value );
		virtual void setClipboardValue( double p_value );
		
		virtual void deserialize( UnzipFile* p_unzipFile );

	protected:

	private:
		DocxMergerPicField( const DocxMergerPicField& p_other );
		DocxMergerPicField operator = ( const DocxMergerPicField& p_other );

		void createPasteField(
			OpcImageFile::TargetMode	p_targetMode,
			const string&				p_fileName,
			const string&				p_fileExt,
			const string&				p_fileUrl );

		unsigned short	m_picId;
		unsigned int	m_emuWidth;
		unsigned int	m_emuHeight;
		UseSize			m_useSize;

		list<pair<string, char>>	m_picStrings;

	};
};

#endif
