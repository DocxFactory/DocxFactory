
#ifndef __DOCXFACTORY_DOCX_MERGER_ID_H__
#define __DOCXFACTORY_DOCX_MERGER_ID_H__

#include <string>



namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class UnzipFile;

	class DocxMergerItemFile;
	class DocxMergerItem;
	class DocxMergerId
	{
	public:
		DocxMergerId();
		virtual ~DocxMergerId();

		void deserialize( UnzipFile* p_unzipFile );
		void link		( UnzipFile* p_unzipFile );

		string getNextId();
		string getCurrId();

	protected:

	private:
		DocxMergerFile*	m_file;
		DocxMergerItem*	m_item;
		string			m_tag;
		string			m_id;
		string			m_prefix;
		size_t			m_seq;
		bool			m_isNum;

	};
};

#endif
