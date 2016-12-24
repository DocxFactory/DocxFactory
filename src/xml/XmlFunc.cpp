
#include "DocxFactory/xml/XmlFunc.h"
#include "DocxFactory/xml/Utf8ToXMLCh.h"
#include "DocxFactory/xml/ParseXmlException.h"
#include "DocxFactory/xml/ParseXmlFileException.h"
#include "DocxFactory/xml/InvalidXmlPathException.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/str/StrEntries.h"

#include "xercesc/dom/DOM.hpp"
#include "xercesc/framework/LocalFileFormatTarget.hpp"
#include "xercesc/framework/MemBufFormatTarget.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"

#include "unicode/ucnv.h"

#include "boost/scoped_array.hpp"

#include <set>
#include <list>
#include <cstring>

using namespace DocxFactory;
using namespace std;



xercesc::DOMDocument* XmlFunc::parseFileToDoc( xercesc::DOMLSParser* p_parser, const string& p_fileName )
{
	xercesc::DOMDocument* l_doc;

	try
	{
		l_doc = p_parser ->parseURI( p_fileName.c_str() );
	}

	catch ( ... )
	{
		throw ParseXmlFileException( p_fileName, __FILE__, __LINE__ );
	}

	return l_doc;
} // parseFileToDoc

xercesc::DOMDocument* XmlFunc::parseBufToDoc( xercesc::DOMLSParser* p_parser, byte* p_buf, size_t p_bufSize )
{
	xercesc::DOMImplementation*	l_impl		= NULL;
	xercesc::DOMLSInput*		l_input		= NULL;
	xercesc::MemBufInputSource*	l_memBuf	= NULL;
	xercesc::DOMDocument*		l_doc		= NULL;

	try
	{
		l_impl		= xercesc::DOMImplementationRegistry::getDOMImplementation( _X( "ls" ) );
		l_input		= l_impl ->createLSInput();

		l_memBuf	= new xercesc::MemBufInputSource( ( const XMLByte* ) p_buf, p_bufSize, "" );
		l_input ->setByteStream( l_memBuf );

		l_doc		= p_parser ->parse( l_input );

		delete l_memBuf;
		l_input ->release();
	}

	catch ( ... )
	{
		if ( l_memBuf )	delete l_memBuf;
		if ( l_input )	l_input ->release();

		throw ParseXmlException( __FILE__, __LINE__ );
	}

	return l_doc;
} // parseBufToDoc



void XmlFunc::saveDocToFile( const xercesc::DOMDocument* p_doc, const string& p_fileName )
{
	xercesc::DOMImplementationLS*	l_impl			= NULL;
	xercesc::DOMLSSerializer*		l_serializer	= NULL;
	xercesc::DOMLSOutput*			l_outputStream	= NULL;
	xercesc::XMLFormatTarget*		l_target		= NULL;

	try
	{
		l_impl			= ( xercesc::DOMImplementationLS* ) xercesc::DOMImplementationRegistry::getDOMImplementation( _X( "ls" ) );
		l_serializer	= l_impl ->createLSSerializer();

		l_outputStream	= l_impl ->createLSOutput();
		l_target		= new xercesc::LocalFileFormatTarget( p_fileName.c_str() );

		l_outputStream ->setByteStream( l_target );
		l_serializer ->write( p_doc, l_outputStream );

		delete l_target;

		l_outputStream ->release();
		l_serializer ->release();
	}

	catch ( ... )
	{
		if ( l_target )	delete l_target;

		if ( l_outputStream )	l_outputStream ->release();
		if ( l_serializer )		l_serializer ->release();

		throw;
	}
} // saveDocToFile

