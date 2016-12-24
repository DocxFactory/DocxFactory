
#include "DocxFactory/DocxCompiler/DocxCompilerFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItemFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItemGroup.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItem.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItemLayout.h"
#include "DocxFactory/DocxCompiler/DocxCompilerField.h"
#include "DocxFactory/DocxCompiler/DocxCompilerXmlString.h"
#include "DocxFactory/DocxCompiler/DocxCompilerId.h"

#include "DocxFactory/xml/XmlFunc.h"
#include "DocxFactory/xml/Utf8ToXMLCh.h"

#include "DocxFactory/zip/ZipFile.h"

#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/str/StrEntries.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include "xercesc/util/XercesDefs.hpp"
#include "xercesc/dom/DOM.hpp"

#include "boost/scoped_ptr.hpp"

using namespace DocxFactory;
using namespace std;



void DocxCompilerItem::getItemOptions(
	string&			p_name,
	double&			p_size,
	double&			p_pageSize,
	bool&			p_keepTogether,
	bool&			p_keepWithPrev,
	bool&			p_filler,
	bool&			p_spacer,
	bool&			p_alternate )
{
	string	l_str;
	size_t	l_len;
	size_t	l_pos;
	double	x;

	p_size			= 0.0f;
	p_pageSize		= 0.0f;
	p_keepTogether	= false;
	p_keepWithPrev	= false;
	p_filler		= false;
	p_spacer		= false;
	p_alternate		= false;

	while ( true )
	{
		l_len = p_name.length();

		l_pos = p_name.rfind( "_pagesize" );
		if ( l_pos != string::npos && l_pos + 9 < l_len )
		{
			l_str = StrFunc::replace( StrFunc::lc( p_name.substr( l_pos + 9 ) ), "d", "." );
			if ( StrFunc::isNum( l_str ) )
			{
				x = StrFunc::strToInt<double>( l_str );
				if ( x > 0 && x < 1000 )
				{
					p_name		= p_name.substr( 0, l_pos );
					p_pageSize	= x;

					continue;
				}
			}
		}

		l_pos = p_name.rfind( "_size" );
		if ( l_pos != string::npos && l_pos + 5 < l_len )
		{
			l_str = StrFunc::replace( StrFunc::lc( p_name.substr( l_pos + 5 ) ), "d", "." );
			if ( StrFunc::isNum( l_str ) )
			{
				x = StrFunc::strToInt<double>( l_str );
				if ( x > 0 && x < 1000 )
				{
					p_name = p_name.substr( 0, l_pos );
					p_size = x;

					continue;
				}
			}
		}

		if ( l_len > 13 && p_name.substr( l_len - 13 ) == "_keeptogether" )
		{
			p_name			= p_name.substr( 0, l_len - 13 );
			p_keepTogether	= true;

			continue;
		}

		if ( l_len > 13 && p_name.substr( l_len - 13 ) == "_keepwithprev" )
		{
			p_name			= p_name.substr( 0, l_len - 13 );
			p_keepWithPrev	= true;

			continue;
		}

		if ( l_len > 10 && p_name.substr( l_len - 10 ) == "_alternate" )
		{
			p_name			= p_name.substr( 0, l_len - 10 );
			p_alternate		= true;

			continue;
		}

		break;
	}

	if ( StrFunc::begins( p_name, "filler" ) )
	{
		if ( p_name.length() > 6 || StrFunc::isNum( p_name.substr( 6 ) ) )
		{
			p_filler = true;

			return;
		}
	}

	if ( StrFunc::begins( p_name, "spacer" ) )
	{
		if ( p_name.length() > 6 || StrFunc::isNum( p_name.substr( 6 ) ) )
		{
			p_spacer = true;

			return;
		}
	}
} // getItemOptions



DocxCompilerItem::DocxCompilerItem(	
	DocxCompilerItemFile*	p_itemFile,
	DocxCompilerItemGroup*	p_itemGroup,
	string					p_itemName,
	double					p_size,
	double					p_pageSize,
	bool					p_keepTogether,
	bool					p_keepWithPrev,
	bool					p_filler,
	bool					p_spacer,
	bool					p_alternate )
{
	xercesc::DOMImplementation*	l_impl;

	m_itemFile		= p_itemFile;
	m_itemGroup		= p_itemGroup;
	m_name			= p_itemName;

	m_doc			= NULL;
	m_bodyNode		= NULL;

	m_topLevel		= false;
	m_fillItemGroup	= NULL;

	m_itemLayout	= NULL;
	m_size			= p_size;
	m_pageSize		= p_pageSize;
	m_pageBottom	= false;
	m_keepTogether	= p_keepTogether;
	m_keepWithPrev	= p_keepWithPrev;
	m_filler		= p_filler;
	m_spacer		= p_spacer;
	m_alternate		= p_alternate;

	if( m_itemGroup )
		m_itemGroup ->insertChildItem( this );

	l_impl		= xercesc::DOMImplementationRegistry::getDOMImplementation( _X( "ls" ) );
	m_doc		= l_impl ->createDocument();

	m_bodyNode	= ( xercesc::DOMElement* ) m_doc ->importNode( m_itemFile ->getBodyNode(), false );
	m_doc ->appendChild( m_bodyNode );
} // c'tor

