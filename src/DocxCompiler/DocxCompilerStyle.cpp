
#include "DocxFactory/DocxCompiler/DocxCompilerFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerStyleFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerStyle.h"

using namespace DocxFactory;
using namespace std;



DocxCompilerStyle::DocxCompilerStyle( 
	StyleType				p_type,
	xercesc::DOMElement*	p_styleNode,
	string					p_styleId,
	string					p_basedOn )
{
	m_type		= p_type;
	m_styleNode	= p_styleNode;
	m_styleId	= p_styleId;
	m_basedOn	= p_basedOn;
} // c'tor

DocxCompilerStyle::~DocxCompilerStyle()
{

} // d'tor



DocxCompilerStyle::StyleType DocxCompilerStyle::getType() const
{
	return m_type;
} // getType

xercesc::DOMElement* DocxCompilerStyle::getStyleNode() const
{
	return m_styleNode;
} // getStyleNode

string DocxCompilerStyle::getStyleId() const
{
	return m_styleId;
} // getStyleId

string DocxCompilerStyle::getBasedOn() const
{
	return m_basedOn;
} // getBasedOn