byte* XmlFunc::saveDocToBuf( const xercesc::DOMDocument* p_doc, size_t& p_bufSize )
{
	byte*							l_buf			= NULL;
	xercesc::DOMImplementationLS*	l_impl			= NULL;
	xercesc::DOMLSSerializer*		l_serializer	= NULL;
	xercesc::DOMLSOutput*			l_outputStream	= NULL;
	xercesc::MemBufFormatTarget*	l_target		= NULL;

	try
	{
		l_impl			= ( xercesc::DOMImplementationLS* ) xercesc::DOMImplementationRegistry::getDOMImplementation( _X( "ls" ) );
		l_serializer	= l_impl ->createLSSerializer();

		l_outputStream	= l_impl ->createLSOutput();
		l_target		= new xercesc::MemBufFormatTarget();

		l_outputStream	->setByteStream( l_target );
		l_serializer	->write( p_doc, l_outputStream );

		p_bufSize	= l_target ->getLen();
		l_buf		= new byte[ p_bufSize ];

		memcpy( l_buf, l_target ->getRawBuffer(), p_bufSize );

		delete l_target;

		l_outputStream ->release();
		l_serializer ->release();
	}

	catch ( ... )
	{
		if ( l_buf )	delete[]	l_buf;
		if ( l_target )	delete		l_target;
		
		if ( l_outputStream )	l_outputStream ->release();
		if ( l_serializer )		l_serializer ->release();

		throw;
	}

	return l_buf;
} // saveDocToBuf



xercesc::DOMElement* XmlFunc::getChildByTagName(
	xercesc::DOMNode*	p_parentNode,
	xercesc::DOMNode*	p_fromChild,
	const string&		p_namespaceUri,
	const string&		p_tagNameList )
{
	set<string>	l_tagList;
	bool		l_tagAll;

	string		l_nodeNs;
	string		l_nodeName;
	
	l_tagAll = p_tagNameList.empty();

	if ( !l_tagAll )
		StrFunc::strToSet( p_tagNameList, l_tagList );

	if ( !p_fromChild )
		p_fromChild = p_parentNode ->getFirstChild();
	
	else
		p_fromChild = p_fromChild ->getNextSibling();

	while ( p_fromChild )
	{
		if ( p_fromChild ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE )
		{
			l_nodeNs	= XMLChToUtf8( p_fromChild ->getNamespaceURI() );
			l_nodeName	= XMLChToUtf8( l_nodeNs.empty() ? p_fromChild ->getNodeName() : p_fromChild ->getLocalName() );
		
			if ( ( p_namespaceUri.empty()	|| p_namespaceUri == l_nodeNs )
			  && ( l_tagAll					|| l_tagList.find( l_nodeName ) != l_tagList.end() ) )

				break;
		}

		p_fromChild = p_fromChild ->getNextSibling();
	}

	if ( p_fromChild )
		return ( xercesc::DOMElement* ) p_fromChild;

	else
		return NULL;
} // getChildByTagName

xercesc::DOMNode* XmlFunc::getNextNode( xercesc::DOMNode* l_cursorNode )
{
	xercesc::DOMElement*	l_rootNode = l_cursorNode ->getOwnerDocument() ->getDocumentElement();
	xercesc::DOMNode*		l_node;

	if( l_cursorNode ->hasChildNodes() )
 		l_cursorNode = l_cursorNode ->getFirstChild();

	else
	{
		while ( l_cursorNode != l_rootNode )
		{
			l_node = l_cursorNode ->getNextSibling();

			if ( l_node )
			{
				l_cursorNode = l_node;
				break;
			}

			l_cursorNode = l_cursorNode ->getParentNode();
		}

		if ( l_cursorNode == l_rootNode )
			 l_cursorNode = NULL;
	}

	return l_cursorNode;
} // getNextNode

xercesc::DOMNode* XmlFunc::getPrevNode( xercesc::DOMNode* l_cursorNode )
{
	xercesc::DOMElement*	l_rootNode = l_cursorNode ->getOwnerDocument() ->getDocumentElement();
	xercesc::DOMNode*		l_parentNode;

	if ( l_cursorNode == l_rootNode )
		 l_cursorNode = NULL;

	else
	{
		l_parentNode = l_cursorNode ->getParentNode();

		if ( l_cursorNode == l_parentNode ->getFirstChild() )
			 l_cursorNode = l_parentNode;

		else
		{
			l_cursorNode = l_cursorNode ->getPreviousSibling();

			while ( l_cursorNode ->hasChildNodes() )
					l_cursorNode = l_cursorNode ->getLastChild();
		}
	}

	return l_cursorNode;
} // getPrevNode

