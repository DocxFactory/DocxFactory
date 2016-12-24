
#include "DocxFactory/DocxCompiler/DocxCompilerFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItemFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItemGroup.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItem.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItemLayout.h"
#include "DocxFactory/DocxCompiler/DocxCompilerXmlString.h"

#include "DocxFactory/xml/XmlFunc.h"
#include "DocxFactory/xml/XmlTreeDriller.h"
#include "DocxFactory/xml/Utf8ToXMLCh.h"

#include "DocxFactory/zip/ZipFile.h"

#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/math/MathFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include "xercesc/util/XercesDefs.hpp"
#include "xercesc/dom/DOM.hpp"

#include "boost/scoped_ptr.hpp"

using namespace DocxFactory;
using namespace std;



DocxCompilerItemGroup::DocxCompilerItemGroup( DocxCompilerItemFile* p_itemFile, DocxCompilerItem* p_parent )
{
	m_itemFile			= p_itemFile;
	m_item				= p_parent;

	m_fillItemTop		= NULL;
	m_fillItemMiddle	= NULL;
	m_fillItemBottom	= NULL;
	m_fillItemSingle	= NULL;
	m_fillItemEqual		= false;
	m_fillItemTrCnt		= 0.0f;

	m_spacerItem		= NULL;
	m_spacerTrCnt		= 0.0f;

	m_shade1			= "auto";
	m_shade2			= "auto";
	m_striped			= false;
} // c'tor

DocxCompilerItemGroup::~DocxCompilerItemGroup()
{
	map<string, DocxCompilerItem*>::iterator l_itemIterator;
	FOR_EACH( l_itemIterator, &m_childItemsByName )
	{
		delete l_itemIterator ->second;
	}
} // d'tor



