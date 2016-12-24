
#include "DocxFactory/DocxCompiler/DocxCompilerFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerStyleFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerStyle.h"
#include "DocxFactory/DocxCompiler/DocxCompilerTableStyle.h"

using namespace DocxFactory;
using namespace std;



DocxCompilerTableStyle::DocxCompilerTableStyle( 
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
	bool					p_conditionalStyle )

	: DocxCompilerStyle(
		p_type,
		p_styleNode,
		p_styleId,
		p_basedOn )
{
	m_topNode			= p_topNode;
	m_leftNode			= p_leftNode;
	m_bottomNode		= p_bottomNode;
	m_rightNode			= p_rightNode;
	m_insideHNode		= p_insideHNode;
	m_insideVNode		= p_insideVNode;
	m_conditionalStyle	= p_conditionalStyle;
} // c'tor

DocxCompilerTableStyle::~DocxCompilerTableStyle()
{

} // d'tor



void DocxCompilerTableStyle::setTopNode( xercesc::DOMElement* p_node )
{
	m_topNode = p_node;
} // setTopNode

void DocxCompilerTableStyle::setLeftNode( xercesc::DOMElement* p_node )
{
	m_leftNode = p_node;
} // setLeftNode

void DocxCompilerTableStyle::setBottomNode( xercesc::DOMElement* p_node )
{
	m_bottomNode = p_node;
} // setBottomNode

void DocxCompilerTableStyle::setRightNode( xercesc::DOMElement* p_node )
{
	m_rightNode = p_node;
} // setRightNode

void DocxCompilerTableStyle::setInsideHNode( xercesc::DOMElement* p_node )
{
	m_insideHNode = p_node;
} // setInsideHNode

void DocxCompilerTableStyle::setInsideVNode( xercesc::DOMElement* p_node )
{
	m_insideVNode = p_node;
} // setInsideVNode

void DocxCompilerTableStyle::setConditionaStyle( bool p_conditionalStyle )
{
	m_conditionalStyle = p_conditionalStyle;
} // setConditionalStyle



xercesc::DOMElement* DocxCompilerTableStyle::getTopNode() const
{
	return m_topNode;
} // getTopNode

xercesc::DOMElement* DocxCompilerTableStyle::getLeftNode() const
{
	return m_leftNode;
} // getLeftNode

xercesc::DOMElement* DocxCompilerTableStyle::getBottomNode() const
{
	return m_bottomNode;
} // getBottomNode

xercesc::DOMElement* DocxCompilerTableStyle::getRightNode() const
{
	return m_rightNode;
} // getRightNode

xercesc::DOMElement* DocxCompilerTableStyle::getInsideHNode() const
{
	return m_insideHNode;
} // getInsideHNode

xercesc::DOMElement* DocxCompilerTableStyle::getInsideVNode() const
{
	return m_insideVNode;
} // getInsideVNode

bool DocxCompilerTableStyle::isConditionalStyle() const
{
	return m_conditionalStyle;
} // isConditionalStyle