bool XmlFunc::getNextTag(
	xercesc::DOMElement*	p_containerNode,
	xercesc::DOMNode*&		p_cursorNode,
	TagPos&					p_cursorPos,
	string&					p_stringValue )
{
	xercesc::DOMNode*	l_lastChild;
	xercesc::DOMAttr*	l_attrNode;
	string				l_attrName;
	string				l_attrValue;

	XMLSize_t	l_len;
	XMLSize_t	l_pos;

	switch ( p_cursorNode ->getNodeType() )
	{
	case xercesc::DOMNode::ELEMENT_NODE:

		if( p_cursorPos == TAG_UNDEFINED )
			p_cursorPos = TAG_BEFORE;

		if(!p_cursorNode ->hasChildNodes() && !( p_cursorPos == TAG_BEFORE || p_cursorPos == TAG_AFTER ) )
			p_cursorPos = TAG_EMPTY;

		break;

	case xercesc::DOMNode::TEXT_NODE:
	case xercesc::DOMNode::CDATA_SECTION_NODE:

		if ( !( p_cursorPos == TAG_BEFORE || p_cursorPos == TAG_AFTER ) )
				p_cursorPos = TAG_UNDEFINED;

		break;
	}

	
	
	// move cursor one tag forward

	if ( p_cursorPos == TAG_BEFORE )
	{
		switch ( p_cursorNode ->getNodeType() )
		{
		case xercesc::DOMNode::ELEMENT_NODE:

			p_cursorPos = ( !p_cursorNode ->hasChildNodes() ? TAG_EMPTY : TAG_START );
			break;

		case xercesc::DOMNode::TEXT_NODE:
		case xercesc::DOMNode::CDATA_SECTION_NODE:

			p_cursorPos = TAG_UNDEFINED;
			break;
		}
	}

	else
	if ( p_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
	  && p_cursorPos == TAG_START )
	{
		p_cursorNode = p_cursorNode ->getFirstChild();

		switch ( p_cursorNode ->getNodeType() )
		{
		case xercesc::DOMNode::ELEMENT_NODE:

			p_cursorPos = ( !p_cursorNode ->hasChildNodes() ? TAG_EMPTY : TAG_START );
			break;

		case xercesc::DOMNode::TEXT_NODE:
		case xercesc::DOMNode::CDATA_SECTION_NODE:

			p_cursorPos = TAG_UNDEFINED;
			break;
		}
	}

	else
	if ( p_cursorNode ->getNodeType() != xercesc::DOMNode::ELEMENT_NODE 
	|| ( p_cursorPos == TAG_END
	  || p_cursorPos == TAG_EMPTY
	  || p_cursorPos == TAG_AFTER ) )
	{
		if ( p_cursorNode == p_containerNode )
			 p_cursorPos = TAG_AFTER;

		else 
		{
			l_lastChild = p_cursorNode ->getNextSibling();
			if ( l_lastChild )
			{
				p_cursorNode = l_lastChild;

				switch ( p_cursorNode ->getNodeType() )
				{
				case xercesc::DOMNode::ELEMENT_NODE:

					p_cursorPos = ( !p_cursorNode ->hasChildNodes() ? TAG_EMPTY : TAG_START );
					break;

				case xercesc::DOMNode::TEXT_NODE:
				case xercesc::DOMNode::CDATA_SECTION_NODE:

					p_cursorPos = TAG_UNDEFINED;
					break;
				}
			}

			else
			{
				p_cursorNode = p_cursorNode ->getParentNode();
				p_cursorPos = TAG_END;
			}
		}
	}


	
	// draw string value

	switch ( p_cursorNode ->getNodeType() )
	{
	case xercesc::DOMNode::ELEMENT_NODE:

		switch ( p_cursorPos )
		{
		case TAG_START:
		case TAG_EMPTY:

			p_stringValue = "<" + XMLChToUtf8( p_cursorNode ->getNodeName() );

			l_len = p_cursorNode ->getAttributes() ->getLength();

			for ( l_pos = 0; l_pos < l_len; ++l_pos )
			{
				l_attrNode		 = ( xercesc::DOMAttr* ) p_cursorNode ->getAttributes() ->item( l_pos );
				l_attrName		 =			  XMLChToUtf8( l_attrNode ->getNodeName() );
				l_attrValue		 = encodeXml( XMLChToUtf8( l_attrNode ->getNodeValue() ) ); // office 2010 requires attribute values to be encoded but not text nodes

				p_stringValue	+= " " + l_attrName + "=\"" + l_attrValue + "\"";
			}

			p_stringValue += ( p_cursorPos == TAG_START ? ">" : "/>" );
			break;

		case TAG_END:

			p_stringValue = "</" + XMLChToUtf8( p_cursorNode ->getNodeName() ) + ">";
			break;

		case TAG_BEFORE:
		case TAG_AFTER:

			p_stringValue = "";
			break;
		}

		break;

	case xercesc::DOMNode::TEXT_NODE:

		p_stringValue = encodeXml( XMLChToUtf8( p_cursorNode ->getNodeValue() ) );
		break;

	case xercesc::DOMNode::CDATA_SECTION_NODE:

		p_stringValue = "<![CDATA[" + XMLChToUtf8( ( ( xercesc::DOMCharacterData* ) p_cursorNode ) ->getData() ) + "]]>";
		break;
	}

	if ( p_cursorPos == TAG_AFTER )
		return false;

	return true;
} // getNextTag