void DocxCompilerItemGroup::loadStripes()
{
	list<DocxCompilerItem*>::iterator	l_itemIterator;
	DocxCompilerItem*					l_item;

	xercesc::DOMElement*				l_bodyNode;
	xercesc::DOMNode*					l_trNode;
	xercesc::DOMNode*					l_tcNode;
	xercesc::DOMNode*					l_tcPrNode;
	xercesc::DOMElement*				l_shdNode;

	string								l_wordMlPrefix = m_itemFile ->getWordMlPrefix();
	string								l_shade[2];
	unsigned short						i = 0;

	l_shade[0] = "auto";
	l_shade[1] = "auto";

	FOR_EACH( l_itemIterator, &m_childItems )
	{
		l_item = *l_itemIterator;
		if ( l_item ->isAlternate() )
		{
			l_bodyNode	= l_item ->getBodyNode();
			l_trNode	= l_bodyNode ->getFirstChild();

			if ( l_trNode
			  && l_trNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
			  && XmlFunc::XMLChCmp( l_trNode ->getLocalName(),		_X( "tr" ) )
			  && XmlFunc::XMLChCmp( l_trNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
			{
				l_tcNode = l_trNode ->getFirstChild() ->getNextSibling(); // tblPrEx, trPr
				l_tcNode = l_tcNode ->getNextSibling();

				if ( l_tcNode
				  && l_tcNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
				  && XmlFunc::XMLChCmp( l_tcNode ->getLocalName(),		_X( "tc" ) )
				  && XmlFunc::XMLChCmp( l_tcNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
				{
					l_tcPrNode = l_tcNode ->getFirstChild();

					l_shdNode = XmlFunc::getChildByTagName(
						l_tcPrNode,
						NULL,
						"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
						"shd" );

					if( l_shdNode && l_shdNode ->hasAttribute( Utf8ToXMLCh( l_wordMlPrefix + ":fill" ) ) )
						l_shade[i] = XmlFunc::XMLChToUtf8( l_shdNode ->getAttribute( Utf8ToXMLCh( l_wordMlPrefix + ":fill" ) ) );
				}
			}
		}

		i++;
		if ( i >= 2 )
			break;
	}

	if ( l_shade[0] != l_shade[1] )
	{
		m_striped	= true;
		m_shade1	= l_shade[0];
		m_shade2	= l_shade[1];
	}
} // loadStripes

void DocxCompilerItemGroup::createStripePlaceHolders()
{
	createStripePlaceHoldersByGroup( this );
} // createStripePlaceHolders

void DocxCompilerItemGroup::createStripePlaceHoldersByGroup( DocxCompilerItemGroup* p_itemGroup )
{
	list<DocxCompilerItem*>::const_iterator	l_itemIterator;
	const list<DocxCompilerItem*>*			l_items;
	DocxCompilerItem*						l_item;
	DocxCompilerItemGroup*					l_itemGroup;

	boost::scoped_ptr<XmlTreeDriller>		l_cursorDriller;
	xercesc::DOMElement*					l_cursorNode;

	xercesc::DOMDocument*					l_doc;
	xercesc::DOMElement*					l_bodyNode;
	xercesc::DOMNode*						l_tcNode;
	xercesc::DOMNode*						l_tcPrNode;
	xercesc::DOMElement*					l_shdNode;
	xercesc::DOMNamedNodeMap*				l_attributes;
	xercesc::DOMElement*					l_node;

	string									l_wordMlPrefix = m_itemFile ->getWordMlPrefix();
	string									l_shade;

	string									l_str;
	size_t									l_len;
	size_t									l_pos;
	bool									l_ok;

	l_items = p_itemGroup ->getChildItems();
	FOR_EACH( l_itemIterator, l_items )
	{
		l_item = *l_itemIterator;

		if ( !l_item ->isAlternate() )
			continue;

		l_doc		= l_item ->getDoc();
		l_bodyNode	= l_item ->getBodyNode();
		l_cursorDriller.reset( new XmlTreeDriller(
			l_bodyNode,
			"", "",
			"", "placeholder,tr",
			"", "", "" ) );

		while ( l_cursorNode = ( xercesc::DOMElement* ) l_cursorDriller ->nextNode() )
		{
			if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
			  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "tr" ) )
			  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
			{
				l_tcNode = l_cursorNode ->getFirstChild() ->getNextSibling(); // tblPrEx + trPr

				while ( l_tcNode = l_tcNode ->getNextSibling() )
				{
					if ( l_tcNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
					  && XmlFunc::XMLChCmp( l_tcNode ->getLocalName(),		_X( "tc" ) )
					  && XmlFunc::XMLChCmp( l_tcNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
					{
						l_tcPrNode = l_tcNode ->getFirstChild();

						l_node	= NULL;
						l_ok	= false;

						while ( l_node = XmlFunc::getChildByTagName( l_tcPrNode, l_node, "http://schemas.openxmlformats.org/wordprocessingml/2006/main", "vMerge,hMerge" ) )
						{
							l_str = XmlFunc::XMLChToUtf8( l_node ->getAttribute( Utf8ToXMLCh( l_wordMlPrefix + ":val" ) ) );
							if ( l_str.empty() || StrFunc::lc( l_str ) == "continue" )
							{
								l_ok = true;
								break;
							}
						}

						if ( l_ok )
							continue;

						l_shdNode = XmlFunc::getChildByTagName(
							l_tcPrNode,
							NULL,
							"http://schemas.openxmlformats.org/wordprocessingml/2006/main", 
							"shd" );

						l_shade = "auto";

						if( l_shdNode && l_shdNode ->hasAttribute( Utf8ToXMLCh( l_wordMlPrefix + ":fill" ) ) )
							l_shade = XmlFunc::XMLChToUtf8( l_shdNode ->getAttribute( Utf8ToXMLCh( l_wordMlPrefix + ":fill" ) ) );

						/***
						if( l_shade != m_shade1
						 && l_shade != m_shade2 )
							continue;
						***/

						if( !l_shdNode )
						{
							l_shdNode = l_doc ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "shd" ) );
							l_shdNode ->setPrefix( Utf8ToXMLCh( l_wordMlPrefix ) );

							l_tcPrNode ->appendChild( l_shdNode );

							l_shdNode ->setAttribute( Utf8ToXMLCh( l_wordMlPrefix + ":val" ),	_X( "clear" ) );
							l_shdNode ->setAttribute( Utf8ToXMLCh( l_wordMlPrefix + ":color" ),	_X( "auto" ) );
						}

						else
						{
							l_attributes = l_shdNode ->getAttributes();

							l_len = l_attributes ->getLength();
							for ( l_pos = l_len - 1; l_pos != -1; l_pos-- )
							{
								l_str = XmlFunc::XMLChToUtf8( l_attributes ->item( l_pos ) ->getNodeName() );

								if ( l_str != l_wordMlPrefix + ":val"
								  && l_str != l_wordMlPrefix + ":color"
								  && l_str != l_wordMlPrefix + ":fill" )

									l_shdNode ->removeAttribute( Utf8ToXMLCh( l_str ) );
							}
						}

						l_shdNode ->setAttribute( Utf8ToXMLCh( l_wordMlPrefix + ":fill" ), _X( "placeholder" ) );
					}
				}

				l_cursorDriller ->setCurrentNode( l_cursorNode, XmlTreeDriller::CURSOR_AFTER );
			}

			else
			if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
  			  && XmlFunc::XMLChCmp( l_cursorNode ->getNodeName(), _X( "placeholder" ) )
			  && XmlFunc::XMLChCmp( l_cursorNode ->getAttribute( _X( "type" ) ), _X( "item-group" ) ) )
			{
				l_str		= XmlFunc::XMLChToUtf8( l_cursorNode ->getAttribute( _X( "ptr" ) ) );
				l_itemGroup	= ( DocxCompilerItemGroup* ) StrFunc::strToInt<size_t>( l_str );

				if ( !l_itemGroup ->isStriped() )
					createStripePlaceHoldersByGroup( l_itemGroup );
			}
		}
	}
} // createStripePlaceHoldersByGroup



void DocxCompilerItemGroup::serialize( ZipFile* p_zipFile )
{
	map<string, DocxCompilerItem*>::iterator l_itemIterator;

	p_zipFile ->writePtr		( this );
	p_zipFile ->writePtr		( m_itemFile );
	p_zipFile ->writePtr		( m_item );

	p_zipFile ->writePtr		( m_fillItemTop );
	p_zipFile ->writePtr		( m_fillItemMiddle );
	p_zipFile ->writePtr		( m_fillItemBottom );
	p_zipFile ->writePtr		( m_fillItemSingle );
	p_zipFile ->writeNum<uint8>	( m_fillItemEqual );
	p_zipFile ->writeNum<double>( m_fillItemTrCnt );

	p_zipFile ->writePtr		( m_spacerItem );
	p_zipFile ->writeNum<double>( m_spacerTrCnt );

	p_zipFile ->writeNum<uint8> ( m_striped );
	p_zipFile ->writeStr		( m_shade1 );
	p_zipFile ->writeStr		( m_shade2 );

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_childItemsByName.size() );
	FOR_EACH( l_itemIterator, &m_childItemsByName )
	{
		p_zipFile ->writeStr( l_itemIterator ->first );
		p_zipFile ->writePtr( l_itemIterator ->second );
	}
} // serialize