DocxCompilerItem::~DocxCompilerItem()
{
	set<DocxCompilerItemGroup*>::iterator			l_itemGroupIterator;
	multimap<string, DocxCompilerField*>::iterator	l_fieldIterator;
	list<DocxCompilerXmlString*>::iterator			l_xmlStringIterator;

	FOR_EACH( l_itemGroupIterator, &m_childItemGroups )
	{
		delete *l_itemGroupIterator;
	}

	FOR_EACH( l_fieldIterator, &m_fieldsByName )
	{
		delete l_fieldIterator ->second;
	}

	FOR_EACH( l_xmlStringIterator, &m_xmlStrings )
	{
		delete *l_xmlStringIterator;
	}

	if( m_doc )
		m_doc ->release();

	if( m_itemLayout )
		delete m_itemLayout;
} // d'tor



void DocxCompilerItem::loadItemLayout()
{
	m_itemLayout = DocxCompilerItemLayout::createItemLayout(
		m_itemFile,
		m_bodyNode,
		NULL,
		m_size );
} // loadItemLayout

void DocxCompilerItem::loadXmlStrings()
{
	DocxCompilerId*							l_id;
	DocxCompilerXmlString*					l_xmlString		= NULL;
	string									l_wordMlPrefix	= m_itemFile ->getWordMlPrefix();
	bool									l_isPlaceHolder;
	DocxCompilerXmlString::XmlStringType	l_type;

	xercesc::DOMNode*						l_cursorNode	= m_bodyNode;
	XmlFunc::TagPos							l_cursorPos		= XmlFunc::TAG_START;
	string									l_stringValue;

	xercesc::DOMNamedNodeMap*				l_attributes;
	xercesc::DOMAttr*						l_attrNode;
	xercesc::DOMNode*						l_node;
	
	boost::scoped_ptr<StrEntries>			l_strEntries;
	string									l_str;
	void*									l_ptr;
	size_t									l_len;
	size_t									l_pos;

	while ( XmlFunc::getNextTag( m_bodyNode, l_cursorNode, l_cursorPos, l_stringValue ) )
	{
		if ( l_cursorNode == m_bodyNode )
			break;

		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
		  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "tc" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) )
		  && l_cursorPos == XmlFunc::TAG_END )
		{
			l_xmlString = new DocxCompilerXmlString(
				this,
				DocxCompilerXmlString::TYPE_TC_END,
				"",
				NULL );

			m_xmlStrings.push_back( l_xmlString );
			m_itemFile ->getFile() ->insertXmlString( l_xmlString );
		}



		l_isPlaceHolder = 

		   ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
  		  && XmlFunc::XMLChCmp( l_cursorNode ->getNodeName(), _X( "placeholder" ) ) );

		if ( l_isPlaceHolder
		  && XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "item-group" ) ) )
		{
			l_str = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_cursorNode )->getAttribute( _X( "ptr" ) ) );
			l_ptr = ( void* ) StrFunc::strToInt<size_t>( l_str );

			l_xmlString = new DocxCompilerXmlString(
				this,
				DocxCompilerXmlString::TYPE_ITEM_GROUP,
				"",
				l_ptr );

			m_xmlStrings.push_back( l_xmlString );
			m_itemFile ->getFile() ->insertXmlString( l_xmlString );
		}

		else 
		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
		  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "br" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) )
		  && XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( Utf8ToXMLCh( l_wordMlPrefix + ":type" ) ), _X( "page" ) )
		  && ( ( xercesc::DOMElement* ) l_cursorNode ) ->hasAttribute( _X( "sectId" ) ) )
		{
			l_pos = StrFunc::strToInt<size_t>( XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "sectId" ) ) ) );
			l_ptr = ( void* ) m_itemFile ->getSects() ->at( l_pos );

			l_stringValue = 
					"<"	+ l_wordMlPrefix + ":p>"
				+ "<"	+ l_wordMlPrefix + ":pPr>"
				+ "<"	+ l_wordMlPrefix + ":spacing "
						+ l_wordMlPrefix + ":before=\"0\" "
						+ l_wordMlPrefix + ":after=\"0\" "
						+ l_wordMlPrefix + ":line=\"2\" "
						+ l_wordMlPrefix + ":lineRule=\"exact\"/>"

				+ "</"	+ l_wordMlPrefix + ":pPr>"
				+ "<"	+ l_wordMlPrefix + ":r>"
				+ "<"	+ l_wordMlPrefix + ":rPr>"
				+ "<"	+ l_wordMlPrefix + ":sz "
						+ l_wordMlPrefix + ":val=\"2\"/>"

				+ "<"	+ l_wordMlPrefix + ":szCs "
						+ l_wordMlPrefix + ":val=\"2\"/>"

				+ "</"	+ l_wordMlPrefix + ":rPr>"
				+ "<"	+ l_wordMlPrefix + ":br "
						+ l_wordMlPrefix + ":type=\"page\"/>"

				+ "</"	+ l_wordMlPrefix + ":r>"
				+ "</"	+ l_wordMlPrefix + ":p>";

			l_xmlString = new DocxCompilerXmlString(
				this,
				DocxCompilerXmlString::TYPE_PAGE_BREAK,
				l_stringValue,
				l_ptr );

			m_xmlStrings.push_back( l_xmlString );
			m_itemFile ->getFile() ->insertXmlString( l_xmlString );
		}

		else 
		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
		  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "sectPr" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
		{
			l_pos		= StrFunc::strToInt<size_t>( XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_cursorNode )->getAttribute( _X( "sectId" ) ) ) );
			l_xmlString	= m_itemFile ->getSects() ->at( l_pos );

			( ( xercesc::DOMElement* ) l_cursorNode ) ->removeAttribute( _X( "sectId" ) );

			l_cursorPos		= XmlFunc::TAG_BEFORE;
			l_node			= l_cursorNode;

			while ( XmlFunc::getNextTag( m_bodyNode, l_cursorNode, l_cursorPos, l_stringValue ) )
			{
				l_xmlString ->concatStr( l_stringValue );

				if ( l_cursorNode == l_node
				&& ( l_cursorPos == XmlFunc::TAG_END || l_cursorPos == XmlFunc::TAG_EMPTY ) )
					break;
			}

			if ( l_pos + 1 < m_itemFile ->getSects() ->size() )
			{
				l_ptr = ( void* ) m_itemFile ->getSects() ->at( l_pos + 1 );

				l_stringValue = 
						"<"	+ l_wordMlPrefix + ":p>"
					+ "<"	+ l_wordMlPrefix + ":pPr>"
					+ "<"	+ l_wordMlPrefix + ":spacing "
							+ l_wordMlPrefix + ":before=\"0\" "
							+ l_wordMlPrefix + ":after=\"0\" "
							+ l_wordMlPrefix + ":line=\"2\" "
							+ l_wordMlPrefix + ":lineRule=\"exact\"/>"

					+ "</"	+ l_wordMlPrefix + ":pPr>"
					+ "<"	+ l_wordMlPrefix + ":r>"
					+ "<"	+ l_wordMlPrefix + ":rPr>"
					+ "<"	+ l_wordMlPrefix + ":sz "
							+ l_wordMlPrefix + ":val=\"2\"/>"

					+ "<"	+ l_wordMlPrefix + ":szCs "
							+ l_wordMlPrefix + ":val=\"2\"/>"

					+ "</"	+ l_wordMlPrefix + ":rPr>"
					+ "<"	+ l_wordMlPrefix + ":br "
							+ l_wordMlPrefix + ":type=\"page\"/>"

					+ "</"	+ l_wordMlPrefix + ":r>"
					+ "</"	+ l_wordMlPrefix + ":p>";

				l_xmlString = new DocxCompilerXmlString(
					this,
					DocxCompilerXmlString::TYPE_PAGE_BREAK,
					l_stringValue,
					l_ptr );

				m_xmlStrings.push_back( l_xmlString );
				m_itemFile ->getFile() ->insertXmlString( l_xmlString );
			}
		}

		else
		if ( l_isPlaceHolder
		&& ( XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "text" ) ) 
		  || XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "number" ) ) 
		  || XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "datetime" ) ) 
		  || XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "boolean" ) ) 
		  || XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "altChunk" ) ) 
		  || XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "pic" ) )
		  || XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "barcode" ) )
		  || XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "chart" ) ) ) )
		{
			l_str = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "ptr" ) ) );
			l_ptr = ( void* ) StrFunc::strToInt<size_t>( l_str );

				 if ( XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "text" ) ) )		l_type = DocxCompilerXmlString::TYPE_TEXT_FIELD;
			else if ( XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "number" ) ) )	l_type = DocxCompilerXmlString::TYPE_NUMBER_FIELD;
			else if ( XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "datetime" ) ) )	l_type = DocxCompilerXmlString::TYPE_DATE_TIME_FIELD; 
			else if ( XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "boolean" ) ) )	l_type = DocxCompilerXmlString::TYPE_BOOLEAN_FIELD;
			else if ( XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "altChunk" ) ) )	l_type = DocxCompilerXmlString::TYPE_ALT_CHUNK_FIELD;
			else if ( XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "pic" ) ) )		l_type = DocxCompilerXmlString::TYPE_PIC_FIELD;
			else if ( XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "barcode" ) ) )	l_type = DocxCompilerXmlString::TYPE_BARCODE_FIELD;
			else if ( XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "chart" ) ) )	l_type = DocxCompilerXmlString::TYPE_CHART_FIELD;

			l_xmlString = new DocxCompilerXmlString(
				this,
				l_type,
				"",
				l_ptr ); 

			m_xmlStrings.push_back( l_xmlString );
			m_itemFile ->getFile() ->insertXmlString( l_xmlString );
		}

		/***
		else
		if ( l_isPlaceHolder
		  && XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "text" ) ) )
		{
			l_str = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "ptr" ) ) );
			l_ptr = ( void* ) StrFunc::strToInt<size_t>( l_str );

			l_xmlString = new DocxCompilerXmlString(
				this,
				DocxCompilerXmlString::TYPE_TEXT_FIELD,
				"",
				l_ptr ); 

			m_xmlStrings.push_back( l_xmlString );
			m_itemFile ->getFile() ->insertXmlString( l_xmlString );
		}

		else
		if ( l_isPlaceHolder
		  && XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "number" ) ) )
		{
			l_str = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "ptr" ) ) );
			l_ptr = ( void* ) StrFunc::strToInt<size_t>( l_str );

			l_xmlString = new DocxCompilerXmlString(
				this,
				DocxCompilerXmlString::TYPE_NUMBER_FIELD,
				"",
				l_ptr ); 

			m_xmlStrings.push_back( l_xmlString );
			m_itemFile ->getFile() ->insertXmlString( l_xmlString );
		}

		else
		if ( l_isPlaceHolder
		  && XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "datetime" ) ) )
		{
			l_str = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "ptr" ) ) );
			l_ptr = ( void* ) StrFunc::strToInt<size_t>( l_str );

			l_xmlString = new DocxCompilerXmlString(
				this,
				DocxCompilerXmlString::TYPE_DATE_TIME_FIELD,
				"",
				l_ptr ); 

			m_xmlStrings.push_back( l_xmlString );
			m_itemFile ->getFile() ->insertXmlString( l_xmlString );
		}

		else
		if ( l_isPlaceHolder
		  && XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "boolean" ) ) )
		{
			l_str = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "ptr" ) ) );
			l_ptr = ( void* ) StrFunc::strToInt<size_t>( l_str );

			l_xmlString = new DocxCompilerXmlString(
				this,
				DocxCompilerXmlString::TYPE_BOOLEAN_FIELD,
				"",
				l_ptr ); 

			m_xmlStrings.push_back( l_xmlString );
			m_itemFile ->getFile() ->insertXmlString( l_xmlString );
		}

		else
		if ( l_isPlaceHolder
		  && XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "altChunk" ) ) )
		{
			l_str = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "ptr" ) ) );
			l_ptr = ( void* ) StrFunc::strToInt<size_t>( l_str );

			l_xmlString = new DocxCompilerXmlString(
				this,
				DocxCompilerXmlString::TYPE_ALT_CHUNK_FIELD,
				"",
				l_ptr );

			m_xmlStrings.push_back( l_xmlString );
			m_itemFile ->getFile() ->insertXmlString( l_xmlString );
		}

		else
		if ( l_isPlaceHolder
		  && XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "pic" ) ) )
		{
			l_str = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "ptr" ) ) );
			l_ptr = ( void* ) StrFunc::strToInt<size_t>( l_str );

			l_xmlString = new DocxCompilerXmlString(
				this,
				DocxCompilerXmlString::TYPE_PIC_FIELD,
				"",
				l_ptr );

			m_xmlStrings.push_back( l_xmlString );
			m_itemFile ->getFile() ->insertXmlString( l_xmlString );
		}

		else
		if ( l_isPlaceHolder
		  && XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "barcode" ) ) )
		{
			l_str = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "ptr" ) ) );
			l_ptr = ( void* ) StrFunc::strToInt<size_t>( l_str );

			l_xmlString = new DocxCompilerXmlString(
				this,
				DocxCompilerXmlString::TYPE_BARCODE_FIELD,
				"",
				l_ptr );

			m_xmlStrings.push_back( l_xmlString );
			m_itemFile ->getFile() ->insertXmlString( l_xmlString );
		}

		else
		if ( l_isPlaceHolder
		  && XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "type" ) ), _X( "chart" ) ) )
		{
			l_str = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( _X( "ptr" ) ) );
			l_ptr = ( void* ) StrFunc::strToInt<size_t>( l_str );

			l_xmlString = new DocxCompilerXmlString(
				this,
				DocxCompilerXmlString::TYPE_CHART_FIELD,
				"",
				l_ptr );

			m_xmlStrings.push_back( l_xmlString );
			m_itemFile ->getFile() ->insertXmlString( l_xmlString );
		}
		***/

		else
		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
		  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "table" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
		{
			l_node = l_cursorNode ->getFirstChild();

			while ( l_node
				 && l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
				 && ( XmlFunc::XMLChCmp( l_node ->getLocalName(),		_X( "tblPr" ) )
				   || XmlFunc::XMLChCmp( l_node ->getLocalName(),		_X( "tblGrid" ) ) )
				   && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
			{
				l_node = l_node ->getNextSibling();
			}

			if ( l_node
			 && !l_node ->getNextSibling()
			 &&  l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
			 && XmlFunc::XMLChCmp( l_node ->getNodeName(), _X( "placeholder" ) )
			 && XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_node ) ->getAttribute( _X( "type" ) ), _X( "item-group" ) ) )
			{
				l_xmlString = new DocxCompilerXmlString(
					this,
					DocxCompilerXmlString::TYPE_EMPTY_TABLE_START,
					l_stringValue,
					NULL );

				m_xmlStrings.push_back( l_xmlString );
				m_itemFile ->getFile() ->insertXmlString( l_xmlString );

				while ( XmlFunc::getNextTag( m_bodyNode, l_cursorNode, l_cursorPos, l_stringValue ) )
				{
					if ( l_cursorNode == l_node )
						break;

					l_xmlString ->concatStr( l_stringValue );
				}

				l_str = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_cursorNode )->getAttribute( _X( "ptr" ) ) );
				l_ptr = ( void* ) StrFunc::strToInt<size_t>( l_str );

				l_xmlString = new DocxCompilerXmlString(
					this,
					DocxCompilerXmlString::TYPE_ITEM_GROUP,
					"",
					l_ptr );

				m_xmlStrings.push_back( l_xmlString );
				m_itemFile ->getFile() ->insertXmlString( l_xmlString );

				if ( XmlFunc::getNextTag( m_bodyNode, l_cursorNode, l_cursorPos, l_stringValue ) )
				{
					l_xmlString = new DocxCompilerXmlString(
						this,
						DocxCompilerXmlString::TYPE_EMPTY_TABLE_END,
						l_stringValue,
						NULL );

					m_xmlStrings.push_back( l_xmlString );
					m_itemFile ->getFile() ->insertXmlString( l_xmlString );
				}
			}

			else
			{
				if ( !l_xmlString || l_xmlString ->getType() != DocxCompilerXmlString::TYPE_XML )
				{
					l_xmlString = new DocxCompilerXmlString(
						this,
						DocxCompilerXmlString::TYPE_XML,
						l_stringValue,
						NULL );

					m_xmlStrings.push_back( l_xmlString );
					m_itemFile ->getFile() ->insertXmlString( l_xmlString );
				}

				else
					l_xmlString ->concatStr( l_stringValue );
			}
		}

		else
		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
		  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "p" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) )

		  && ( l_cursorPos == XmlFunc::TAG_START || l_cursorPos == XmlFunc::TAG_EMPTY ) )
		{
			l_xmlString = new DocxCompilerXmlString(
				this,
				DocxCompilerXmlString::TYPE_P_START,
				l_stringValue,
				NULL );

			m_xmlStrings.push_back( l_xmlString );
			m_itemFile ->getFile() ->insertXmlString( l_xmlString );
		}

		else
		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
		  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "docPr" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing" ) ) )
		{
			l_stringValue = "<" + XmlFunc::XMLChToUtf8( l_cursorNode ->getNodeName() );

			l_attributes = ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttributes();

			l_len = l_attributes ->getLength();
			for ( l_pos = 0; l_pos < l_len; ++l_pos )
			{
				l_attrNode	= ( xercesc::DOMAttr* ) l_attributes ->item( l_pos );
				l_str		= XmlFunc::XMLChToUtf8( l_attrNode ->getNodeName() );

				if( l_str == "id" )
					break;

				l_stringValue +=
					  " "
					+ l_str
					+ "="
					+ "\"" + XmlFunc::XMLChToUtf8( l_attrNode ->getNodeValue() ) + "\"";
			}

			l_stringValue += " " + l_str + "=" + "\"";

			if ( !l_xmlString || l_xmlString ->getType() != DocxCompilerXmlString::TYPE_XML )
			{
				l_xmlString = new DocxCompilerXmlString(
					this,
					DocxCompilerXmlString::TYPE_XML,
					l_stringValue,
					NULL );

				m_xmlStrings.push_back( l_xmlString );
				m_itemFile ->getFile() ->insertXmlString( l_xmlString );
			}

			else
				l_xmlString ->concatStr( l_stringValue );

			l_id = m_itemFile ->getFile() ->insertId(
				this,
				XmlFunc::XMLChToUtf8( l_cursorNode ->getLocalName() ),
				XmlFunc::XMLChToUtf8( l_attrNode ->getNodeValue() ) );

			l_xmlString = new DocxCompilerXmlString(
				this,
				DocxCompilerXmlString::TYPE_ID,
				"",
				( void* ) l_id );

			m_xmlStrings.push_back( l_xmlString );
			m_itemFile ->getFile() ->insertXmlString( l_xmlString );

			l_stringValue = "\"";

			for ( l_pos++; l_pos < l_len; ++l_pos )
			{
				l_attrNode	= ( xercesc::DOMAttr* ) l_attributes ->item( l_pos );
				l_str		= XmlFunc::XMLChToUtf8( l_attrNode ->getNodeName() );

				l_stringValue +=
					  " "
					+ l_str
					+ "="
					+ "\"" + XmlFunc::XMLChToUtf8( l_attrNode ->getNodeValue() ) + "\"";
			}

			l_stringValue += ( l_cursorPos == XmlFunc::TAG_START ? ">" : " />" );

			l_xmlString = new DocxCompilerXmlString(
				this,
				DocxCompilerXmlString::TYPE_XML,
				l_stringValue,
				NULL );

			m_xmlStrings.push_back( l_xmlString );
			m_itemFile ->getFile() ->insertXmlString( l_xmlString );			
		}

		else
		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
		  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "shd" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) )
		  && XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttribute( Utf8ToXMLCh( l_wordMlPrefix + ":fill" ) ), _X( "placeholder" ) ) )
		{
			l_stringValue = "<" + XmlFunc::XMLChToUtf8( l_cursorNode ->getNodeName() );

			l_attributes = ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttributes();

			l_len = l_attributes ->getLength();
			for ( l_pos = 0; l_pos < l_len; ++l_pos )
			{
				l_attrNode	= ( xercesc::DOMAttr* ) l_attributes ->item( l_pos );
				l_str		= XmlFunc::XMLChToUtf8( l_attrNode ->getNodeName() );

				if( l_str == l_wordMlPrefix + ":fill" )
					break;

				l_stringValue +=
					  " "
					+ l_str
					+ "="
					+ "\"" + XmlFunc::XMLChToUtf8( l_attrNode ->getNodeValue() ) + "\"";
			}

			l_stringValue += " " + l_str + "=" + "\"";

			if ( !l_xmlString || l_xmlString ->getType() != DocxCompilerXmlString::TYPE_XML )
			{
				l_xmlString = new DocxCompilerXmlString(
					this,
					DocxCompilerXmlString::TYPE_XML,
					l_stringValue,
					NULL );

				m_xmlStrings.push_back( l_xmlString );
				m_itemFile ->getFile() ->insertXmlString( l_xmlString );
			}

			else
				l_xmlString ->concatStr( l_stringValue );

			l_xmlString = new DocxCompilerXmlString(
				this,
				DocxCompilerXmlString::TYPE_SHADE,
				"",
				NULL );

			m_xmlStrings.push_back( l_xmlString );
			m_itemFile ->getFile() ->insertXmlString( l_xmlString );

			l_stringValue = "\"";

			for ( l_pos++; l_pos < l_len; ++l_pos )
			{
				l_attrNode	= ( xercesc::DOMAttr* ) l_attributes ->item( l_pos );
				l_str		= XmlFunc::XMLChToUtf8( l_attrNode ->getNodeName() );

				l_stringValue +=
					  " "
					+ l_str
					+ "="
					+ "\"" + XmlFunc::XMLChToUtf8( l_attrNode ->getNodeValue() ) + "\"";
			}

			l_stringValue += ( l_cursorPos == XmlFunc::TAG_START ? ">" : " />" );

			l_xmlString = new DocxCompilerXmlString(
				this,
				DocxCompilerXmlString::TYPE_XML,
				l_stringValue,
				NULL );

			m_xmlStrings.push_back( l_xmlString );
			m_itemFile ->getFile() ->insertXmlString( l_xmlString );
		}

		else
		{
			if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
			  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "line" ) )
			  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "urn:schemas-microsoft-com:vml" ) )

			  && ( l_cursorPos == XmlFunc::TAG_START || l_cursorPos == XmlFunc::TAG_EMPTY ) )
			{
				l_stringValue	= "<" + XmlFunc::XMLChToUtf8( l_cursorNode ->getNodeName() );
				l_strEntries.reset( new StrEntries( "id,style,from,to,strokeweight" ) );

				l_len = l_strEntries ->getNumEntries();
				for ( l_pos = 0; l_pos < l_len; ++l_pos ) // "id,style,from,to,strokeweight" will be the first attributes
				{
					l_str		= l_strEntries ->getEntry( l_pos );
					l_attrNode	= ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttributeNode( Utf8ToXMLCh( l_str ) );

					if( l_attrNode )
						l_stringValue +=
								" "
							+ l_str
							+ "="
							+ "\"" + XmlFunc::XMLChToUtf8( l_attrNode ->getNodeValue() ) + "\"";
				}

				l_attributes = ( ( xercesc::DOMElement* ) l_cursorNode ) ->getAttributes();

				l_len = l_attributes ->getLength();
				for ( l_pos = 0; l_pos < l_len; ++l_pos )
				{
					l_attrNode	= ( xercesc::DOMAttr* ) l_attributes ->item( l_pos );
					l_str		= XmlFunc::XMLChToUtf8( l_attrNode ->getNodeName() );

					if( l_strEntries ->lookup( l_str ) == StrEntries::npos )
						l_stringValue +=
								" "
							+ l_str
							+ "="
							+ "\"" + XmlFunc::XMLChToUtf8( l_attrNode ->getNodeValue() ) + "\"";
				}

				l_stringValue += ( l_cursorPos == XmlFunc::TAG_START ? ">" : " />" );
			}

			if ( !l_xmlString || l_xmlString ->getType() != DocxCompilerXmlString::TYPE_XML )
			{
				l_xmlString = new DocxCompilerXmlString(
					this,
					DocxCompilerXmlString::TYPE_XML,
					l_stringValue,
					NULL );

				m_xmlStrings.push_back( l_xmlString );
				m_itemFile ->getFile() ->insertXmlString( l_xmlString );
			}

			else
				l_xmlString ->concatStr( l_stringValue );
		}



		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
		  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "tc" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) )
		  && l_cursorPos == XmlFunc::TAG_START )
		{
			l_node = l_cursorNode ->getFirstChild();

			if ( !l_node
			  ||  l_node ->getNodeType() != xercesc::DOMNode::ELEMENT_NODE
			  || !XmlFunc::XMLChCmp( l_node ->getLocalName(),		_X( "tcPr" ) )
			  || !XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
			{
				l_xmlString = new DocxCompilerXmlString(
					this,
					DocxCompilerXmlString::TYPE_TC_START,
					"",
					NULL );

				m_xmlStrings.push_back( l_xmlString );
				m_itemFile ->getFile() ->insertXmlString( l_xmlString );
			}
		}

		else
		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
		  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "tcPr" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) )
		  && ( l_cursorPos == XmlFunc::TAG_END || l_cursorPos == XmlFunc::TAG_EMPTY ) )
		{
			l_xmlString = new DocxCompilerXmlString(
				this,
				DocxCompilerXmlString::TYPE_TC_START,
				"",
				NULL );

			m_xmlStrings.push_back( l_xmlString );
			m_itemFile ->getFile() ->insertXmlString( l_xmlString );
		}
	}
} // loadXmlStrings