size_t XmlFunc::getChildNum( xercesc::DOMNode* p_node )
{
	xercesc::DOMNode* l_parentNode = p_node ->getParentNode();

	XMLSize_t	l_len = l_parentNode ->getChildNodes() ->getLength();
	XMLSize_t	l_pos;

	for ( l_pos = 0; l_pos < l_len; ++l_len )
	{
		if ( l_parentNode ->getChildNodes() ->item( l_pos ) == p_node )
			break;
	}

	return l_pos;
} // getChildNum

list<size_t> XmlFunc::getNodePath( xercesc::DOMNode* p_node )
{
	list<size_t>			l_path;
	xercesc::DOMElement*	l_rootNode = p_node ->getOwnerDocument() ->getDocumentElement();

	while ( p_node != l_rootNode )
	{
		l_path.push_front( getChildNum( p_node ) );
		p_node = p_node ->getParentNode();
	}

	return l_path;
} // getNodePath

xercesc::DOMNode* XmlFunc::getNodeByPath( xercesc::DOMDocument* p_doc, list<size_t>* p_path )
{
	xercesc::DOMNode*		l_node = p_doc ->getDocumentElement();
	list<size_t>::iterator	l_pathIterator;

	FOR_EACH( l_pathIterator, p_path )
	{
		l_node = l_node ->getChildNodes() ->item( *l_pathIterator );

		if ( !l_node )
			throw InvalidXmlPathException( __FILE__, __LINE__ );
	}

	return l_node;
} // getNodeByPath



void XmlFunc::unindentDoc( xercesc::DOMDocument* p_doc )
{
	xercesc::DOMElement* l_root = p_doc ->getDocumentElement();
	unindentDocRecurr( l_root );
} // unindentDoc

