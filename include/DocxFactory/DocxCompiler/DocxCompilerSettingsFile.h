
#ifndef __DOCXFACTORY_DOCX_COMPILER_SETTINGS_FILE_H__
#define __DOCXFACTORY_DOCX_COMPILER_SETTINGS_FILE_H__

#include "xercesc/util/XercesDefs.hpp"

#include <string>



XERCES_CPP_NAMESPACE_BEGIN

class DOMElement;

XERCES_CPP_NAMESPACE_END

namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class OpcPart;

	class DocxCompilerFile;
	class DocxCompilerSettingsFile
	{
	public:
		DocxCompilerSettingsFile( DocxCompilerFile* p_file );
		virtual ~DocxCompilerSettingsFile();

		void load();

		void serialize( ZipFile* p_zipFile );

		DocxCompilerFile*		getFile() const;
		OpcPart*				getPart() const;

		xercesc::DOMElement*	getSettingsNode() const;
		string					getWordMlPrefix() const;
		string					getRelMlPrefix() const;

	protected:

	private:
		DocxCompilerSettingsFile( const DocxCompilerSettingsFile& p_other );
		DocxCompilerSettingsFile& operator = ( const DocxCompilerSettingsFile& p_other );

		void normalize();

		DocxCompilerFile*		m_file;
		OpcPart*				m_part;

		xercesc::DOMElement*	m_settingsNode;
		string					m_wordMlPrefix;
		string					m_relMlPrefix;

	};
};

#endif