void DocxCompilerItem::serialize( ZipFile* p_zipFile )
{
	list<DocxCompilerItem*>::iterator				l_pathIterator;
	set<DocxCompilerItemGroup*>::iterator			l_itemGroupIterator;
	multimap<string, DocxCompilerField*>::iterator	l_fieldIterator;
	list<DocxCompilerXmlString*>::iterator			l_xmlStringIterator;

	p_zipFile ->writePtr		( this );
	p_zipFile ->writePtr		( m_itemFile );
	p_zipFile ->writePtr		( m_itemGroup );
	p_zipFile ->writeStr		( m_name );

	p_zipFile ->writeNum<uint8>	( m_topLevel );
	p_zipFile ->writePtr		( m_fillItemParent );
	p_zipFile ->writePtr		( m_fillItemGroup );

	p_zipFile ->writeNum<double>( m_pageSize );
	p_zipFile ->writeNum<uint8>	( m_pageBottom );
	p_zipFile ->writeNum<uint8>	( m_keepTogether );
	p_zipFile ->writeNum<uint8>	( m_keepWithPrev );

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_path.size() );
	FOR_EACH( l_pathIterator, &m_path )
	{
		p_zipFile ->writePtr( *l_pathIterator );
	}

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_childItemGroups.size() );
	FOR_EACH( l_itemGroupIterator, &m_childItemGroups )
	{
		p_zipFile ->writePtr( *l_itemGroupIterator );
	}

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_fieldsByName.size() );
	FOR_EACH( l_fieldIterator, &m_fieldsByName )
	{
		p_zipFile ->writeStr( l_fieldIterator ->first );
		p_zipFile ->writePtr( l_fieldIterator ->second );
	}
	
	p_zipFile ->writeNum<uint32>( ( uint32 ) m_xmlStrings.size() );
	FOR_EACH( l_xmlStringIterator, &m_xmlStrings )
	{
		p_zipFile ->writePtr( *l_xmlStringIterator );
	}

	DocxCompilerItemLayout::serializeItemLayout( p_zipFile, m_itemLayout );
} // serialize