void XmlFunc::removeAttrByNamespace(
	xercesc::DOMElement*	p_node,
	const string&			p_attrNsList,
	const string&			p_attrList )
{
	/***
	list<xercesc::DOMNode*>				l_attrRemoveList;
	list<xercesc::DOMNode*>::iterator	l_attrIterator;
	***/

	set<string>	l_attrNsList;
	set<string>	l_attrList;
	bool		l_attrNsAll;
	bool		l_attrAll;

	xercesc::DOMNode*	l_attrNode;
	string				l_attrNs;
	string				l_attrName;

	XMLSize_t	l_pos;
	XMLSize_t	l_len = p_node ->getAttributes() ->getLength();

	l_attrNsAll	= p_attrNsList.empty();
	l_attrAll	= p_attrList.empty();

	if ( !l_attrNsAll )	StrFunc::strToSet( p_attrNsList,	l_attrNsList );
	if ( !l_attrAll )	StrFunc::strToSet( p_attrList,		l_attrList );

	for ( l_pos = l_len - 1; l_pos != -1; --l_pos )
	{
		l_attrNode	= p_node ->getAttributes() ->item( l_pos );
		l_attrNs	= XMLChToUtf8( l_attrNode ->getNamespaceURI() );
		l_attrName	= XMLChToUtf8( l_attrNs.empty() ? l_attrNode ->getNodeName() : l_attrNode ->getLocalName() );

		if ( ( l_attrNsAll	|| l_attrNsList.find( l_attrNs ) != l_attrNsList.end() )
		  && ( l_attrAll	|| l_attrList.find( l_attrName ) != l_attrList.end() ) )

			p_node ->removeAttribute( l_attrNode ->getNodeName() );
	}



	/***
	for ( l_pos = 0; l_pos < l_len; ++l_pos )
	{
		l_attrNode	= p_node ->getAttributes() ->item( l_pos );
		l_attrNs	= XMLChToUtf8( l_attrNode ->getNamespaceURI() );
		l_attrName	= XMLChToUtf8( l_attrNs.empty() ? l_attrNode ->getNodeName() : l_attrNode ->getLocalName() );

		if ( ( l_attrNsAll		|| l_attrNsList.find( l_attrNs ) != l_attrNsList.end() )
		  && ( l_attrAll		|| l_attrList.find( l_attrName ) != l_attrList.end() ) )

			l_attrRemoveList.push_back( l_attrNode );
	}

	FOR_EACH( l_attrIterator, &l_attrRemoveList )
	{
		l_attrNode = *l_attrIterator;
		p_node ->removeAttributeNS( l_attrNode ->getNamespaceURI(), l_attrNode ->getLocalName() );
	}
	***/

} // removeAttrByNamespace

void XmlFunc::importAttributes( xercesc::DOMElement* p_sourceNode, xercesc::DOMElement* p_targetNode )
{
	xercesc::DOMAttr*	l_attrNode;
	size_t				l_len;
	size_t				l_pos;

	l_len = p_sourceNode ->getAttributes() ->getLength();

	for ( l_pos = 0; l_pos < l_len; l_pos++ )
	{
		l_attrNode = ( xercesc::DOMAttr* ) p_sourceNode ->getAttributes() ->item( l_pos );

		p_targetNode ->setAttribute( l_attrNode ->getName(), l_attrNode ->getValue() );
	}
} // importAttributes

string XmlFunc::normalizeNsPrefix(
	xercesc::DOMElement*	p_rootNode,
	const string&			p_nsPrefixDefault,
	const string&			p_nsUri )
{
	xercesc::DOMNamedNodeMap*	l_attributes = p_rootNode ->getAttributes();
	xercesc::DOMAttr*			l_attrNode;
	string						l_attrNs;
	string						l_attrName;
	string						l_attrValue;
	string						l_attrPrefix;

	string						l_prefix;
	bool						l_found = false;
	XMLSize_t					l_len	= l_attributes ->getLength();
	XMLSize_t					l_pos;

	for ( l_pos = 0; l_pos < l_len; ++l_pos )
	{
		l_attrNode		= ( xercesc::DOMAttr* ) l_attributes ->item( l_pos );
		l_attrNs		= XMLChToUtf8( l_attrNode ->getNamespaceURI() );
		l_attrName		= XMLChToUtf8( l_attrNs.empty() ? l_attrNode ->getNodeName() : l_attrNode ->getLocalName() );
		l_attrValue		= XMLChToUtf8( l_attrNode ->getNodeValue() );
		l_attrPrefix	= XMLChToUtf8( l_attrNode ->getPrefix() );

		/*** 
		// i decided that the normalizeNsPrefix will not accept default xmlns
		// and must add a named xmlns because it would be easier to work with later on.

		if ( l_attrPrefix == "" && l_attrName == "xmlns" )
			l_prefix = "";

		else
		***/

		if ( l_attrPrefix == "xmlns" && !l_attrName.empty() )
		{
			l_prefix = l_attrName;

			if ( l_attrValue == p_nsUri )
			{
				l_found = true;
				break;
			}
		}
	}

	if ( !l_found )
	{
		l_prefix = p_nsPrefixDefault;
		l_pos = 0;

		while ( p_rootNode ->hasAttribute( Utf8ToXMLCh( "xmlns:" + l_prefix ) ) )
		{
			++l_pos;
			l_prefix = p_nsPrefixDefault + StrFunc::intToStr( l_pos );
		}

		p_rootNode ->setAttribute( Utf8ToXMLCh( "xmlns:" + l_prefix ), Utf8ToXMLCh( p_nsUri ) );
	}

	return l_prefix;  
} // normalizeNsPrefix



