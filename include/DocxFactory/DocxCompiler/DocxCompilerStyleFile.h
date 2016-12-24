
#ifndef __DOCXFACTORY_DOCX_COMPILER_STYLE_FILE_H__
#define __DOCXFACTORY_DOCX_COMPILER_STYLE_FILE_H__

#include "xercesc/util/XercesDefs.hpp"

#include <map>
#include <string>



XERCES_CPP_NAMESPACE_BEGIN

class DOMElement;

XERCES_CPP_NAMESPACE_END

namespace DocxFactory
{
	using namespace std;

	class OpcPart;

	class DocxCompilerFile;
	class DocxCompilerStyle;
	class DocxCompilerTableStyle;
	class DocxCompilerStyleFile
	{
	public:
		DocxCompilerStyleFile( DocxCompilerFile* p_file );
		virtual ~DocxCompilerStyleFile();

		void load();

		DocxCompilerFile*		getFile() const;
		OpcPart*				getPart() const;

		xercesc::DOMElement*	getStylesNode() const;
		string					getWordMlPrefix() const;
		string					getRelMlPrefix() const;

		const map<string, DocxCompilerTableStyle*>*	getTableStyles() const;

	protected:

	private:
		DocxCompilerStyleFile( const DocxCompilerStyleFile& p_other );
		DocxCompilerStyleFile& operator = ( const DocxCompilerStyleFile& p_other );

		void loadStyles();

		void normalize();
		void normalizeStyles();

		DocxCompilerFile*						m_file;
		OpcPart*								m_part;

		xercesc::DOMElement*					m_stylesNode;
		string									m_wordMlPrefix;
		string									m_relMlPrefix;

		map<string, DocxCompilerTableStyle*>	m_tableStyles;

	};
};

#endif
