
#ifndef __DOCXFACTORY_DOCX_MERGER_SETTINGS_FILE_H__
#define __DOCXFACTORY_DOCX_MERGER_SETTINGS_FILE_H__

#include "xercesc/util/XercesDefs.hpp"

#include <string>



XERCES_CPP_NAMESPACE_BEGIN

class DOMElement;

XERCES_CPP_NAMESPACE_END

namespace DocxFactory
{
	using namespace std;

	class UnzipFile;
	class ZipFile;
	class OpcPart;

	class DocxMergerFile;
	class DocxMergerSettingsFile
	{
	public:
		DocxMergerSettingsFile();
		virtual ~DocxMergerSettingsFile();

		void deserialize( UnzipFile* p_unzipFile );
		void link		( UnzipFile* p_unzipFile );

		void setUpdateFields( bool p_val );

		DocxMergerFile*			getFile() const;
		OpcPart*				getPart() const;

		xercesc::DOMElement*	getSettingsNode() const;
		string					getWordMlPrefix() const;
		string					getRelMlPrefix() const;

	protected:

	private:
		DocxMergerSettingsFile( const DocxMergerSettingsFile& p_other );
		DocxMergerSettingsFile& operator = ( const DocxMergerSettingsFile& p_other );

		DocxMergerFile*			m_file;
		OpcPart*				m_part;

		xercesc::DOMElement*	m_settingsNode;
		string					m_wordMlPrefix;
		string					m_relMlPrefix;

	};
};

#endif