void DocxCompilerItem::removeChildItemGroup( DocxCompilerItemGroup* p_itemGroup )
{
	m_childItemGroups.erase( p_itemGroup );
} // removeChildGroup

void DocxCompilerItem::insertChildItemGroup( DocxCompilerItemGroup* p_itemGroup )
{
	m_childItemGroups.insert( p_itemGroup );
} // insertChildGroup

void DocxCompilerItem::insertField( DocxCompilerField* p_field )
{
	m_fieldsByName.insert( make_pair( p_field ->getName(), p_field ) );
} // insertField

void DocxCompilerItem::setPath()
{
	DocxCompilerItem* l_item = this;

	m_path.clear();

	do {
		m_path.push_front( l_item );

		l_item = l_item ->getItemGroup() ->getItem();
	} while ( l_item );
} // setPath

void DocxCompilerItem::setTopLevel( bool p_topLevel )
{
	m_topLevel = p_topLevel;
} // setTopLevel

void DocxCompilerItem::setFillItem( DocxCompilerItem* p_fillItem )
{
	m_fillItemParent	= p_fillItem ->getItemGroup() ->getItem();
	m_fillItemGroup		= p_fillItem ->getItemGroup();
} // setFillItem

void DocxCompilerItem::setSize( double p_size )
{
	m_size = p_size;
} // setSize

