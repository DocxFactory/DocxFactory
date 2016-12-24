
#ifndef __DOCXFACTORY_DOCX_COMPILER_ID_H__
#define __DOCXFACTORY_DOCX_COMPILER_ID_H__

#include <string>



namespace DocxFactory
{
	using namespace std;

	class ZipFile;

	class DocxCompilerItemFile;
	class DocxCompilerItem;
	class DocxCompilerId
	{
	public:
		DocxCompilerId(
			DocxCompilerItem*	p_item,
			const string&		p_tag,
			const string&		p_id );

		virtual ~DocxCompilerId();

		virtual void serialize( ZipFile* p_zipFile );

		string getTag() const;
		string getId() const;

	protected:

	private:
		DocxCompilerId( const DocxCompilerId& p_other );
		DocxCompilerId operator = ( const DocxCompilerId& p_other );

		DocxCompilerItem*	m_item;
		string				m_tag;
		string				m_id;
		string				m_prefix;
		int					m_seq;
		bool				m_isNum;

	};
};

#endif