void DocxCompilerItemGroup::setItem( DocxCompilerItem* p_parent )
{
	m_item = p_parent;
} // setItem

void DocxCompilerItemGroup::insertChildItem( DocxCompilerItem* p_item )
{
	m_childItemsByName.insert( make_pair( p_item ->getName(), p_item ) );
	m_childItems.push_back( p_item );
} // insertItemToGroup

void DocxCompilerItemGroup::setFillItem( DocxCompilerItem* p_fillItem )
{
	list<DocxCompilerItem*>::iterator	l_childItemIterator;
	DocxCompilerItem*					l_childItem;

	string								l_pageNum;
	bool								l_pageBottom;

	xercesc::DOMDocument*				l_doc;
	xercesc::DOMElement*				l_bodyNode;

	xercesc::DOMElement*				l_trNode;
	xercesc::DOMElement*				l_trNodePrev;
	xercesc::DOMElement*				l_trNodeNext;

	xercesc::DOMElement*				l_trNodeTop;
	xercesc::DOMElement*				l_trNodeMiddle;
	xercesc::DOMElement*				l_trNodeBottom;
	xercesc::DOMElement*				l_trNodeTemp;

	xercesc::DOMElement*				l_tcNode;
	xercesc::DOMElement*				l_tcBordersNode;
	xercesc::DOMElement*				l_tcBorderNode;
	xercesc::DOMElement*				l_tcBorderNodeTemp;

	xercesc::DOMNamedNodeMap*			l_attributes;
	xercesc::DOMAttr*					l_attrNode;
	XMLSize_t							l_len;
	XMLSize_t							l_pos;

	l_doc			= p_fillItem ->getDoc();
	l_bodyNode		= p_fillItem ->getBodyNode();
	l_pageNum		= p_fillItem ->getPath() ->front() ->getName().substr(5);

	l_trNodeTop		= NULL;
	l_trNodeMiddle	= NULL;
	l_trNodeBottom	= NULL;

	l_trNodePrev	= NULL;
	l_trNode		= XmlFunc::getChildByTagName(
		l_bodyNode,
		NULL,
		"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
		"tr" );

	while ( l_trNode )
	{
		l_trNodeNext = XmlFunc::getChildByTagName(
			l_bodyNode,
			l_trNode,
			"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
			"tr" );

		if(!l_trNodePrev )
			l_trNodeTop = l_trNode;

		if(!l_trNodeMiddle && l_trNodePrev && l_trNodeNext )
			l_trNodeMiddle = l_trNode;

		if(!l_trNodeNext )
			l_trNodeBottom = l_trNode;

		l_trNodePrev	= l_trNode;
		l_trNode		= l_trNodeNext;
	}

	if ( !l_trNodeTop || !l_trNodeMiddle || !l_trNodeBottom )
	{
		m_fillItemTop		= p_fillItem;
		m_fillItemMiddle	= p_fillItem;
		m_fillItemBottom	= p_fillItem;
		m_fillItemSingle	= p_fillItem;
		m_fillItemEqual		= true;
		m_fillItemTrCnt		= p_fillItem ->getItemLayout() ->getFixedTrCnt();

		if( m_fillItemTrCnt == 0.0f )
			m_fillItemTrCnt = 1.0f;
	}

	else
	{
		m_fillItemTop = new DocxCompilerItem(
			m_itemFile, 
			this, 
			"_fillItemTop" + l_pageNum, 
			0.0f,
			0.0f,
			false,
			false,
			false,
			false,
			false );

		m_itemFile ->insertItem( m_fillItemTop );
		m_itemFile ->getFile() ->insertItem( m_fillItemTop );

		l_doc		= m_fillItemTop ->getDoc();
		l_bodyNode	= m_fillItemTop ->getBodyNode();

		l_trNodeTemp = ( xercesc::DOMElement* ) l_doc ->importNode( l_trNodeTop, true );
		l_bodyNode ->appendChild( l_trNodeTemp );



		m_fillItemMiddle = new DocxCompilerItem(
			m_itemFile, 
			this, 
			"_fillItemMiddle" + l_pageNum, 
			0.0f,
			0.0f,
			false,
			false,
			false,
			false,
			false );

		m_itemFile ->insertItem( m_fillItemMiddle );
		m_itemFile ->getFile() ->insertItem( m_fillItemMiddle );

		l_doc		= m_fillItemMiddle ->getDoc();
		l_bodyNode	= m_fillItemMiddle ->getBodyNode();

		if ( l_trNodeMiddle )
		{
			l_trNodeTemp = ( xercesc::DOMElement* ) l_doc ->importNode( l_trNodeMiddle, true );
			l_bodyNode ->appendChild( l_trNodeTemp );
		}

		else
		{
			l_trNodeTemp = ( xercesc::DOMElement* ) l_doc ->importNode( l_trNodeTop, true );
			l_bodyNode ->appendChild( l_trNodeTemp );

			l_tcBorderNode = NULL;

			l_tcNode = XmlFunc::getChildByTagName(
				l_trNodeTop,
				NULL,
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				"tc" );

			if( l_tcNode )
			{
				l_tcBordersNode = XmlFunc::getChildByTagName(
					l_tcNode ->getFirstChild(),
					NULL,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"tcBorders" );

				if( l_tcBordersNode )
				{
					l_tcBorderNode = XmlFunc::getChildByTagName(
						l_tcBordersNode,
						NULL,
						"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
						"bottom" );
				}
			}

			if ( l_tcBorderNode )
			{
				l_tcNode = NULL;

				while ( l_tcNode = XmlFunc::getChildByTagName(
					l_trNodeTemp,
					l_tcNode,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"tc" ) )
				{
					l_tcBordersNode = XmlFunc::getChildByTagName(
						l_tcNode ->getFirstChild(),
						NULL,
						"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
						"tcBorders" );

					if( l_tcBordersNode )
					{
						l_tcBorderNodeTemp = XmlFunc::getChildByTagName(
							l_tcBordersNode,
							NULL,
							"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
							"top" );

						l_attributes = l_tcBorderNodeTemp ->getAttributes();

						l_len = l_attributes ->getLength();
						for ( l_pos = l_len - 1; l_pos != -1; l_pos-- )
						{
							l_tcBorderNodeTemp ->removeAttributeNode( ( xercesc::DOMAttr* ) l_attributes ->item( l_pos ) );
						}

						l_attributes = l_tcBorderNode ->getAttributes();

						l_len = l_attributes ->getLength();
						for ( l_pos = 0; l_pos < l_len; l_pos++ )
						{
							l_attrNode = ( xercesc::DOMAttr* ) l_attributes ->item( l_pos );

							l_tcBorderNodeTemp ->setAttribute( l_attrNode ->getNodeName(), l_attrNode ->getNodeValue() );
						}
					}
				}
			}
		}



		m_fillItemBottom = new DocxCompilerItem(
			m_itemFile, 
			this, 
			"_fillItemBottom" + l_pageNum, 
			0.0f,
			0.0f,
			false,
			false,
			false,
			false,
			false );

		m_itemFile ->insertItem( m_fillItemBottom );
		m_itemFile ->getFile() ->insertItem( m_fillItemBottom );

		l_doc		= m_fillItemBottom ->getDoc();
		l_bodyNode	= m_fillItemBottom ->getBodyNode();

		l_trNodeTemp = ( xercesc::DOMElement* ) l_doc ->importNode( l_trNodeBottom, true );
		l_bodyNode ->appendChild( l_trNodeTemp );



		m_fillItemSingle = new DocxCompilerItem(
			m_itemFile, 
			this, 
			"_fillItemSingle" + l_pageNum, 
			0.0f,
			0.0f,
			false,
			false,
			false,
			false,
			false );

		m_itemFile ->insertItem( m_fillItemSingle );
		m_itemFile ->getFile() ->insertItem( m_fillItemSingle );

		l_doc		= m_fillItemSingle ->getDoc();
		l_bodyNode	= m_fillItemSingle ->getBodyNode();

		l_trNodeTemp = ( xercesc::DOMElement* ) l_doc ->importNode( l_trNodeTop, true );
		l_bodyNode ->appendChild( l_trNodeTemp );

		l_tcBorderNode = NULL;

		l_tcNode = XmlFunc::getChildByTagName(
			l_trNodeBottom,
			NULL,
			"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
			"tc" );

		if( l_tcNode )
		{
			l_tcBordersNode = XmlFunc::getChildByTagName(
				l_tcNode ->getFirstChild(),
				NULL,
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				"tcBorders" );

			if( l_tcBordersNode )
			{
				l_tcBorderNode = XmlFunc::getChildByTagName(
					l_tcBordersNode,
					NULL,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"bottom" );
			}
		}

		if ( l_tcBorderNode )
		{
			l_tcNode = NULL;

			while ( l_tcNode = XmlFunc::getChildByTagName(
				l_trNodeTemp,
				l_tcNode,
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				"tc" ) )
			{
				l_tcBordersNode = XmlFunc::getChildByTagName(
					l_tcNode ->getFirstChild(),
					NULL,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"tcBorders" );

				if( l_tcBordersNode )
				{
					l_tcBorderNodeTemp = XmlFunc::getChildByTagName(
						l_tcBordersNode,
						NULL,
						"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
						"bottom" );

					l_attributes = l_tcBorderNodeTemp ->getAttributes();

					l_len = l_attributes ->getLength();
					for ( l_pos = l_len - 1; l_pos != -1; l_pos-- )
					{
						l_tcBorderNodeTemp ->removeAttributeNode( ( xercesc::DOMAttr* ) l_attributes ->item( l_pos ) );
					}

					l_attributes = l_tcBorderNode ->getAttributes();

					l_len = l_attributes ->getLength();
					for ( l_pos = 0; l_pos < l_len; l_pos++ )
					{
						l_attrNode = ( xercesc::DOMAttr* ) l_attributes ->item( l_pos );

						l_tcBorderNodeTemp ->setAttribute( l_attrNode ->getNodeName(), l_attrNode ->getNodeValue() );
					}
				}
			}
		}

		if( p_fillItem ->getSize() > 0.0f )
			m_fillItemTrCnt = MathFunc::round( p_fillItem ->getSize(), 0 );
	}



	l_pageBottom = false;

	FOR_EACH( l_childItemIterator, &m_childItems )
	{
		l_childItem = *l_childItemIterator;

		if( l_childItem == p_fillItem )
			l_pageBottom = true;

		else
		if( l_pageBottom )
			l_childItem ->setPageBottom( l_pageBottom );
	}
} // setFillItem

