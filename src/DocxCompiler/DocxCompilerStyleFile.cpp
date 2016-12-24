
#include "DocxFactory/DocxCompiler/DocxCompilerFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerStyleFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerStyle.h"
#include "DocxFactory/DocxCompiler/DocxCompilerTableStyle.h"

#include "DocxFactory/opc/OpcFunc.h"
#include "DocxFactory/opc/OpcPackage.h"
#include "DocxFactory/opc/OpcPart.h"
#include "DocxFactory/opc/OpcRelationships.h"
#include "DocxFactory/opc/OpcRelationship.h"
#include "DocxFactory/opc/OpcContentTypes.h"
#include "DocxFactory/opc/PartTypeNotFoundException.h"
#include "DocxFactory/opc/ContentTypeNotFoundException.h"

#include "DocxFactory/xml/XmlFunc.h"
#include "DocxFactory/xml/XmlTreeDriller.h"
#include "DocxFactory/xml/Utf8ToXMLCh.h"
#include "DocxFactory/xml/NodeNotFoundException.h"

#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include "xercesc/dom/DOM.hpp"

#include "boost/scoped_ptr.hpp"

#include <set>

using namespace DocxFactory;
using namespace std;



DocxCompilerStyleFile::DocxCompilerStyleFile( DocxCompilerFile* p_file )
{
	m_file			= p_file;
	m_part			= NULL;

	m_stylesNode	= NULL;
	m_wordMlPrefix	= "";
	m_relMlPrefix	= "";
} // c'tor

DocxCompilerStyleFile::~DocxCompilerStyleFile()
{
	map<string, DocxCompilerTableStyle*>::iterator l_tableStyleIterator;
	FOR_EACH( l_tableStyleIterator, &m_tableStyles )
	{
		delete l_tableStyleIterator ->second;
	}
} // d'tor



