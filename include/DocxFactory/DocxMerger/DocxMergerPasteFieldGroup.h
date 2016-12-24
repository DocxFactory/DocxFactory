
#ifndef __DOCXFACTORY_DOCX_MERGER_PASTE_FIELD_GROUP_H__
#define __DOCXFACTORY_DOCX_MERGER_PASTE_FIELD_GROUP_H__

#include <map>



namespace DocxFactory
{
	using namespace std;

	class DocxMergerField;
	class DocxMergerPasteField;
	class DocxMergerPasteFieldGroup
	{
	public:
		DocxMergerPasteFieldGroup();
		virtual ~DocxMergerPasteFieldGroup();

		bool insertPasteField( DocxMergerField* p_field, DocxMergerPasteField* p_pasteField );

		const map<DocxMergerField*, DocxMergerPasteField*>* getPasteFieldsByField() const;

	protected:

	private:
		DocxMergerPasteFieldGroup( const DocxMergerPasteFieldGroup& p_other );
		DocxMergerPasteFieldGroup operator = ( const DocxMergerPasteFieldGroup& p_other );

		map<DocxMergerField*, DocxMergerPasteField*> m_pasteFieldsByField;

	};
};

#endif