// ** encodeXml does not contain any protection against invalid utf8 chars

string XmlFunc::encodeXml( const string& p_srcStr )
{
	size_t			l_srcLen	= p_srcStr.length();
	size_t			l_srcPos	= 0;

	size_t			l_dstPos	= 0;
	unsigned char	l_ch;

	boost::scoped_array<char> l_dstStr( new char[ l_srcLen * 6 + 1 ] );

	while ( l_srcPos < l_srcLen )
	{
		l_ch = ( unsigned char ) p_srcStr[ l_srcPos ];

			 if ( l_ch == '\t' )	{ strncpy( l_dstStr.get() + l_dstPos, "&#x9;", 5 );		l_dstPos += 5; }
		else if ( l_ch == '\n' )	{ strncpy( l_dstStr.get() + l_dstPos, "&#xA;", 5 );		l_dstPos += 5; }
		else if ( l_ch == '\v' )	{ strncpy( l_dstStr.get() + l_dstPos, "&#xB;", 5 );		l_dstPos += 5; }
		else if ( l_ch == '\f' )	{ strncpy( l_dstStr.get() + l_dstPos, "&#xC;", 5 );		l_dstPos += 5; }
		else if ( l_ch == '\r' )	{ strncpy( l_dstStr.get() + l_dstPos, "&#xD;", 5 );		l_dstPos += 5; }
		else if ( l_ch == '&' )		{ strncpy( l_dstStr.get() + l_dstPos, "&amp;", 5 );		l_dstPos += 5; }
		else if ( l_ch == '"' )		{ strncpy( l_dstStr.get() + l_dstPos, "&quot;", 6 );	l_dstPos += 6; }
		else if ( l_ch == '~' )		{ strncpy( l_dstStr.get() + l_dstPos, "&apos;", 6 );	l_dstPos += 6; }
		else if ( l_ch == '<' )		{ strncpy( l_dstStr.get() + l_dstPos, "&lt;", 4 );		l_dstPos += 4; }
		else if ( l_ch == '>' )		{ strncpy( l_dstStr.get() + l_dstPos, "&gt;", 4 );		l_dstPos += 4; }

		else if ( l_ch <= 31 || l_ch == 127 )
		{
			l_dstStr[ l_dstPos ] = '?';
			l_dstPos++;
		}

		else
		{
			l_dstStr[ l_dstPos ] = p_srcStr[ l_srcPos ];
			l_dstPos++;
		}

		l_srcPos++;
	}

	l_dstStr[ l_dstPos ] = '\0';

	return l_dstStr.get();
} // encodeXml

string XmlFunc::getXmlDeclaration( xercesc::DOMDocument* p_doc, bool p_standAlone )
{
	string l_encoding = XMLChToUtf8( p_doc ->getXmlEncoding() );

	return "<?xml version=\"1.0\""
		+ ( !l_encoding.empty()	? " encoding=\"" + l_encoding + "\"" : "" )
		+ (  p_standAlone		? " standalone=\"yes\"" : "" )
		+ "?>\n";
} // getXmlDeclaration



string XmlFunc::XMLChToUtf8( const XMLCh* p_xstr )
{
	if ( p_xstr == NULL || *p_xstr == 0 )
		return "";

	boost::scoped_array<UChar>	l_ustr;
	boost::scoped_array<char>	l_str;
	size_t						l_len = getXMLChLen( p_xstr );

	l_ustr.reset( XMLChToUChar( p_xstr, l_len ) );
	l_str.reset	( LocaleFunc::ucharToUtf8( l_ustr.get(), l_len ) );

	return l_str.get();
} // XMLChToUtf8

string XmlFunc::XMLChToStr( const XMLCh* p_xstr )
{
	if ( p_xstr == NULL || *p_xstr == 0 )
		return "";

	boost::scoped_array<UChar>	l_ustr;
	boost::scoped_array<char>	l_str;
	size_t						l_len = getXMLChLen( p_xstr );

	l_ustr.reset( XMLChToUChar( p_xstr, l_len ) );
	l_str.reset	( LocaleFunc::ucharToStr( l_ustr.get(), l_len ) );

	return l_str.get();
} // XMLChToStr

