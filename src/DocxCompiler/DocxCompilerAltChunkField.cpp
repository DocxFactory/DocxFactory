
#include "DocxFactory/DocxCompiler/DocxCompilerItemFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItem.h"
#include "DocxFactory/DocxCompiler/DocxCompilerAltChunkField.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/xml/XmlFunc.h"
#include "DocxFactory/xml/Utf8ToXMLCh.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include "xercesc/dom/DOM.hpp"

using namespace DocxFactory;
using namespace std;



DocxCompilerAltChunkField::DocxCompilerAltChunkField(
	DocxCompilerItem*				p_item,
	const string&					p_name,
	const string&					p_format,
	const map<FieldParam, string>*	p_valueByParam,
	xercesc::DOMElement*			p_placeHolderNode )

	: DocxCompilerField(
		p_item,
		TYPE_ALT_CHUNK,
		p_name,
		p_format,
		p_valueByParam,
		p_placeHolderNode )
{
	xercesc::DOMDocument*	l_doc;
	xercesc::DOMElement*	l_pNode;
	xercesc::DOMElement*	l_pTempNode;
	xercesc::DOMElement*	l_pPrNode;
	xercesc::DOMElement*	l_pPrTempNode;
	xercesc::DOMElement*	l_rNode;
	xercesc::DOMElement*	l_rTempNode;
	xercesc::DOMElement*	l_rPrNode;
	xercesc::DOMElement*	l_rPrTempNode;
	xercesc::DOMElement*	l_tNode;
	xercesc::DOMElement*	l_tTempNode;

	xercesc::DOMNode*		l_parentNode;
	xercesc::DOMNode*		l_node;
	xercesc::DOMNode*		l_tempNode;

	string	l_wordMlPrefix	= p_item ->getItemFile() ->getWordMlPrefix();
	string	l_relMlPrefix	= p_item ->getItemFile() ->getRelMlPrefix();
	string	l_str;

	l_tNode			= ( xercesc::DOMElement* ) p_placeHolderNode ->getParentNode();
	l_rNode			= ( xercesc::DOMElement* ) l_tNode ->getParentNode();
	l_rPrNode		= ( xercesc::DOMElement* ) l_rNode ->getFirstChild();
	l_pNode			= ( xercesc::DOMElement* ) l_rNode ->getParentNode();
	l_pPrNode		= ( xercesc::DOMElement* ) l_pNode ->getFirstChild();
	l_parentNode	= l_pNode ->getParentNode();

	if ( l_tNode ->getChildNodes() ->getLength() == 1 )
	{
		l_tNode ->removeChild( p_placeHolderNode );
		l_rNode	->insertBefore( p_placeHolderNode, l_tNode );
		l_rNode ->removeChild( l_tNode );
	}

	else
	if ( p_placeHolderNode == l_tNode ->getFirstChild() )
	{
		l_tNode ->removeChild( p_placeHolderNode );
		l_rNode ->insertBefore( p_placeHolderNode, l_tNode );
	}

	else
	if ( p_placeHolderNode == l_tNode ->getLastChild() )
	{
		l_tNode ->removeChild( p_placeHolderNode );

		if ( l_tNode == l_rNode ->getLastChild() )
			 l_rNode ->appendChild( p_placeHolderNode );

		else
		{
			l_node = l_tNode ->getNextSibling();
			l_rNode ->insertBefore( p_placeHolderNode, l_node );
		}
	}

	else
	{
		l_tTempNode = ( xercesc::DOMElement* ) l_tNode ->cloneNode( false );
		l_rNode ->insertBefore( l_tTempNode, l_tNode );

		while ( p_placeHolderNode != l_tNode ->getFirstChild() )
		{
			l_node = p_placeHolderNode ->getPreviousSibling();
			l_tNode ->removeChild( l_node );

			if( l_tTempNode ->getFirstChild() )
				l_tTempNode ->insertBefore( l_node, l_tTempNode ->getFirstChild() );
			else
				l_tTempNode ->appendChild( l_node );
		}

		l_tNode ->removeChild( p_placeHolderNode );
		l_rNode ->insertBefore( p_placeHolderNode, l_tNode );
	}



	if ( l_rNode ->getChildNodes() ->getLength() == 2 )
	{
		l_rNode ->removeChild( p_placeHolderNode );
		l_pNode ->insertBefore( p_placeHolderNode, l_rNode );
		l_pNode ->removeChild( l_rNode );
	}

	else
	if ( p_placeHolderNode == l_rNode ->getChildNodes() ->item(1) )
	{
		l_rNode ->removeChild( p_placeHolderNode );
		l_pNode ->insertBefore( p_placeHolderNode, l_rNode );
	}

	else
	if ( p_placeHolderNode == l_rNode ->getLastChild() )
	{
		l_rNode ->removeChild( p_placeHolderNode );

		if ( l_rNode == l_pNode ->getLastChild() )
			 l_pNode ->appendChild( p_placeHolderNode );

		else
		{
			l_node = l_rNode ->getNextSibling();
			l_pNode ->insertBefore( p_placeHolderNode, l_node );
		}
	}

	else
	{
		l_rTempNode = ( xercesc::DOMElement* ) l_rNode ->cloneNode( false );
		l_pNode ->insertBefore( l_rTempNode, l_rNode );

		l_rPrTempNode = ( xercesc::DOMElement* ) l_rPrNode ->cloneNode( true );
		l_rTempNode ->appendChild( l_rPrTempNode );

		while ( p_placeHolderNode != l_rNode ->getChildNodes() ->item(1) )
		{
			l_node = p_placeHolderNode ->getPreviousSibling();
			l_rNode ->removeChild( l_node );

			if ( l_rTempNode ->getChildNodes() ->getLength() == 1 )
				 l_rTempNode ->appendChild( l_node );

			else
			{
				l_tempNode = l_rTempNode ->getChildNodes() ->item(1);
				l_rTempNode ->insertBefore( l_node, l_tempNode );
			}
		}

		l_rNode ->removeChild( p_placeHolderNode );
		l_pNode ->insertBefore( p_placeHolderNode, l_rNode );
	}



	if ( l_pNode ->getChildNodes() ->getLength() == 2 )
	{
		l_pNode ->removeChild( p_placeHolderNode );
		l_parentNode ->insertBefore( p_placeHolderNode, l_pNode );
		l_parentNode ->removeChild( l_pNode );
	}

	else
	if ( p_placeHolderNode == l_pNode ->getChildNodes() ->item(1) )
	{
		l_pNode ->removeChild( p_placeHolderNode );
		l_parentNode ->insertBefore( p_placeHolderNode, l_pNode );
	}

	else
	if ( p_placeHolderNode == l_pNode ->getLastChild() )
	{
		l_pNode ->removeChild( p_placeHolderNode );

		if ( l_pNode == l_parentNode ->getLastChild() )
			 l_parentNode ->appendChild( p_placeHolderNode );

		else
		{
			l_node = l_pNode ->getNextSibling();
			l_parentNode ->insertBefore( p_placeHolderNode, l_node );
		}
	}

	else
	{
		l_pTempNode = ( xercesc::DOMElement* ) l_pNode ->cloneNode( false );
		l_parentNode ->insertBefore( l_pTempNode, l_pNode );

		l_pPrTempNode = ( xercesc::DOMElement* ) l_pPrNode ->cloneNode( true );
		l_pTempNode ->appendChild( l_pPrTempNode );

		while ( p_placeHolderNode != l_pNode ->getChildNodes() ->item(1) )
		{
			l_node = p_placeHolderNode ->getPreviousSibling();
			l_pNode ->removeChild( l_node );

			if ( l_pTempNode ->getChildNodes() ->getLength() == 1 )
				 l_pTempNode ->appendChild( l_node );

			else
			{
				l_tempNode = l_pTempNode ->getChildNodes() ->item(1);
				l_tempNode ->insertBefore( l_node, l_tempNode );
			}
		}

		l_pNode ->removeChild( p_placeHolderNode );
		l_parentNode ->insertBefore( p_placeHolderNode, l_pNode );
	}



	m_type				= ( p_valueByParam ->find( PARAM_RTF ) != p_valueByParam ->end() ? TYPE_RTF : TYPE_HTML );
	m_altChunkString1	= "<" + l_wordMlPrefix + ":altChunk " + l_relMlPrefix + ":id=\"";
	m_altChunkString2	= "\"/>";

	if ( l_parentNode ->getNodeType()		== xercesc::DOMNode::ELEMENT_NODE
	  && XmlFunc::XMLChCmp( l_parentNode ->getLocalName(),		_X( "tc" ) )
	  && XmlFunc::XMLChCmp( l_parentNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) )
	  && p_placeHolderNode					== l_parentNode ->getLastChild() )
	{
		/***
		m_altChunkString2 +=
			  "<"	+ l_wordMlPrefix + ":p>"
			+ "<"	+ l_wordMlPrefix + ":pPr>"
			+ "<"	+ l_wordMlPrefix + ":spacing "	+ l_wordMlPrefix + ":before=\"0\" " + l_wordMlPrefix + ":after=\"0\"/>"
			+ "<"	+ l_wordMlPrefix + ":rPr>"
			+ "<"	+ l_wordMlPrefix + ":sz "		+ l_wordMlPrefix + ":val=\"2\" />"
			+ "<"	+ l_wordMlPrefix + ":szCs "		+ l_wordMlPrefix + ":val=\"2\" />"
			+ "</"	+ l_wordMlPrefix + ":rPr>"
			+ "</"	+ l_wordMlPrefix + ":pPr>"
			+ "</"	+ l_wordMlPrefix + ":p>";
		***/

		l_doc = p_item ->getDoc();

		l_pNode = l_doc ->createElementNS( Utf8ToXMLCh( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "p" ) );
		l_pNode ->setPrefix( Utf8ToXMLCh( l_wordMlPrefix ) );
		l_parentNode ->appendChild( l_pNode );

		l_pPrNode = l_doc ->createElementNS( Utf8ToXMLCh( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "pPr" ) );
		l_pPrNode ->setPrefix( Utf8ToXMLCh( l_wordMlPrefix ) );
		l_pNode ->appendChild( l_pPrNode );
		
		l_node = l_doc ->createElementNS( Utf8ToXMLCh( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "spacing" ) );
		l_node ->setPrefix( Utf8ToXMLCh( l_wordMlPrefix ) );
		l_pPrNode ->appendChild( l_node );

		( ( xercesc::DOMElement* ) l_node ) ->setAttribute( Utf8ToXMLCh( l_wordMlPrefix + ":before" ), _X( "0" ) );
		( ( xercesc::DOMElement* ) l_node ) ->setAttribute( Utf8ToXMLCh( l_wordMlPrefix + ":after" ), _X( "0" ) );
		
		l_rPrNode = l_doc ->createElementNS( Utf8ToXMLCh( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "rPr" ) );
		l_rPrNode ->setPrefix( Utf8ToXMLCh( l_wordMlPrefix ) );
		l_pPrNode ->appendChild( l_rPrNode );

		l_node = l_doc ->createElementNS( Utf8ToXMLCh( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "sz" ) );
		l_node ->setPrefix( Utf8ToXMLCh( l_wordMlPrefix ) );
		l_rPrNode ->appendChild( l_node );

		( ( xercesc::DOMElement* ) l_node ) ->setAttribute( Utf8ToXMLCh( l_wordMlPrefix + ":val" ), _X( "2" ) );

		l_node = l_doc ->createElementNS( Utf8ToXMLCh( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "szCs" ) );
		l_node ->setPrefix( Utf8ToXMLCh( l_wordMlPrefix ) );
		l_rPrNode ->appendChild( l_node );

		( ( xercesc::DOMElement* ) l_node ) ->setAttribute( Utf8ToXMLCh( l_wordMlPrefix + ":val" ), _X( "2" ) );
	}

	p_placeHolderNode ->setAttribute( _X( "type" ), _X( "altChunk" ) );
} // c'tor

DocxCompilerAltChunkField::~DocxCompilerAltChunkField()
{

} // d'tor



void DocxCompilerAltChunkField::serialize( ZipFile* p_zipFile )
{
	DocxCompilerField::serialize( p_zipFile );

	p_zipFile ->writeNum<int16>	( m_type );
	p_zipFile ->writeStr		( m_altChunkString1 );
	p_zipFile ->writeStr		( m_altChunkString2 );
} // serialize
