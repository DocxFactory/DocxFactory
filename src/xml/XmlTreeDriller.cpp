
#include "DocxFactory/xml/XmlTreeDriller.h"
#include "DocxFactory/xml/XmlFunc.h"

#include "DocxFactory/str/StrFunc.h"

#include "xercesc/dom/DOM.hpp"

using namespace DocxFactory;
using namespace std;



XmlTreeDriller::XmlTreeDriller( xercesc::DOMNode* p_rootNode,

	const string& p_drillTagNsList,	const string& p_drillTagList,
	const string& p_tagNsList,		const string& p_tagList,
	const string& p_attrNsList,		const string& p_attrList, const string& p_attrValueList )
{
	// when the driller is created the current node is positioned before the root node
	// the first time the nextNode() is called the search starts from the root node
	// and returns the first node matching the filter.
	m_rootNode		= p_rootNode;
	m_cursorNode	= NULL;
	m_cursorPos		= CURSOR_NODE;

	m_drillTagNsAll = p_drillTagNsList.empty();
	m_drillTagAll	= p_drillTagList.empty();
	m_tagNsAll		= p_tagNsList.empty();
	m_tagAll		= p_tagList.empty();
	m_attrNsAll		= p_attrNsList.empty();
	m_attrAll		= p_attrList.empty();
	m_attrValueAll	= p_attrValueList.empty();

	if ( !m_drillTagNsAll )	StrFunc::strToSet( p_drillTagNsList,	m_drillTagNsList );
	if ( !m_drillTagAll )	StrFunc::strToSet( p_drillTagList,		m_drillTagList );
	if ( !m_tagNsAll )		StrFunc::strToSet( p_tagNsList,			m_tagNsList );
	if ( !m_tagAll )		StrFunc::strToSet( p_tagList,			m_tagList );
	if ( !m_attrNsAll )		StrFunc::strToSet( p_attrNsList,		m_attrNsList );
	if ( !m_attrAll )		StrFunc::strToSet( p_attrList,			m_attrList );
	if ( !m_attrValueAll )	StrFunc::strToSet( p_attrValueList,		m_attrValueList );
} // c'tor

XmlTreeDriller::~XmlTreeDriller()
{

} // d'tor



xercesc::DOMNode* XmlTreeDriller::nextNode()
{
	string				l_nodeNs;
	string				l_nodeName;

	xercesc::DOMAttr*	l_attrNode;
	string				l_attrNs;
	string				l_attrName;
	string				l_attrValue;

	XMLSize_t			l_len;
	XMLSize_t			l_pos;

	while ( true )
	{
		if ( !m_cursorNode )
			m_cursorNode = m_rootNode;

		else
		{
			if ( !drillNode() )
				goto _end;
		}

		if ( m_cursorNode ->getNodeType() == xercesc::DOMNode::TEXT_NODE )
		{
			if ( m_tagList.find( "%text%" ) != m_tagList.end() )
				goto _end;
		}

		else
		if ( m_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE )
		{
			l_nodeNs	= XmlFunc::XMLChToUtf8( m_cursorNode ->getNamespaceURI() );
			l_nodeName	= XmlFunc::XMLChToUtf8( l_nodeNs.empty() ? m_cursorNode ->getNodeName() : m_cursorNode ->getLocalName() );

			if ( ( m_tagNsAll	|| m_tagNsList.find( l_nodeNs ) != m_tagNsList.end() )
			  && ( m_tagAll		|| m_tagList.find( l_nodeName ) != m_tagList.end() ) )
			{
				if ( m_attrNsAll
				  && m_attrAll
				  && m_attrValueAll )
					goto _end;

				else
				{
					l_len = m_cursorNode ->getAttributes() ->getLength();
					for ( l_pos = 0; l_pos < l_len; ++l_pos )
					{
						l_attrNode	= ( xercesc::DOMAttr* ) m_cursorNode ->getAttributes() ->item( l_pos );
						l_attrNs	= XmlFunc::XMLChToUtf8( l_attrNode ->getNamespaceURI() );
						l_attrName	= XmlFunc::XMLChToUtf8( l_attrNs.empty() ? l_attrNode ->getNodeName() : l_attrNode ->getLocalName() );
						l_attrValue	= XmlFunc::XMLChToUtf8( l_attrNode ->getNodeValue() );

						if ( ( m_attrNsAll		|| m_attrNsList.find( l_attrNs )		!= m_attrNsList.end() )
						  && ( m_attrAll		|| m_attrList.find( l_attrName )		!= m_attrList.end() )
						  && ( m_attrValueAll	|| m_attrValueList.find( l_attrValue )	!= m_attrValueList.end() ) )
						{
							goto _end;
						}
					}
				}
			}
		}
 	}

	_end:

	return m_cursorNode;
} // nextNode



void XmlTreeDriller::deleteNode()
{
	if ( m_cursorNode != m_rootNode )
	{
		xercesc::DOMNode* l_node = m_cursorNode;

		m_cursorNode = XmlFunc::getPrevNode( m_cursorNode );

		l_node ->getParentNode() ->removeChild( l_node );
	}
} // deleteNode

void XmlTreeDriller::setCurrentNode( xercesc::DOMNode* p_cursorNode, CursorPos p_cursorPos /* = CURSOR_NODE */ )
{
	m_cursorNode	= p_cursorNode;
	m_cursorPos		= p_cursorPos;
} // setCurrentNode




xercesc::DOMNode* XmlTreeDriller::getCurrentNode() const
{
	return m_cursorNode;
} // getCurrentNode


xercesc::DOMNode* XmlTreeDriller::getRoot() const
{
	return m_rootNode;
} // getRoot



xercesc::DOMNode* XmlTreeDriller::drillNode()
{
	xercesc::DOMNode* l_node;

	if( m_cursorPos == CURSOR_BEFORE )
		m_cursorPos	 = CURSOR_NODE;

	else
	if( m_cursorPos == CURSOR_AFTER )
	{
		m_cursorNode = m_cursorNode ->getNextSibling();
		m_cursorPos	 = CURSOR_NODE;
	}

	else
	{
		string	l_nodeNs	= XmlFunc::XMLChToUtf8( m_cursorNode ->getNamespaceURI() );
		string	l_nodeName	= XmlFunc::XMLChToUtf8( l_nodeNs.empty() ? m_cursorNode ->getNodeName() : m_cursorNode ->getLocalName() );

		if ( m_cursorNode ->hasChildNodes() 

		  && ( m_cursorNode == m_rootNode

			|| ( m_drillTagNsAll	|| m_drillTagNsList.find( l_nodeNs ) != m_drillTagNsList.end() )
			&& ( m_drillTagAll		|| m_drillTagList.find( l_nodeName ) != m_drillTagList.end() ) ) )

				m_cursorNode = m_cursorNode ->getFirstChild();

		else
		{
			while ( m_cursorNode != m_rootNode )
			{
				l_node = m_cursorNode ->getNextSibling();

				if ( l_node )
				{
					m_cursorNode = l_node;
					break;
				}

				m_cursorNode = m_cursorNode ->getParentNode();
			}

			if ( m_cursorNode == m_rootNode )
				 m_cursorNode = NULL;
		}
	}

	return m_cursorNode;
} // drillNode