void DocxCompilerItemGroup::setSpacerItem( DocxCompilerItem* p_spacerItem )
{
	m_spacerItem = p_spacerItem;

	if( p_spacerItem ->getItemLayout() )
		m_spacerTrCnt = p_spacerItem ->getItemLayout() ->getFixedTrCnt();
} // setSpacerItem



DocxCompilerItem* DocxCompilerItemGroup::getItem() const
{
	return m_item;
} // getItem

const map<string, DocxCompilerItem*>* DocxCompilerItemGroup::getChildItemsByName() const
{
	return &m_childItemsByName;
} // getChildItemsByName

const list<DocxCompilerItem*>* DocxCompilerItemGroup::getChildItems() const
{
	return &m_childItems;
} // getChildItems

DocxCompilerItem* DocxCompilerItemGroup::getFillItemTop() const
{
	return m_fillItemTop;
} // getFillItemTop

DocxCompilerItem* DocxCompilerItemGroup::getFillItemMiddle() const
{
	return m_fillItemMiddle;
} // getFillItemMiddle

DocxCompilerItem* DocxCompilerItemGroup::getFillItemBottom() const
{
	return m_fillItemBottom;
} // getFillItemBottom

DocxCompilerItem* DocxCompilerItemGroup::getFillItemSingle() const
{
	return m_fillItemSingle;
} // getFillItemSingle

bool DocxCompilerItemGroup::getFillItemEqual() const
{
	return m_fillItemEqual;
} // getFillItemEqual

double DocxCompilerItemGroup::getFillItemTrCnt() const
{
	return m_fillItemTrCnt;
} // getFillItemTrCnt

DocxCompilerItem* DocxCompilerItemGroup::getSpacerItem() const
{
	return m_spacerItem;
} // getSpacerItem

bool DocxCompilerItemGroup::isStriped() const
{
	return m_striped;
} // isStriped

string DocxCompilerItemGroup::getShade1() const
{
	return m_shade1;
} // getShade1

string DocxCompilerItemGroup::getShade2() const
{
	return m_shade2;
} // getShade2