XMLCh* XmlFunc::utf8ToXMLCh( const string& p_str )
{
	boost::scoped_array<UChar>	l_ustr;
	XMLCh*						l_xstr;

	l_ustr.reset( LocaleFunc::utf8ToUChar( p_str.c_str(), p_str.length() ) );
	l_xstr		= ucharToXMLCh( l_ustr.get(), p_str.length() );

	return l_xstr;
} // utf8ToXMLCh

XMLCh* XmlFunc::strToXMLCh( const string& p_str )
{
	boost::scoped_array<UChar>	l_ustr;
	XMLCh*						l_xstr;

	l_ustr.reset( LocaleFunc::strToUChar( p_str.c_str(), p_str.length() ) );
	l_xstr		= ucharToXMLCh( l_ustr.get(), p_str.length() );

	return l_xstr;
} // strToXMLCh

bool XmlFunc::XMLChCmp( const XMLCh* p_xstr1, const XMLCh* p_xstr2 )
{
	if ( p_xstr1 == NULL || p_xstr2 == NULL )
		return false;

	size_t l_pos = 0;

	while ( true )
	{
		if ( p_xstr1[ l_pos ] != p_xstr2[ l_pos ] )
			return false;

		if ( p_xstr1[ l_pos ] == 0 )
			return true;

		l_pos++;
	}
} // XMLChCmp



XMLCh* XmlFunc::ucharToXMLCh( const UChar* p_ustr, size_t p_len )
{
	XMLCh*	l_xstr	= new XMLCh[ UCNV_GET_MAX_BYTES_FOR_STRING( p_len, 2 ) ];
	size_t	l_pos	= 0;

	// length is mostly used for buffer size and in this case for added safety. 
	// whether the position is null is what should terminate the loop.

	while ( p_ustr[ l_pos ] && l_pos < p_len )
	{
		l_xstr[ l_pos ] = ( XMLCh ) p_ustr[ l_pos ];
		l_pos++;
	}

	l_xstr[ l_pos ] = NULL;

	return l_xstr;
} // ucharToXMLCh

UChar* XmlFunc::XMLChToUChar( const XMLCh* p_xstr, size_t p_len )
{
	UChar*	l_ustr	= new UChar[ UCNV_GET_MAX_BYTES_FOR_STRING( p_len, 2 ) ];
	size_t	l_pos	= 0;

	while ( p_xstr[ l_pos ] && l_pos < p_len )
	{
		l_ustr[ l_pos ] = ( UChar ) p_xstr[ l_pos ];
		l_pos++;
	}

	l_ustr[ l_pos ] = NULL;

	return l_ustr;
} // XMLChToUChar

size_t XmlFunc::getXMLChLen( const XMLCh* p_xstr )
{
	size_t l_len = 0;

	while ( p_xstr[ l_len ] ) l_len++;

	return l_len;
} //getXMLChLen



void XmlFunc::unindentDocRecurr( const xercesc::DOMElement* p_parentNode )
{
	xercesc::DOMNodeList*	l_childNodeList	= p_parentNode ->getChildNodes();
	xercesc::DOMNode*		l_childNode;

	string		l_str;
	XMLSize_t	l_pos;

	for ( l_pos = l_childNodeList ->getLength() - 1; l_pos != -1; --l_pos )
	{
		l_childNode = l_childNodeList ->item( l_pos );
		switch ( l_childNode ->getNodeType() )
		{
			case xercesc::DOMNode::ELEMENT_NODE:

				unindentDocRecurr( ( xercesc::DOMElement* ) l_childNode );
				break;

			case xercesc::DOMNode::TEXT_NODE:

				l_str = XMLChToUtf8( l_childNode ->getNodeValue() );

				if ( l_childNodeList -> getLength() > 1
				  && strchr( l_str.c_str(), '\n' )
				  && StrFunc::trim( l_str ).empty() )
					( ( xercesc::DOMNode* ) p_parentNode ) ->removeChild( l_childNode );

				break;
		}
	}
} // unindentDocRecurr