void DocxCompilerItem::setPageSize( double p_pageSize )
{
	m_pageSize = p_pageSize;
} // setPageSize

void DocxCompilerItem::setPageBottom( bool p_pageBottom )
{
	m_pageBottom = p_pageBottom;
} // setPageBottom

void DocxCompilerItem::setKeepTogether( bool p_keepTogether )
{
	m_keepTogether = p_keepTogether;
} // setKeepTogether

void DocxCompilerItem::setKeepWithPrev( bool p_keepWithPrev )
{
	m_keepWithPrev = p_keepWithPrev;
} // setKeepWithPrev

void DocxCompilerItem::setFiller( bool p_filler )
{
	m_filler = p_filler;
} // setFiller

void DocxCompilerItem::setSpacer( bool p_spacer )
{
	m_spacer = p_spacer;
} // setSpacer



DocxCompilerItemFile* DocxCompilerItem::getItemFile() const
{
	return m_itemFile;
} // getItemFile

DocxCompilerItemGroup* DocxCompilerItem::getItemGroup() const
{
	return m_itemGroup;
} // getItemGroup

string DocxCompilerItem::getName() const
{
	return m_name;
} // getName

const list<DocxCompilerItem*>* DocxCompilerItem::getPath() const
{
	return &m_path;
} // getPath

