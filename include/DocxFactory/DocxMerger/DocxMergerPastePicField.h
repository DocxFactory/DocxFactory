
#ifndef __DOCXFACTORY_DOCX_MERGER_PASTE_PIC_FIELD_H__
#define __DOCXFACTORY_DOCX_MERGER_PASTE_PIC_FIELD_H__

#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"

#include "DocxFactory/opc/OpcImageFile.h"



namespace DocxFactory
{
	using namespace std;

	class DocxMergerField;
	class DocxMergerPastePicField : public DocxMergerPasteField
	{
	public:
		DocxMergerPastePicField(
			DocxMergerField*			p_field,
			OpcImageFile::TargetMode	p_targetMode,
			const string&				p_fileName,
			const string&				p_fileExt,
			const string&				p_fileUrl );

		virtual ~DocxMergerPastePicField();

		OpcImageFile::TargetMode	getTargetMode() const;
		const string*				getFileName() const;
		const string*				getFileExt() const;
		const string*				getFileUrl() const;

	protected:

	private:
		DocxMergerPastePicField( const DocxMergerPastePicField& p_other );
		DocxMergerPastePicField operator = ( const DocxMergerPastePicField& p_other );

		OpcImageFile::TargetMode	m_targetMode;
		string						m_fileName;
		string						m_fileExt;
		string						m_fileUrl;

	};
};

#endif