void DocxCompilerStyleFile::load()
{
	map<OpcPart*, string>::const_iterator	l_overrideIterator;
	const map<OpcPart*, string>*			l_overrides = m_file ->getContentTypes() ->getOverridesByPart();

	FOR_EACH( l_overrideIterator, l_overrides )
	{
		if ( l_overrideIterator ->second == "application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml" )
		{
			m_part = l_overrideIterator ->first;
			break;
		}
	}

	if ( l_overrideIterator == l_overrides ->end() )
		throw ContentTypeNotFoundException( "application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml", __FILE__, __LINE__ );

	m_part ->loadDoc();

	m_stylesNode	= m_part ->getDoc() ->getDocumentElement();
	m_wordMlPrefix	= XmlFunc::normalizeNsPrefix( m_stylesNode, "w", "http://schemas.openxmlformats.org/wordprocessingml/2006/main" );
	m_relMlPrefix	= XmlFunc::normalizeNsPrefix( m_stylesNode, "r", "http://schemas.openxmlformats.org/officeDocument/2006/relationships" );

	if ( !XmlFunc::XMLChCmp( m_stylesNode ->getLocalName(),		_X( "styles" ) )
	  || !XmlFunc::XMLChCmp( m_stylesNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

		throw NodeNotFoundException( "styles", __FILE__, __LINE__ );

	normalize();

	loadStyles();
} // load

void DocxCompilerStyleFile::loadStyles()
{
	DocxCompilerTableStyle*			l_tableStyle;
	DocxCompilerStyle::StyleType	l_type;
	xercesc::DOMElement*			l_styleNode;
	string							l_styleId;
	string							l_basedOn;

	xercesc::DOMElement*			l_tblPrNode;
	xercesc::DOMElement*			l_tblBordersNode;
	xercesc::DOMElement*			l_topNode;
	xercesc::DOMElement*			l_leftNode;
	xercesc::DOMElement*			l_bottomNode;
	xercesc::DOMElement*			l_rightNode;
	xercesc::DOMElement*			l_insideHNode;
	xercesc::DOMElement*			l_insideVNode;
	xercesc::DOMElement*			l_tblStylePrNode;

	xercesc::DOMDocument*			l_doc = m_part ->getDoc();
	xercesc::DOMNode*				l_node;
	string							l_str;

	l_styleNode = NULL;

	while ( l_styleNode = XmlFunc::getChildByTagName(
		m_stylesNode,
		l_styleNode,
		"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
		"style" ) )
	{
		l_type		= DocxCompilerStyle::UNDEFINED_STYLE;
		l_styleId	= StrFunc::lc( XmlFunc::XMLChToUtf8( l_styleNode ->getAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":styleId" ) ) ) );
		l_basedOn	= "";

		l_str = StrFunc::lc( XmlFunc::XMLChToUtf8( l_styleNode ->getAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":type" ) ) ) );

			 if ( l_str == "character" )	l_type = DocxCompilerStyle::CHARACTER_STYLE;
		else if ( l_str == "numbering" )	l_type = DocxCompilerStyle::NUMBERING_STYLE;
		else if ( l_str == "paragraph" )	l_type = DocxCompilerStyle::PARAGRAPH_STYLE;
		else if ( l_str == "table" )		l_type = DocxCompilerStyle::TABLE_STYLE;

		l_node = XmlFunc::getChildByTagName(
			l_styleNode, 
			NULL,
			"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
			"basedOn" );

		if( l_node )
			l_basedOn = StrFunc::lc( XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_node ) ->getAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":val" ) ) ) );



		if ( l_type == DocxCompilerStyle::TABLE_STYLE )
		{
			l_topNode			= NULL;
			l_leftNode			= NULL;
			l_bottomNode		= NULL;
			l_rightNode			= NULL;
			l_insideHNode		= NULL;
			l_insideVNode		= NULL;
			l_tblStylePrNode	= NULL;

			l_tblPrNode = XmlFunc::getChildByTagName(
				l_styleNode,
				NULL,
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				"tblPr" );

			if( l_tblPrNode )
			{
				l_tblBordersNode = XmlFunc::getChildByTagName(
					l_tblPrNode,
					NULL,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"tblBorders" );

				if( l_tblBordersNode )
				{
					l_node = l_tblBordersNode ->getFirstChild();

					while ( l_node )
					{
						if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
						  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "top" ) )
						  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

							l_topNode = ( xercesc::DOMElement* ) l_node;

						else
						if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
						  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "left" ) )
						  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

							l_leftNode = ( xercesc::DOMElement* ) l_node;

						else
						if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
						  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "bottom" ) )
						  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

							l_bottomNode = ( xercesc::DOMElement* ) l_node;

						else
						if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
						  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "right" ) )
						  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

							l_rightNode = ( xercesc::DOMElement* ) l_node;

						else
						if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
						  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "insideH" ) )
						  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

							l_insideHNode = ( xercesc::DOMElement* ) l_node;

						else
						if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
						  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "insideV" ) )
						  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

							l_insideVNode = ( xercesc::DOMElement* ) l_node;

						l_node = l_node ->getNextSibling();
					}
				}
			}

			l_tblStylePrNode = XmlFunc::getChildByTagName(
				l_styleNode,
				NULL,
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				"tblStylePr" );



			l_tableStyle = new DocxCompilerTableStyle(
				l_type,
				l_styleNode,
				l_styleId,
				l_basedOn,

				l_topNode,
				l_leftNode,
				l_bottomNode,
				l_rightNode,
				l_insideHNode,
				l_insideVNode,
			  ( l_tblStylePrNode != NULL ) );

			m_tableStyles.insert( make_pair( l_styleId, l_tableStyle ) );
		}
	}

	normalizeStyles();
} // loadStyles



void DocxCompilerStyleFile::normalize()
{
	m_part ->setChangeStatus( OpcPart::PART_CHANGED_IN_DOM );

	boost::scoped_ptr<XmlTreeDriller> l_treeDriller( new XmlTreeDriller(

		m_stylesNode,
		"http://schemas.openxmlformats.org/wordprocessingml/2006/main", "style",
		"http://schemas.openxmlformats.org/wordprocessingml/2006/main", "rsid",
		"", "", "" ) );

	while ( l_treeDriller ->nextNode() )
	{
		l_treeDriller ->deleteNode();
	}
} // load

