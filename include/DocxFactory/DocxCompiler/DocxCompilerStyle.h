
#ifndef __DOCXFACTORY_DOCX_COMPILER_STYLE_H__
#define __DOCXFACTORY_DOCX_COMPILER_STYLE_H__

#include "xercesc/util/XercesDefs.hpp"

#include <string>



XERCES_CPP_NAMESPACE_BEGIN

class DOMElement;

XERCES_CPP_NAMESPACE_END

namespace DocxFactory
{
	using namespace std;

	class DocxCompilerStyleFile;
	class DocxCompilerStyle
	{
	public:
		enum StyleType
		{
			UNDEFINED_STYLE,
			CHARACTER_STYLE,
			NUMBERING_STYLE,
			PARAGRAPH_STYLE,
			TABLE_STYLE
		};

		DocxCompilerStyle(
			StyleType				p_type,
			xercesc::DOMElement*	p_styleNode,
			string					p_styleId,
			string					p_basedOn );

		virtual ~DocxCompilerStyle();

		StyleType				getType() const;
		xercesc::DOMElement*	getStyleNode() const;
		string					getStyleId() const;
		string					getBasedOn() const;

	protected:
		StyleType				m_type;
		xercesc::DOMElement*	m_styleNode;
		string					m_styleId;
		string					m_basedOn;

	private:
		DocxCompilerStyle( const DocxCompilerStyle& p_other );
		DocxCompilerStyle& operator = ( const DocxCompilerStyle& p_other );

	};
};

#endif
