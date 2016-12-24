
#ifndef __DOCXFACTORY_DOCX_COMPILER_TABLE_STYLE_H__
#define __DOCXFACTORY_DOCX_COMPILER_TABLE_STYLE_H__

#include "DocxFactory/DocxCompiler/DocxCompilerStyle.h"



namespace DocxFactory
{
	using namespace std;

	class DocxCompilerTableStyle : public DocxCompilerStyle
	{
	public:
		DocxCompilerTableStyle(
			StyleType				p_type,
			xercesc::DOMElement*	p_styleNode,
			string					p_styleId,
			string					p_basedOn,
			
			xercesc::DOMElement*	p_topNode,
			xercesc::DOMElement*	p_leftNode,
			xercesc::DOMElement*	p_bottomNode,
			xercesc::DOMElement*	p_rightNode,
			xercesc::DOMElement*	p_insideHNode,
			xercesc::DOMElement*	p_insideVNode,
			bool					p_conditionalStyle );

		virtual ~DocxCompilerTableStyle();

		void setTopNode			( xercesc::DOMElement* p_node );
		void setLeftNode		( xercesc::DOMElement* p_node );
		void setBottomNode		( xercesc::DOMElement* p_node );
		void setRightNode		( xercesc::DOMElement* p_node );
		void setInsideHNode		( xercesc::DOMElement* p_node );
		void setInsideVNode		( xercesc::DOMElement* p_node );
		void setConditionaStyle	( bool p_conditionalStyle );

		xercesc::DOMElement*	getTopNode() const;
		xercesc::DOMElement*	getLeftNode() const;
		xercesc::DOMElement*	getBottomNode() const;
		xercesc::DOMElement*	getRightNode() const;
		xercesc::DOMElement*	getInsideHNode() const;
		xercesc::DOMElement*	getInsideVNode() const;
		bool					isConditionalStyle() const;

	protected:

	private:
		DocxCompilerTableStyle( const DocxCompilerTableStyle& p_other );
		DocxCompilerTableStyle& operator = ( const DocxCompilerTableStyle& p_other );

		xercesc::DOMElement*	m_topNode;
		xercesc::DOMElement*	m_leftNode;
		xercesc::DOMElement*	m_bottomNode;
		xercesc::DOMElement*	m_rightNode;
		xercesc::DOMElement*	m_insideHNode;
		xercesc::DOMElement*	m_insideVNode;
		bool					m_conditionalStyle;

	};
};

#endif
