
#include "DocxFactory/DocxCompiler/DocxCompilerItemFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerBookmark.h"

#include "DocxFactory/xml/XmlFunc.h"
#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include "xercesc/dom/DOM.hpp"

#include <map>
#include <climits>

using namespace DocxFactory;
using namespace std;



DocxCompilerBookmark::DocxCompilerBookmark(
	DocxCompilerItemFile*	p_itemFile,
	DocxCompilerBookmark*	p_parent,
	const string&			p_name,
	size_t					p_id,
	xercesc::DOMElement*	p_startNode )
{
	m_itemFile	= p_itemFile;
	m_parent	= p_parent;
	m_name		= p_name;
	m_id		= p_id;
	m_startNode	= p_startNode;
	m_endNode	= NULL;
} // c'tor

DocxCompilerBookmark::~DocxCompilerBookmark()
{
	list<DocxCompilerBookmark*>::iterator l_bookmarkIterator;
	FOR_EACH( l_bookmarkIterator, &m_children )
	{
		delete *l_bookmarkIterator;
	}
} // d'tor



void DocxCompilerBookmark::normalize()
{
	vector<xercesc::DOMNode*>	l_startNodePath;
	vector<xercesc::DOMNode*>	l_endNodePath;

	xercesc::DOMElement*	l_bodyNode;
	xercesc::DOMNode*		l_parentNode;
	xercesc::DOMNode*		l_containerNode;
	xercesc::DOMNode*		l_containerNodeTemp;
	xercesc::DOMNode*		l_commonParentNode;
	xercesc::DOMNode*		l_prNode;
	xercesc::DOMNode*		l_prNodeTemp;
	xercesc::DOMNode*		l_prNode2;
	xercesc::DOMNode*		l_prNodeTemp2;
	xercesc::DOMNode*		l_childNode;

	size_t	l_commonParent;
	size_t	l_parent;
	size_t	l_pos;
	bool	l_ok;
	
	l_bodyNode		= m_itemFile ->getBodyNode();
	l_parentNode	= m_startNode ->getParentNode();
	
	while ( true )
	{
		l_startNodePath.insert( l_startNodePath.begin(), l_parentNode );

		if ( l_parentNode == l_bodyNode )
			break;

		l_parentNode = l_parentNode ->getParentNode();
	}

	l_parentNode = m_endNode ->getParentNode();

	while ( true )
	{
		l_endNodePath.insert( l_endNodePath.begin(), l_parentNode );

		if ( l_parentNode == l_bodyNode )
			break;

		l_parentNode = l_parentNode ->getParentNode();
	}

	l_commonParent = 0;

	while ( l_commonParent + 1 < l_startNodePath.size()
		 && l_commonParent + 1 < l_endNodePath.size() 
		 && l_startNodePath[ l_commonParent + 1 ] == l_endNodePath[ l_commonParent + 1 ] )
	{
		l_commonParent++;
	}
	
	// if the common parent is p it means the part holds r's.
	// if we wanted to use altChunk we would need to replace the entire p.
	// so the smallest part possible is a p.

	if ( XmlFunc::XMLChCmp( l_startNodePath[ l_commonParent ] ->getLocalName(), _X( "p" ) )

	// except for boolean fields which can be an r's.
	 && !( m_name.length() > 7 && m_name.substr( m_name.length() - 7 ) == "_notset"
	    || m_name.length() > 4 && m_name.substr( m_name.length() - 4 ) == "_yes"
		|| m_name.length() > 3 && m_name.substr( m_name.length() - 3 ) == "_no" ) )

		l_commonParent--;

	// if the common parent is tr it means the part holds tc's.
	// if we cut/paste tc's different rows may not have the same number of tc's.
	// so it is only possible to cut/paste whole rows.
	
	if ( XmlFunc::XMLChCmp( l_startNodePath[ l_commonParent ] -> getLocalName(), _X( "tr" ) ) )
		l_commonParent--;

	l_commonParentNode = l_startNodePath[ l_commonParent ];



	if ( m_startNode ->getParentNode() != l_commonParentNode )
	{
		l_parent = l_startNodePath.size() - 1;

		while ( true )
		{
			l_containerNode	= m_startNode ->getParentNode();
			l_parentNode	= l_containerNode ->getParentNode();

			if ( l_containerNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
			&& ( XmlFunc::XMLChCmp( l_containerNode ->getLocalName(),		_X( "tbl" ) )
			  || XmlFunc::XMLChCmp( l_containerNode ->getLocalName(),		_X( "tr" ) ) )
			  && XmlFunc::XMLChCmp( l_containerNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
			{
				l_prNode	= l_containerNode ->getFirstChild();
				l_prNode2	= l_prNode ->getNextSibling();

				l_ok = false;

				if ( m_startNode == l_prNode2 ->getNextSibling() )
					 l_ok = true;

				else
				{
					for ( l_pos = l_commonParent + 1; l_pos <= l_parent; l_pos++ )
					{
						if ( XmlFunc::XMLChCmp( l_startNodePath[ l_pos ] ->getLocalName(), _X( "tr" ) )
						  || XmlFunc::XMLChCmp( l_startNodePath[ l_pos ] ->getLocalName(), _X( "rect" ) ) 
						  || XmlFunc::XMLChCmp( l_startNodePath[ l_pos ] ->getLocalName(), _X( "pict" ) )
						  || XmlFunc::XMLChCmp( l_startNodePath[ l_pos ] ->getLocalName(), _X( "drawing" ) ) )
						{
							l_ok = true;
							break;
						}
					}
				}

				if ( l_ok )
				{
					l_containerNode ->removeChild( m_startNode );
					l_parentNode ->insertBefore( m_startNode, l_containerNode );
				}

				else
				{
					l_containerNodeTemp = l_containerNode ->cloneNode( false );
					l_parentNode ->insertBefore( l_containerNodeTemp, l_containerNode );

					l_prNodeTemp = l_prNode ->cloneNode( true );
					l_containerNodeTemp ->appendChild( l_prNodeTemp );

					l_prNodeTemp2 = l_prNode2 ->cloneNode( true );
					l_containerNodeTemp ->appendChild( l_prNodeTemp2 );	

					l_childNode = m_startNode ->getPreviousSibling();

					while ( l_childNode != l_prNode2 )
					{
						l_containerNode ->removeChild( l_childNode );

						if( l_containerNodeTemp ->getLastChild() == l_prNodeTemp2 )
							l_containerNodeTemp ->appendChild( l_childNode );

						else
							l_containerNodeTemp ->insertBefore( l_childNode, l_prNodeTemp2 ->getNextSibling() );

						l_childNode = m_startNode ->getPreviousSibling();
					}

					l_containerNode ->removeChild( m_startNode );
					l_parentNode ->insertBefore( m_startNode, l_containerNode );
				}
			}

			else
			if ( l_containerNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
			&& ( XmlFunc::XMLChCmp( l_containerNode ->getLocalName(),		_X( "tc" ) )
			  || XmlFunc::XMLChCmp( l_containerNode ->getLocalName(),		_X( "p" ) )
			  || XmlFunc::XMLChCmp( l_containerNode ->getLocalName(),		_X( "r" ) ) )
			  && XmlFunc::XMLChCmp( l_containerNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
			{
				l_prNode = l_containerNode ->getFirstChild();
				
				l_ok = false;

				if ( m_startNode == l_prNode ->getNextSibling() )
					l_ok = true;

				else
				{
					for ( l_pos = l_commonParent + 1; l_pos <= l_parent; l_pos++ )
					{
						if ( XmlFunc::XMLChCmp( l_startNodePath[ l_pos ] ->getLocalName(), _X( "tr" ) )
						  || XmlFunc::XMLChCmp( l_startNodePath[ l_pos ] ->getLocalName(), _X( "rect" ) ) 
						  || XmlFunc::XMLChCmp( l_startNodePath[ l_pos ] ->getLocalName(), _X( "pict" ) )
						  || XmlFunc::XMLChCmp( l_startNodePath[ l_pos ] ->getLocalName(), _X( "drawing" ) ) )
						{
							l_ok = true;
							break;
						}
					}
				}

				if ( l_ok )
				{
					l_containerNode ->removeChild( m_startNode );
					l_parentNode ->insertBefore( m_startNode, l_containerNode );
				}

				else
				{
					l_containerNodeTemp = ( l_containerNode ->cloneNode( false ) );
					l_parentNode ->insertBefore( l_containerNodeTemp, l_containerNode );

					l_prNodeTemp = ( l_prNode ->cloneNode( true ) );
					l_containerNodeTemp ->appendChild( l_prNodeTemp );

					l_childNode = m_startNode ->getPreviousSibling();

					while ( l_childNode != l_prNode )
					{
						l_containerNode ->removeChild( l_childNode );

						if( l_containerNodeTemp ->getLastChild() == l_prNodeTemp )
							l_containerNodeTemp ->appendChild( l_childNode );

						else
							l_containerNodeTemp ->insertBefore( l_childNode, l_prNodeTemp ->getNextSibling() );

						l_childNode = m_startNode ->getPreviousSibling();
					}

					l_containerNode ->removeChild( m_startNode );
					l_parentNode ->insertBefore( m_startNode, l_containerNode );
				}
			}

			else
			{
				l_containerNode ->removeChild( m_startNode );
				l_parentNode ->insertBefore( m_startNode, l_containerNode );
			}

			l_parent--;

			if ( l_parentNode == l_commonParentNode )
				break;
		}
	}



	if ( m_endNode ->getParentNode() != l_commonParentNode )
	{
		l_parent = l_endNodePath.size() - 1;

		while ( true )
		{
			l_containerNode	= m_endNode ->getParentNode();
			l_parentNode	= l_containerNode ->getParentNode();

			if ( l_containerNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
			&& ( XmlFunc::XMLChCmp( l_containerNode ->getLocalName(),		_X( "tbl" ) )
			  || XmlFunc::XMLChCmp( l_containerNode ->getLocalName(),		_X( "tr" ) ) )
			  && XmlFunc::XMLChCmp( l_containerNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
			{
				l_prNode	= l_containerNode ->getFirstChild();
				l_prNode2	= l_prNode ->getNextSibling();

				l_ok = false;

				if ( m_endNode == l_containerNode ->getLastChild() ) 
					l_ok = true;

				else
				{
					for ( l_pos = l_commonParent + 1; l_pos <= l_parent; l_pos++ )
					{
						if ( XmlFunc::XMLChCmp( l_endNodePath[ l_pos ] ->getLocalName(), _X( "tr" ) )
						  || XmlFunc::XMLChCmp( l_endNodePath[ l_pos ] ->getLocalName(), _X( "rect" ) ) 
						  || XmlFunc::XMLChCmp( l_endNodePath[ l_pos ] ->getLocalName(), _X( "pict" ) )
						  || XmlFunc::XMLChCmp( l_endNodePath[ l_pos ] ->getLocalName(), _X( "drawing" ) ) )
						{
							l_ok = true;
							break;
						}
					}
				}

				if ( l_ok )
				{
					l_containerNode ->removeChild( m_endNode );

					if( l_containerNode == l_parentNode ->getLastChild() )
						l_parentNode ->appendChild( m_endNode );

					else
						l_parentNode ->insertBefore( m_endNode, l_containerNode ->getNextSibling() );
				}

				else
				{
					l_containerNodeTemp = l_containerNode ->cloneNode( false );
					l_parentNode ->insertBefore( l_containerNodeTemp, l_containerNode );

					l_prNodeTemp = l_prNode ->cloneNode( true );
					l_containerNodeTemp ->appendChild( l_prNodeTemp );

					l_prNodeTemp2 = l_prNode2 ->cloneNode( true );
					l_containerNodeTemp ->appendChild( l_prNodeTemp2 );	

					l_childNode = m_endNode ->getPreviousSibling();

					while ( l_childNode != l_prNode2 )
					{
						l_containerNode ->removeChild( l_childNode );

						if( l_containerNodeTemp ->getLastChild() == l_prNodeTemp2 )
							l_containerNodeTemp ->appendChild( l_childNode );

						else
							l_containerNodeTemp ->insertBefore( l_childNode, l_prNodeTemp2 ->getNextSibling() );

						l_childNode = m_endNode ->getPreviousSibling();
					}

					l_containerNode ->removeChild( m_endNode );
					l_parentNode ->insertBefore( m_endNode, l_containerNode );
				}
			}

			else
			if ( l_containerNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
			&& ( XmlFunc::XMLChCmp( l_containerNode ->getLocalName(),		_X( "tc" ) )
			  || XmlFunc::XMLChCmp( l_containerNode ->getLocalName(),		_X( "p" ) )
			  || XmlFunc::XMLChCmp( l_containerNode ->getLocalName(),		_X( "r" ) ) )
			  && XmlFunc::XMLChCmp( l_containerNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
			{
				l_prNode = l_containerNode ->getFirstChild();

				l_ok = false;

				if ( m_endNode == l_containerNode ->getLastChild() )
					l_ok = true;

				else
				{
					for ( l_pos = l_commonParent + 1; l_pos <= l_parent; l_pos++ )
					{
						if ( XmlFunc::XMLChCmp( l_endNodePath[ l_pos ] ->getLocalName(), _X( "tr" ) )
						  || XmlFunc::XMLChCmp( l_endNodePath[ l_pos ] ->getLocalName(), _X( "rect" ) ) 
						  || XmlFunc::XMLChCmp( l_endNodePath[ l_pos ] ->getLocalName(), _X( "pict" ) )
						  || XmlFunc::XMLChCmp( l_endNodePath[ l_pos ] ->getLocalName(), _X( "drawing" ) ) )
						{
							l_ok = true;
							break;
						}
					}
				}

				if ( l_ok )
				{
					l_containerNode ->removeChild( m_endNode );

					if( l_containerNode == l_parentNode ->getLastChild() )
						l_parentNode ->appendChild( m_endNode );

					else
						l_parentNode ->insertBefore( m_endNode, l_containerNode ->getNextSibling() );
				}

				else
				{
					l_containerNodeTemp = l_containerNode ->cloneNode( false );
					l_parentNode ->insertBefore( l_containerNodeTemp, l_containerNode );

					l_prNodeTemp = l_prNode ->cloneNode( true );
					l_containerNodeTemp ->appendChild( l_prNodeTemp );

					l_childNode = m_endNode ->getPreviousSibling();

					while ( l_childNode != l_prNode )
					{
						l_containerNode ->removeChild( l_childNode );

						if( l_containerNodeTemp ->getLastChild() == l_prNodeTemp )
							l_containerNodeTemp ->appendChild( l_childNode );

						else
							l_containerNodeTemp ->insertBefore( l_childNode, l_prNodeTemp ->getNextSibling() );

						l_childNode = m_endNode ->getPreviousSibling();
					}

					l_containerNode ->removeChild( m_endNode );
					l_parentNode ->insertBefore( m_endNode, l_containerNode );
				}
			}

			else
			{
				l_containerNode ->removeChild( m_endNode );

				if( l_containerNode == l_parentNode ->getLastChild() )
					l_parentNode ->appendChild( m_endNode );

				else
					l_parentNode ->insertBefore( m_endNode, l_containerNode ->getNextSibling() );
			}

			l_parent--;

			if ( l_parentNode == l_commonParentNode )
				break;
		}
	}



	/***
	l_containerNode = m_startNode ->getParentNode();

	if ( l_containerNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
	  && XmlFunc::XMLChCmp( l_containerNode ->getLocalName(),		_X( "tbl" ) )
	  && XmlFunc::XMLChCmp( l_containerNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
	{
		l_prNode	= l_containerNode ->getFirstChild();
		l_prNode2	= l_prNode ->getNextSibling();

		l_ok = false;

		l_childNode = m_startNode ->getPreviousSibling();

		while ( l_childNode != l_prNode2 )
		{
			if ( l_childNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
			  && XmlFunc::XMLChCmp( l_childNode ->getLocalName(),		_X( "tr" ) )
			  && XmlFunc::XMLChCmp( l_childNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
			{
				l_ok = true;
				break;
			}

			l_childNode = l_childNode ->getPreviousSibling();
		}

		if ( !l_ok )
		{
			l_childNode = m_endNode ->getNextSibling();

			while ( l_childNode )
			{
				if ( l_childNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
				  && XmlFunc::XMLChCmp( l_childNode ->getLocalName(),		_X( "tr" ) )
				  && XmlFunc::XMLChCmp( l_childNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
				{
					l_ok = true;
					break;
				}

				l_childNode = l_childNode ->getNextSibling();
			}

			if ( !l_ok )
			{
				l_parentNode = l_containerNode ->getParentNode();

				l_containerNode ->removeChild( m_startNode );
				l_parentNode ->insertBefore( m_startNode, l_containerNode );

				l_containerNode ->removeChild( m_endNode );

				if ( l_containerNode == l_parentNode ->getLastChild() )
					 l_parentNode ->appendChild( m_endNode );
					
				else
					l_parentNode ->insertBefore( m_endNode, l_containerNode ->getNextSibling() );
			}
		}
	}
	***/
} // normalize



void DocxCompilerBookmark::setGroupId( size_t p_groupId )
{
	m_groupId = p_groupId;
} // setGroupId

void DocxCompilerBookmark::setEndNode( xercesc::DOMElement* p_node )
{
	m_endNode = p_node;
} // setEndNode

void DocxCompilerBookmark::insertChild( DocxCompilerBookmark* p_bookmark )
{
	m_children.push_back( p_bookmark );
} // insertChild



string DocxCompilerBookmark::getName()const
{
	return m_name;
} // getName

size_t DocxCompilerBookmark::getId() const
{
	return m_id;
} // getId

size_t DocxCompilerBookmark::getGroupId() const
{
	return m_groupId;
} // getGroupId

xercesc::DOMElement* DocxCompilerBookmark::getStartNode() const
{
	return m_startNode;
} // getStartNode

xercesc::DOMElement* DocxCompilerBookmark::getEndNode() const
{
	return m_endNode;
} // getEndNode

DocxCompilerBookmark* DocxCompilerBookmark::getParent() const
{
	return m_parent;
} // getParent

const list<DocxCompilerBookmark*>* DocxCompilerBookmark::getChildren() const
{
	return &m_children;
} // getChildren