void DocxCompilerStyleFile::normalizeStyles()
{
	map<string, DocxCompilerTableStyle*>::iterator	l_tableStyleIterator;
	DocxCompilerTableStyle*							l_tableStyle;

	map<string, DocxCompilerTableStyle*>::iterator	l_basedOnStyleIterator;
	DocxCompilerTableStyle*							l_basedOnStyle;

	map<string, DocxCompilerTableStyle*>			l_basedOnList;
	map<string, DocxCompilerTableStyle*>			l_tableStyleList;

	FOR_EACH( l_tableStyleIterator, &m_tableStyles )
	{
		l_tableStyle = l_tableStyleIterator ->second;

		if( l_tableStyle ->getBasedOn().empty() )
			l_basedOnList.insert( make_pair( l_tableStyle ->getStyleId(), l_tableStyle ) );
	}

	while ( !l_basedOnList.empty() )
	{
		FOR_EACH( l_tableStyleIterator, &m_tableStyles )
		{
			l_tableStyle = l_tableStyleIterator ->second;

			l_basedOnStyleIterator = l_basedOnList.find( l_tableStyle ->getBasedOn() );
			if( l_basedOnStyleIterator != l_basedOnList.end() )
			{
				l_basedOnStyle = l_basedOnStyleIterator ->second;

				if(!l_tableStyle ->getTopNode() && l_basedOnStyle ->getTopNode() )
					l_tableStyle ->setTopNode( l_basedOnStyle ->getTopNode() );

				if(!l_tableStyle ->getLeftNode() && l_basedOnStyle ->getLeftNode() )
					l_tableStyle ->setLeftNode( l_basedOnStyle ->getLeftNode() );

				if(!l_tableStyle ->getBottomNode() && l_basedOnStyle ->getBottomNode() )
					l_tableStyle ->setBottomNode( l_basedOnStyle ->getBottomNode() );

				if(!l_tableStyle ->getRightNode() && l_basedOnStyle ->getRightNode() )
					l_tableStyle ->setRightNode( l_basedOnStyle ->getRightNode() );

				if(!l_tableStyle ->getInsideHNode() && l_basedOnStyle ->getInsideHNode() )
					l_tableStyle ->setInsideHNode( l_basedOnStyle ->getInsideHNode() );

				if(!l_tableStyle ->getInsideVNode() && l_basedOnStyle ->getInsideVNode() )
					l_tableStyle ->setInsideVNode( l_basedOnStyle ->getInsideVNode() );

				if(!l_tableStyle ->isConditionalStyle() && l_basedOnStyle ->isConditionalStyle() )
					l_tableStyle ->setConditionaStyle( l_basedOnStyle ->isConditionalStyle() );

				l_tableStyleList.insert( make_pair( l_tableStyle ->getStyleId(), l_tableStyle ) );
			}
		}

		l_basedOnList = l_tableStyleList;
		l_tableStyleList.clear();
	}
} // normalizeStyles



DocxCompilerFile* DocxCompilerStyleFile::getFile() const
{
	return m_file;
} // getFile

OpcPart* DocxCompilerStyleFile::getPart() const
{
	return m_part;
} // getPart

xercesc::DOMElement* DocxCompilerStyleFile::getStylesNode() const
{
	return m_stylesNode;
} // getStylesNode

string DocxCompilerStyleFile::getWordMlPrefix() const
{
	return m_wordMlPrefix;
} // getWordMlPrefix

string DocxCompilerStyleFile::getRelMlPrefix() const
{
	return m_relMlPrefix;
} // getRelMlPrefix

const map<string, DocxCompilerTableStyle*>*	DocxCompilerStyleFile::getTableStyles() const
{
	return &m_tableStyles;
} // getTableStyles
