
#ifndef __DOCXFACTORY_DOCX_MERGER_FIELD_H__
#define __DOCXFACTORY_DOCX_MERGER_FIELD_H__

#include "DocxFactory/DocxMerger/DocxMergerItemFile.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteFieldGroup.h"

#include "DocxFactory/util/DocxFactoryDefs.h"

#include <list>
#include <string>



namespace DocxFactory
{
	using namespace std;

	class UnzipFile;

	class DocxMergerFile;
	class DocxMergerItemFile;
	class DocxMergerItem;
	class DocxMergerPasteField;
	class DocxMergerField
	{
	public:
		enum FieldType
		{
			TYPE_UNDEFINED,
			TYPE_TEXT,
			TYPE_NUMBER,
			TYPE_DATE_TIME,
			TYPE_BOOLEAN,
			TYPE_ALT_CHUNK,
			TYPE_PIC,
			TYPE_BARCODE,
			TYPE_CHART
		};

		DocxMergerField( FieldType p_type );
		virtual ~DocxMergerField();

		virtual void save( DocxMergerPasteFieldGroup* p_pasteFieldGroup );

		virtual void setClipboardValue( const string& p_value );
		virtual void setClipboardValue( double p_value );

		virtual void deserialize( UnzipFile* p_unzipFile );
		virtual void link( UnzipFile* p_unzipFile );

		DocxMergerItemFile*	getItemFile() const;
		DocxMergerItem*		getItem() const;
		FieldType			getType() const;
		string				getName() const;

	protected:
		template <class Value_T, class Field_T>
		void createPasteField( Value_T p_value );

		DocxMergerFile*		m_file;
		DocxMergerItemFile*	m_itemFile;
		DocxMergerItem*		m_item;
		string				m_name;
		FieldType			m_type;

	private:
		DocxMergerField( const DocxMergerField& p_other );
		DocxMergerField operator = ( const DocxMergerField& p_other );

	};
	


	template <class Value_T, class Field_T>
	void DocxMergerField::createPasteField( Value_T p_value )
	{
		DocxMergerPasteField*	l_pasteField;
		bool					l_ok;

		l_pasteField	= new Field_T( this, p_value );
		l_ok			= m_itemFile ->getPasteFieldGroup() ->insertPasteField( this, l_pasteField );

		if ( !l_ok )
			delete l_pasteField;
	} // createPasteField
};

#endif