const set<DocxCompilerItemGroup*>* DocxCompilerItem::getChildItemGroups() const
{
	return &m_childItemGroups;
} // getchildItemGroups

const multimap<string, DocxCompilerField*>*	DocxCompilerItem::getFieldsByName() const
{
	return &m_fieldsByName;
} // getFieldsByName

xercesc::DOMDocument* DocxCompilerItem::getDoc() const
{
	return m_doc;
} // getDoc

xercesc::DOMElement* DocxCompilerItem::getBodyNode() const
{
	return m_bodyNode;
} // getBodyNode

const list<DocxCompilerXmlString*>*	DocxCompilerItem::getXmlStrings() const
{
	return &m_xmlStrings;
} // getXmlStrings

bool DocxCompilerItem::isTopLevel() const
{
	return m_topLevel;
} // isTopLevel

DocxCompilerItem* DocxCompilerItem::getFillItemParent() const
{
	return m_fillItemParent;
} // getFillItemParent

DocxCompilerItemGroup* DocxCompilerItem::getFillItemGroup() const
{
	return m_fillItemGroup;
} // getFillItemGroup

DocxCompilerItemLayout* DocxCompilerItem::getItemLayout() const
{
	return m_itemLayout;
} // getItemLayout

double DocxCompilerItem::getSize() const
{
	return m_size;
} // getSize

double DocxCompilerItem::getPageSize() const
{
	return m_pageSize;
} // getPageSize

bool DocxCompilerItem::isPageBottom() const
{
	return m_pageBottom;
} // isPageBottom

bool DocxCompilerItem::isKeepTogether() const
{
	return m_keepTogether;
} // isKeepTogether

bool DocxCompilerItem::isKeepWithPrev() const
{
	return m_keepWithPrev;
} // isKeepWithPrev

bool DocxCompilerItem::isFiller() const
{
	return m_filler;
} // isFiller

bool DocxCompilerItem::isSpacer() const
{
	return m_spacer;
} // isSpacer

bool DocxCompilerItem::isAlternate() const
{
	return m_alternate;
} // isAlternate
