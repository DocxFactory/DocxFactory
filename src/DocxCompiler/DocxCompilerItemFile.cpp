
#include "DocxFactory/DocxCompiler/DocxCompilerFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerStyleFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerStyle.h"
#include "DocxFactory/DocxCompiler/DocxCompilerTableStyle.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItemFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItemGroup.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItem.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItemLayout.h"
#include "DocxFactory/DocxCompiler/DocxCompilerBookmark.h"
#include "DocxFactory/DocxCompiler/DocxCompilerField.h"
#include "DocxFactory/DocxCompiler/DocxCompilerTextField.h"
#include "DocxFactory/DocxCompiler/DocxCompilerNumberField.h"
#include "DocxFactory/DocxCompiler/DocxCompilerDateTimeField.h"
#include "DocxFactory/DocxCompiler/DocxCompilerBooleanField.h"
#include "DocxFactory/DocxCompiler/DocxCompilerPicField.h"
#include "DocxFactory/DocxCompiler/DocxCompilerAltChunkField.h"
#include "DocxFactory/DocxCompiler/DocxCompilerBarcodeField.h"
#include "DocxFactory/DocxCompiler/DocxCompilerChartField.h"
#include "DocxFactory/DocxCompiler/DocxCompilerChartFieldSeries.h"
#include "DocxFactory/DocxCompiler/DocxCompilerChartFieldSingleSeries.h"
#include "DocxFactory/DocxCompiler/DocxCompilerChartFieldFixedSeries.h"
#include "DocxFactory/DocxCompiler/DocxCompilerChartFieldXY.h"
#include "DocxFactory/DocxCompiler/DocxCompilerChartFieldXYSize.h"
#include "DocxFactory/DocxCompiler/DocxCompilerXmlString.h"

#include "DocxFactory/opc/OpcPart.h"
#include "DocxFactory/opc/OpcRelationships.h"
#include "DocxFactory/opc/OpcRelationship.h"

#include "DocxFactory/xml/XmlFunc.h"
#include "DocxFactory/xml/XmlTreeDriller.h"
#include "DocxFactory/xml/Utf8ToXMLCh.h"
#include "DocxFactory/xml/NodeNotFoundException.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include "xercesc/util/XercesDefs.hpp"
#include "xercesc/dom/DOM.hpp"

#include "boost/tuple/tuple.hpp"
#include "boost/scoped_ptr.hpp"

#include <climits>

using namespace DocxFactory;
using namespace std;



// Bookmark objects are used by normalize() to sort <BookmarkStart> tags
struct Bookmark
{
	static bool BookmarkCmp( Bookmark* p_left, Bookmark* p_right );

	xercesc::DOMElement*	m_startNode;
	xercesc::DOMElement*	m_endNode;
	string					m_name;
	size_t					m_id;
	size_t					m_startGroupSeq;
	size_t					m_endGroupSeq;
};

bool Bookmark::BookmarkCmp( Bookmark* p_left, Bookmark* p_right )
{
	if ( p_left ->m_startGroupSeq < p_right ->m_startGroupSeq )
		return true;

	if ( p_left ->m_startGroupSeq == p_right ->m_startGroupSeq )
	{
		if ( p_left ->m_endGroupSeq > p_right ->m_endGroupSeq )
			return true;

		if ( p_left ->m_endGroupSeq == p_right ->m_endGroupSeq )
		{
			if ( p_left ->m_id < p_right ->m_id )
				return true;

			return false;
		}

		return false;
	}

	return false;
} // BookmarkCmp



DocxCompilerItemFile::DocxCompilerItemFile( DocxCompilerFile* p_file, OpcPart* p_part, ItemFileType p_type )
{
	m_file			= p_file;
	m_part			= p_part;
	m_type			= p_type;
	m_groupIdSeq	= -1;
	m_sectLen		= 0;

	m_rootItemGroup	= new DocxCompilerItemGroup( this, NULL );

	m_itemGroups.push_back( m_rootItemGroup ); 
	m_file ->insertItemGroup( m_rootItemGroup );
} // c'tor

DocxCompilerItemFile::~DocxCompilerItemFile()
{
	list<DocxCompilerBookmark*>::iterator	l_bookmarkIterator;
	size_t									i;

	FOR_EACH( l_bookmarkIterator, &m_rootBookmarks )
	{
		delete *l_bookmarkIterator;
	}

	delete m_rootItemGroup;

	for ( i = 0; i < m_sects.size(); i++ )
	{
		delete m_sects[i];
	}
} // d'tor



void DocxCompilerItemFile::load()
{
	normalize();
} // load

void DocxCompilerItemFile::loadBookmarks()
{
	map<size_t, DocxCompilerBookmark*>::iterator	l_bookmarkIterator;
	DocxCompilerBookmark*							l_bookmark;

	list<DocxCompilerBookmark*>						l_parentBookmarks;
	DocxCompilerBookmark*							l_parentBookmark = NULL;

	boost::scoped_ptr<XmlTreeDriller>				l_cursorDriller;
	xercesc::DOMElement*							l_cursorNode;

	xercesc::DOMDocument*							l_doc = m_part ->getDoc();
	xercesc::DOMNode*								l_parentNode;

	string											l_bookmarkName;
	size_t											l_bookmarkId;

	l_cursorDriller.reset( new XmlTreeDriller(
		m_bodyNode,

		// i decided to drill the entire tree
		// to cover all cases like message boxes that have complex tree structures and others.

		// originally we did not drill all nodes to improve performance
		// but because the file is compiled only once it's not an issue.

		// "http://schemas.openxmlformats.org/wordprocessingml/2006/main", "p,r,tbl,tr,tc",

		"", "",
		"http://schemas.openxmlformats.org/wordprocessingml/2006/main", "bookmarkStart,bookmarkEnd", // ,br,sectPr,t,tbl,pict,drawing
		"", "", "" ) );

	while ( l_cursorNode = ( xercesc::DOMElement* ) l_cursorDriller ->nextNode() )
	{
		if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "bookmarkStart" ) ) )
		{
			if ( l_parentBookmark
			&& ( l_parentBookmark ->getName().length() > 7 && l_parentBookmark ->getName().substr( l_parentBookmark ->getName().length() - 7 ) == "_notset"
			  || l_parentBookmark ->getName().length() > 4 && l_parentBookmark ->getName().substr( l_parentBookmark ->getName().length() - 4 ) == "_yes"
			  || l_parentBookmark ->getName().length() > 3 && l_parentBookmark ->getName().substr( l_parentBookmark ->getName().length() - 3 ) == "_no" ) )

				l_cursorDriller ->deleteNode();

			else
			{
				l_bookmarkName	= StrFunc::lc				( XmlFunc::XMLChToUtf8( l_cursorNode ->getAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":name" ) ) ) );
				l_bookmarkId	= StrFunc::strToInt<size_t>	( XmlFunc::XMLChToUtf8( l_cursorNode ->getAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":id" ) ) ) );

				l_bookmark = new DocxCompilerBookmark(
					this,
					l_parentBookmark,
					l_bookmarkName,
					l_bookmarkId,
					l_cursorNode );
				
				m_bookmarksById.insert( make_pair( l_bookmarkId, l_bookmark ) );
				m_file ->insertBookmark( l_bookmark );

				if ( !l_parentBookmark )
 					m_rootBookmarks.push_back( l_bookmark );

				else
					l_parentBookmark ->insertChild( l_bookmark );
			
				l_parentBookmark = l_bookmark;
				l_parentBookmarks.push_back( l_bookmark );

				// l_br = false;
			}
		}

		else
		if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "bookmarkEnd" ) ) )
		{
			l_bookmarkId		= StrFunc::strToInt<size_t>( XmlFunc::XMLChToUtf8( l_cursorNode ->getAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":id" ) ) ) );
			l_bookmarkIterator	= m_bookmarksById.find( l_bookmarkId );

			if( l_bookmarkIterator == m_bookmarksById.end() )
				l_cursorDriller ->deleteNode();

			else
			{
				l_bookmark = l_bookmarkIterator ->second;
				l_bookmark ->setEndNode( l_cursorNode );

				if ( l_bookmark == l_parentBookmark )
				{
					while ( true )
					{
						if ( l_parentBookmarks.empty() )
							break;

						l_parentBookmarks.pop_back();

						if ( l_parentBookmarks.empty() )
						{
							l_parentBookmark = NULL;
							break;
						}
			
						l_parentBookmark = l_parentBookmarks.back();
						
						if ( !l_parentBookmark ->getEndNode() )
							break;

						l_parentNode = l_parentBookmark ->getEndNode() ->getParentNode();
						l_parentNode ->removeChild( l_parentBookmark ->getEndNode() );

						l_parentNode = l_bookmark ->getEndNode() ->getParentNode();

						if ( l_bookmark ->getEndNode() == l_parentNode ->getLastChild() )
							l_parentNode ->appendChild( l_parentBookmark ->getEndNode() );

						else
							l_parentNode ->insertBefore( l_parentBookmark ->getEndNode(), l_bookmark ->getEndNode() ->getNextSibling() );

						l_bookmark = l_parentBookmark;
					}
				}
			}
		}
	}

	normalizeBookmarksBr();
	normalizeBookmarksHierarchy();
} // loadBookmarks

void DocxCompilerItemFile::loadItems()
{
	map<string, DocxCompilerItem*>::const_iterator	l_itemByNameIterator;
	const map<string, DocxCompilerItem*>*			l_itemsByName;

	list<DocxCompilerItem*>::const_iterator			l_itemIterator;
	const list<DocxCompilerItem*>*					l_items;
	DocxCompilerItem*								l_item;
	DocxCompilerItemGroup*							l_itemGroup;

	boost::scoped_ptr<XmlTreeDriller> l_treeDriller;

	xercesc::DOMDocument*	l_doc;
	xercesc::DOMElement*	l_bodyNode;
	xercesc::DOMNode*		l_pNode;
	xercesc::DOMNode*		l_pNodeTemp;

	xercesc::DOMElement*	l_cursorNode;
	xercesc::DOMNodeList*	l_nodeList;
	xercesc::DOMNode*		l_node;

	DocxCompilerItem*		l_fillItem;
	size_t					l_pageNum = 0; 
	string					l_str;

	try
	{
		loadItemsByBookmark();

		if ( m_type == DOCUMENT )
		{
			l_item = new DocxCompilerItem(
				this, 
				m_rootItemGroup, 
				"_page" + StrFunc::intToStr( ++l_pageNum ), 
				0.0f,
				0.0f,
				false,
				false,
				false,
				false,
				false );

			insertItem( l_item );
			m_file ->insertItem( l_item );

			l_doc		= l_item ->getDoc();
			l_bodyNode	= l_item ->getBodyNode();

			// cut all nodes into page items separated by br type=page
			l_nodeList = m_bodyNode ->getChildNodes();
			while ( l_nodeList ->getLength() > 0 )
			{
				l_pNode	= m_bodyNode ->getFirstChild();

				if ( l_pNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
				  && XmlFunc::XMLChCmp( l_pNode ->getLocalName(),		_X( "br" ) )
				  && XmlFunc::XMLChCmp( l_pNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) )
				  && XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_pNode ) ->getAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":type" ) ), _X( "page" ) ) 

				  || l_pNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
				  && XmlFunc::XMLChCmp( l_pNode ->getLocalName(),		_X( "sectPr" ) )
				  && XmlFunc::XMLChCmp( l_pNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) )
				  && l_pNode != m_bodyNode ->getLastChild() )
				{
					l_item = new DocxCompilerItem(
						this, 
						m_rootItemGroup, 
						"_page" + StrFunc::intToStr( ++l_pageNum ), 
						0.0f,
						0.0f,
						false,
						false,
						false,
						false,
						false );

					insertItem( l_item );
					m_file ->insertItem( l_item );

					l_doc		= l_item ->getDoc();
					l_bodyNode	= l_item ->getBodyNode();
				}

				l_pNodeTemp = l_doc ->importNode( l_pNode, true );
				l_bodyNode ->appendChild( l_pNodeTemp ); 

				m_bodyNode ->removeChild( l_pNode );
			}

			// remove empty p's at the end of the page
			l_items = m_rootItemGroup ->getChildItems();
			FOR_EACH( l_itemIterator, l_items )
			{
				l_item		= *l_itemIterator;
				l_doc		= l_item ->getDoc();
				l_bodyNode	= l_item ->getBodyNode();

				l_pNode	= l_bodyNode ->getLastChild();
				while ( l_pNode != l_bodyNode ->getFirstChild() )
				{
					if ( l_pNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
					  && XmlFunc::XMLChCmp( l_pNode ->getLocalName(),		_X( "p" ) )
					  && XmlFunc::XMLChCmp( l_pNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) )
					  && l_pNode ->getChildNodes() ->getLength() == 1 )
					{
						l_node = l_pNode ->getPreviousSibling();
						l_bodyNode ->removeChild( l_pNode );
						l_pNode = l_node;
					}
					
					else
						break;
				}
			}

			// insert page break for first page
			l_itemsByName			= m_rootItemGroup ->getChildItemsByName();
			l_itemByNameIterator	= l_itemsByName ->find( "_page1" );

			if ( l_itemByNameIterator != l_itemsByName ->end() )
			{
				l_item		= ( *l_itemByNameIterator ).second;
				l_doc		= l_item ->getDoc();
				l_bodyNode	= l_item ->getBodyNode();

				l_pNode = l_doc ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "br" ) ) ;
				l_pNode ->setPrefix( Utf8ToXMLCh( m_wordMlPrefix ) );

				( ( xercesc::DOMElement* ) l_pNode ) ->setAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":type" ), _X( "page" ) );
				( ( xercesc::DOMElement* ) l_pNode ) ->setAttribute( _X( "sectId" ), _X( "0" ) );

				if( l_bodyNode ->getChildNodes() ->getLength() > 0 )
					l_bodyNode ->insertBefore( l_pNode, l_bodyNode ->getFirstChild() );
				else  
					l_bodyNode ->appendChild( l_pNode );
			}
		}

		else
		{
			l_item = new DocxCompilerItem(
				this,
				m_rootItemGroup,
				( m_type == HEADER ? 
					"_header" + StrFunc::intToStr( m_file ->nextHeaderSeq() ) : 
					"_footer" + StrFunc::intToStr( m_file ->nextFooterSeq() ) ),
				0.0f,
				0.0f,
				false,
				false,
				false,
				false,
				false );

			insertItem( l_item );
			m_file ->insertItem( l_item );

			l_doc		= l_item ->getDoc();
			l_bodyNode	= l_item ->getBodyNode();

			l_nodeList = m_bodyNode ->getChildNodes();
			while( l_nodeList ->getLength() > 0)
			{
				l_pNode		 = m_bodyNode ->getFirstChild();
				l_pNodeTemp	 = l_doc ->importNode( l_pNode, true );

				l_bodyNode ->appendChild( l_pNodeTemp );
				m_bodyNode ->removeChild( l_pNode );
			}
		}

		// assign all page items to it
		l_items = m_rootItemGroup ->getChildItems();
		FOR_EACH( l_itemIterator, l_items )
		{
			l_item		= *l_itemIterator;
			l_bodyNode	= l_item ->getBodyNode();
			l_doc		= l_item ->getDoc();

			l_treeDriller.reset( new XmlTreeDriller(
				l_bodyNode,
				"", "",
				"", "placeholder",
				"", "type", "item-group" ) );

			while ( l_cursorNode = ( xercesc::DOMElement* ) l_treeDriller ->nextNode() )
			{
				l_str		= XmlFunc::XMLChToUtf8( l_cursorNode ->getAttribute( _X( "ptr" ) ) ) ;
				l_itemGroup	= ( DocxCompilerItemGroup* ) StrFunc::strToInt<size_t>( l_str ); 

				l_itemGroup ->setItem( l_item );
				l_item ->insertChildItemGroup( l_itemGroup );
			}
		}

		setItemsPath( m_rootItemGroup );



		if ( m_type == DOCUMENT )
		{
			FOR_EACH( l_itemIterator, &m_items )
			{
				l_item = *l_itemIterator;

				if( l_item ->getPath() ->size() == 2
				 && l_item ->getItemGroup() ->getChildItems() ->size() == 1 
				 && l_item ->getItemGroup() ->getItem() ->getChildItemGroups() ->size() == 1 )
				{
					l_item ->setTopLevel( true );

					if ( l_item ->getPageSize() > 0 )
					{
						l_fillItem = NULL;

						loadItemLayout( l_item, l_fillItem );

						if( l_fillItem )
						{
							l_item						->setFillItem( l_fillItem );
							l_fillItem ->getItemGroup() ->setFillItem( l_fillItem );
						}
					}
				}
			}

			// spacer items must be run after loading item layouts
			// because they will use the fixed trcnt if there is an item layout

			FOR_EACH( l_itemIterator, &m_items )
			{
				l_item = *l_itemIterator;

				if (l_item ->isSpacer() )
					l_item ->getItemGroup() ->setSpacerItem( l_item );
			}
		}
	}

	catch ( ... )
	{
		FOR_EACH( l_itemIterator, &m_items )
		{
			delete *l_itemIterator;
		}

		m_items.clear();

		throw;
	}
} // loadItems

void DocxCompilerItemFile::loadItemsByBookmark()
{
	list<DocxCompilerItem*>	l_itemPath;
	loadItemsByBookmark( NULL, &l_itemPath );
} // loadItemsByBookmark

void DocxCompilerItemFile::loadItemsByBookmark( DocxCompilerBookmark* p_bookmark, list<DocxCompilerItem*>* p_itemPath )
{
	map<string, DocxCompilerBookmark*>::const_iterator	l_bookmarkIterator;
	const map<string, DocxCompilerBookmark*>*			l_bookmarks;

	list<DocxCompilerBookmark*>::const_iterator			l_nextChildBookmarkIterator;
	DocxCompilerBookmark*								l_nextChildBookmark;

	list<DocxCompilerBookmark*>::const_iterator			l_childBookmarkIterator;
	const list<DocxCompilerBookmark*>*					l_childBookmarks;
	DocxCompilerBookmark*								l_childBookmark;

	map<string, DocxCompilerItem*>::const_iterator		l_itemIterator;
	const map<string, DocxCompilerItem*>*				l_items;
	DocxCompilerItem*									l_item;
	DocxCompilerItem*									l_itemParent;
	DocxCompilerItemGroup*								l_itemGroup;

	size_t									l_groupId;
	double									l_size;
	double									l_pageSize;
	bool									l_keepTogether;
	bool									l_keepWithPrev;
	bool									l_filler;
	bool									l_spacer;
	bool									l_alternate;

	DocxCompilerBooleanField*				l_booleanField;
	string									l_name;
	DocxCompilerBooleanField::BooleanValue	l_value;
	size_t									l_len;
	bool									l_ok;

	xercesc::DOMDocument*					l_doc;
	xercesc::DOMElement*					l_bodyNode;
	xercesc::DOMNode*						l_parentNode;
	xercesc::DOMNode*						l_childNode;
	xercesc::DOMNode*						l_childNodeTemp;
	xercesc::DOMElement*					l_placeHolderNode;

	l_bookmarks	= m_file ->getBookmarksByName();
	l_items		= m_file ->getItemsByName();

	if ( !p_bookmark )
		l_childBookmarks = &m_rootBookmarks;
	else
		l_childBookmarks = p_bookmark ->getChildren();

	if ( !p_itemPath ->empty() )
		l_itemParent = p_itemPath ->back();
	else
		l_itemParent = NULL;

	l_itemGroup = NULL;

	FOR_EACH( l_childBookmarkIterator, l_childBookmarks )
	{
		l_childBookmark = *l_childBookmarkIterator;
		l_parentNode	= l_childBookmark ->getStartNode() ->getParentNode();

		l_name	= l_childBookmark ->getName();
		l_len	= l_name.length();
		l_ok	= false;

		if ( l_len > 7 && l_name.substr( l_len - 7 ) == "_notset" )
		{
			l_name	= l_name.substr( 0, l_len - 7 );
			l_value	= DocxCompilerBooleanField::BOOLEAN_UNDEFINED;
			l_ok	= true;
		}

		else
		if ( l_len > 4 && l_name.substr( l_len - 4 ) == "_yes" )
		{
			l_name	= l_name.substr( 0, l_len - 4 );
			l_value	= DocxCompilerBooleanField::BOOLEAN_TRUE;
			l_ok	= true;
		}

		else
		if ( l_len > 3 && l_name.substr( l_len - 3 ) == "_no" )
		{
			l_name	= l_name.substr( 0, l_len - 3 );
			l_value	= DocxCompilerBooleanField::BOOLEAN_FALSE;
			l_ok	= true;
		}

		if ( l_ok )
		{
			l_placeHolderNode = m_bodyNode ->getOwnerDocument() ->createElement( _X( "placeholder" ) );
			l_parentNode ->insertBefore( l_placeHolderNode, l_childBookmark ->getStartNode() );

			l_booleanField = new DocxCompilerBooleanField(
				l_name,
				l_placeHolderNode,
				l_value );

			l_doc		= l_booleanField ->getDoc();
			l_bodyNode	= ( xercesc::DOMElement* ) l_doc ->importNode( m_bodyNode, false );
			l_doc ->appendChild( l_bodyNode );

			l_placeHolderNode ->setAttribute( _X( "type" ), _X( "boolean" ) );
			l_placeHolderNode ->setAttribute( _X( "ptr" ), Utf8ToXMLCh( StrFunc::intToStr( ( size_t ) l_booleanField ) ) );

			while ( true )
			{
				l_childNode	= l_childBookmark ->getStartNode() ->getNextSibling();

				if ( l_childNode == l_childBookmark ->getEndNode() )
					break;

				l_childNodeTemp = l_doc ->importNode( l_childNode, true );
				l_bodyNode ->appendChild( l_childNodeTemp );

				l_parentNode ->removeChild( l_childNode );
			}
		}

		else
		{
			DocxCompilerItem::getItemOptions(
				l_name,
				l_size,
				l_pageSize,
				l_keepTogether,
				l_keepWithPrev,
				l_filler,
				l_spacer,
				l_alternate );

			if ( l_name != l_childBookmark ->getName() )
			{
				l_ok = true;

				l_bookmarkIterator = l_bookmarks ->find( l_name );
				if( l_bookmarkIterator != l_bookmarks ->end() )
					l_ok = false;

				if ( l_ok )
				{
					l_itemIterator = l_items ->find( l_name );
					if( l_itemIterator != l_items ->end() )
						l_ok = false;
				}

				if ( !l_ok )
				{
					l_name			= l_childBookmark ->getName();
					l_size			= 0.0f;
					l_pageSize		= 0.0f;
					l_keepTogether	= false;
					l_keepWithPrev	= false;
					l_filler		= false;
					l_spacer		= false;
					l_alternate		= false;
				}
			}



			if ( !l_itemGroup )
			{
				l_groupId		= l_childBookmark ->getGroupId();
				l_itemGroup		= new DocxCompilerItemGroup( this, l_itemParent );

				m_itemGroups.push_back( l_itemGroup );
				m_file ->insertItemGroup( l_itemGroup );
			}

			l_item = new DocxCompilerItem( 
				this, 
				l_itemGroup, 
				l_name, 
				l_size,
				l_pageSize,
				l_keepTogether,
				l_keepWithPrev,
				l_filler,
				l_spacer,
				l_alternate );

			insertItem( l_item );
			m_file ->insertItem( l_item );

			l_doc		= l_item ->getDoc();
			l_bodyNode	= l_item ->getBodyNode();



			if ( !l_childBookmark ->getChildren() ->empty() )
			{
				p_itemPath ->push_back( l_item );
				loadItemsByBookmark( l_childBookmark, p_itemPath );
				p_itemPath ->pop_back();
			}

			while ( true )
			{
				l_childNode	= l_childBookmark ->getStartNode() ->getNextSibling();

				if ( l_childNode == l_childBookmark ->getEndNode() )
					break;

				l_childNodeTemp = l_doc ->importNode( l_childNode, true );
				l_bodyNode ->appendChild( l_childNodeTemp );

				l_parentNode ->removeChild( l_childNode );
			}



			l_nextChildBookmarkIterator	= l_childBookmarkIterator;
			l_nextChildBookmarkIterator++;

			if( l_nextChildBookmarkIterator == l_childBookmarks ->end() )
				l_nextChildBookmark = NULL;

			else
				l_nextChildBookmark = ( *l_nextChildBookmarkIterator );

			if ( !l_nextChildBookmark 
			   || l_nextChildBookmark ->getGroupId() != l_groupId
			   || l_nextChildBookmark ->getName().length() > 7 && l_nextChildBookmark ->getName().substr( l_nextChildBookmark ->getName().length() - 7 ) == "_notset"
			   || l_nextChildBookmark ->getName().length() > 4 && l_nextChildBookmark ->getName().substr( l_nextChildBookmark ->getName().length() - 4 ) == "_yes"
			   || l_nextChildBookmark ->getName().length() > 3 && l_nextChildBookmark ->getName().substr( l_nextChildBookmark ->getName().length() - 3 ) == "_no" )
			{
				l_placeHolderNode = m_bodyNode ->getOwnerDocument() ->createElement( _X( "placeholder" ) );
				l_parentNode ->insertBefore( l_placeHolderNode, l_childBookmark ->getStartNode() );

				l_placeHolderNode ->setAttribute( _X( "type" ), _X( "item-group" ) );
				l_placeHolderNode ->setAttribute( _X( "ptr" ), Utf8ToXMLCh( StrFunc::intToStr( ( size_t ) l_itemGroup ) ) );

				if( l_itemParent )
					l_itemParent ->insertChildItemGroup( l_itemGroup );

				l_itemGroup = NULL;
			}
		}

		l_parentNode ->removeChild( l_childBookmark ->getStartNode() );
		l_parentNode ->removeChild( l_childBookmark ->getEndNode() );
	}
} // loadItemsByBookmark

void DocxCompilerItemFile::loadItemLayout( DocxCompilerItem* p_item, DocxCompilerItem*& p_fillItem )
{
	set<DocxCompilerItemGroup*>::const_iterator		l_childItemGroupIterator;
	const set<DocxCompilerItemGroup*>*				l_childItemGroups;
	DocxCompilerItemGroup*							l_childItemGroup;

	list<DocxCompilerItem*>::const_iterator			l_childItemIterator;
	const list<DocxCompilerItem*>*					l_childItems;
	DocxCompilerItem*								l_childItem;
	DocxCompilerItem*								l_fillItem;

	l_childItemGroups = p_item ->getChildItemGroups();
	FOR_EACH( l_childItemGroupIterator, l_childItemGroups )
	{
		l_childItemGroup = *l_childItemGroupIterator;

		l_childItems = l_childItemGroup ->getChildItems();
		FOR_EACH( l_childItemIterator, l_childItems )
		{
			l_childItem	= *l_childItemIterator;
			l_fillItem	= NULL;

			loadItemLayout( l_childItem, l_fillItem );

			if(!p_fillItem && l_fillItem )
				p_fillItem = l_fillItem;
		}
	}

	p_item ->loadItemLayout();

	if ( !p_fillItem && p_item ->isFiller() )
	{
		if( p_item ->getItemLayout()
	   && ( p_item ->getItemLayout() ->getFixedTrCnt() > 0
		 ||!p_item ->getItemLayout() ->getTrList() ->empty() ) )

			p_fillItem = p_item;
	}
} // loadItemLayout

void DocxCompilerItemFile::setItemsPath( DocxCompilerItemGroup* p_itemGroup )
{
	list<DocxCompilerItem*>::const_iterator			l_childItemIterator;
	const list<DocxCompilerItem*>*					l_childItems;
	DocxCompilerItem*								l_childItem;

	set<DocxCompilerItemGroup*>::const_iterator		l_childItemGroupIterator; 
	const set<DocxCompilerItemGroup*>*				l_childItemGroups;
	DocxCompilerItemGroup*							l_childItemGroup;

	l_childItems = p_itemGroup ->getChildItems();
	FOR_EACH( l_childItemIterator, l_childItems )
	{
		l_childItem = *l_childItemIterator;
		l_childItem ->setPath();

		l_childItemGroups = l_childItem ->getChildItemGroups();
		FOR_EACH( l_childItemGroupIterator, l_childItemGroups )
		{
			l_childItemGroup = *l_childItemGroupIterator;
			setItemsPath( l_childItemGroup );
		}
	}
} // setItemsPath



void DocxCompilerItemFile::loadFields()
{
	typedef list<boost::tuple<xercesc::DOMElement*, xercesc::DOMElement*, xercesc::DOMText*, string>> NodeList_T;

	list<DocxCompilerItem*>::iterator			l_itemIterator;
	DocxCompilerItem*							l_item;

	list<DocxCompilerField*>::iterator			l_fieldIterator;
	list<DocxCompilerField*>					l_altChunkFields;
	list<DocxCompilerField*>					l_barcodeFields;
	DocxCompilerBooleanField*					l_booleanField;

	DocxCompilerField*							l_field;
	DocxCompilerField::FieldType				l_fieldType;
	string										l_fieldName;
	string										l_fieldText;
	string										l_format;
	map<DocxCompilerField::FieldParam, string>	l_valueByParam;
	xercesc::DOMElement*						l_placeHolderNode;

	boost::scoped_ptr<XmlTreeDriller>	l_cursorDriller;
	xercesc::DOMElement*				l_cursorNode;

	boost::scoped_ptr<XmlTreeDriller>	l_treeDriller;

	NodeList_T				l_tNodeList;
	NodeList_T::iterator	l_tIterator;
	NodeList_T::iterator	l_tStart;
	NodeList_T::iterator	l_tEnd;
	NodeList_T::iterator	l_tNext;

	xercesc::DOMDocument*	l_doc;
	xercesc::DOMElement*	l_bodyNode;
	xercesc::DOMElement*	l_drawingNode;
	xercesc::DOMElement*	l_chartNode;
	xercesc::DOMElement*	l_pictNode;
	xercesc::DOMElement*	l_pNode;
	xercesc::DOMElement*	l_rNode;
	xercesc::DOMElement*	l_tNode;
	xercesc::DOMText*		l_textNode;
	string					l_textValue;

	xercesc::DOMElement*	l_parentNode;
	xercesc::DOMNode*		l_node;
	XmlFunc::TagPos			l_nodePos;

	OpcRelationships*		l_relationships;
	OpcRelationship*		l_relationship;
	OpcPart*				l_part;

	set<string>				l_inuseFileRIds;
	set<string>				l_deleteFileRIds;
	set<string>::iterator	l_fileRIdIterator;
	string					l_fileRId;

	size_t	l_tEndIndex;
	size_t	l_tStartIndex;
	bool	l_tStartFound;
	bool	l_found;
	bool	l_escape;

	string	l_str;
	char	l_ch;
	char	l_chNext;

	size_t	l_len;
	size_t	l_pos;
	size_t	i;

	try
	{
		FOR_EACH( l_itemIterator, &m_items )
		{
			l_item		= *l_itemIterator;
			l_doc		= l_item ->getDoc();
			l_bodyNode	= l_item ->getBodyNode();

			l_cursorDriller.reset( new XmlTreeDriller(

				l_bodyNode,

				// i decided to drill the entire tree
				// to cover all cases like message boxes that have complex tree structures and others.

				// originally we did not drill all nodes to improve performance
				// but because the file is compiled only once it's not an issue.

				// "http://schemas.openxmlformats.org/wordprocessingml/2006/main", "p,r,tbl,tr,tc,hyperlink",

				"", "",
				"", "%text%,pict,drawing,placeholder",
				"", "", "" ) );

			while ( l_cursorNode = ( xercesc::DOMElement* ) l_cursorDriller ->nextNode() )
			{
				if ( XmlFunc::XMLChCmp( l_cursorNode ->getNodeName(), _X( "placeholder" ) )
				  && XmlFunc::XMLChCmp( l_cursorNode ->getAttribute( _X( "type" ) ), _X( "boolean" ) ) )
				{
					l_str			= XmlFunc::XMLChToUtf8( l_cursorNode ->getAttribute( _X( "ptr" ) ) );
					l_booleanField	= ( DocxCompilerBooleanField* ) StrFunc::strToInt<size_t>( l_str );

					l_parentNode	= l_booleanField ->getDoc() ->getDocumentElement();
					l_node			= l_parentNode;
					l_nodePos		= XmlFunc::TAG_START;

					l_drawingNode	= NULL;
					l_pictNode		= NULL;

					while ( XmlFunc::getNextTag( l_parentNode, l_node, l_nodePos, l_str ) )
					{
						if ( l_node	== l_parentNode )
							break;

						if ( XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "drawing" ) )
						  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
						{
								 if ( l_nodePos == XmlFunc::TAG_START )	l_drawingNode = ( xercesc::DOMElement* ) l_node;
							else if ( l_nodePos == XmlFunc::TAG_END )	l_drawingNode = NULL;
						}

						else
						if ( XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "pict" ) )
						  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
						{
								 if ( l_nodePos == XmlFunc::TAG_START )	l_pictNode = ( xercesc::DOMElement* ) l_node;
							else if ( l_nodePos == XmlFunc::TAG_END )	l_pictNode = NULL;
						}

						else
						if ( XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "blip" ) )
						  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/drawingml/2006/main" ) ) )
						{
							if ( l_drawingNode )
							{
								l_fileRId = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_node ) ->getAttributeNS(
									_X( "http://schemas.openxmlformats.org/officeDocument/2006/relationships" ),
									_X( "embed" ) ) );

								if ( l_fileRId.empty() )

								l_fileRId = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_node ) ->getAttributeNS(
									_X( "http://schemas.openxmlformats.org/officeDocument/2006/relationships" ),
									_X( "link" ) ) );

								if ( !l_fileRId.empty()
								  && l_inuseFileRIds.find( l_fileRId ) == l_inuseFileRIds.end() )
									 l_inuseFileRIds.insert( l_fileRId );
							}
						}

						else
						if ( XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "imagedata" ) )
						  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "urn:schemas-microsoft-com:vml" ) ) )
						{
							if ( l_pictNode )
							{
								l_fileRId = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_node ) ->getAttributeNS(
									_X( "http://schemas.openxmlformats.org/officeDocument/2006/relationships" ),
									_X( "id" ) ) );

								if ( !l_fileRId.empty()
								  && l_inuseFileRIds.find( l_fileRId ) == l_inuseFileRIds.end() )
									 l_inuseFileRIds.insert( l_fileRId );
							}
						}

						l_booleanField ->concatStr( l_str );
					}

					l_booleanField ->setItem( l_item );
					insertField( l_booleanField );
				}



				else
				if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::TEXT_NODE
				  && XmlFunc::XMLChCmp( l_cursorNode ->getParentNode() ->getLocalName(),	_X( "t" ) )
				  && XmlFunc::XMLChCmp( l_cursorNode ->getParentNode() ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
				{
					l_textNode	= ( xercesc::DOMText* )	l_cursorNode;
					l_textValue	= XmlFunc::XMLChToUtf8( l_textNode ->getNodeValue() );

					if ( strchr( l_textValue.c_str(), '{' ) )
					{
						l_tNodeList.clear();

						l_tNode	= ( xercesc::DOMElement* ) l_textNode ->getParentNode();
						l_rNode	= ( xercesc::DOMElement* ) l_tNode ->getParentNode();
						l_pNode	= ( xercesc::DOMElement* ) l_rNode ->getParentNode();

						while ( true )
						{
							l_tNodeList.push_back( boost::make_tuple( l_rNode, l_tNode, l_textNode, l_textValue ) );

							l_tNode = ( xercesc::DOMElement* ) l_tNode ->getNextSibling();
							if ( !l_tNode )
							{
								l_rNode = ( xercesc::DOMElement* ) l_rNode ->getNextSibling();

								if ( !l_rNode
								  || l_rNode ->getNodeType() != xercesc::DOMNode::ELEMENT_NODE
								  || !XmlFunc::XMLChCmp( l_rNode ->getLocalName(),		_X( "r" ) )
								  || !XmlFunc::XMLChCmp( l_rNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
									break;

								l_tNode = ( xercesc::DOMElement* ) l_rNode ->getFirstChild(); // rpr
								l_tNode = ( xercesc::DOMElement* ) l_tNode ->getNextSibling();
							}

							if ( !l_tNode
							  || l_tNode ->getNodeType() != xercesc::DOMNode::ELEMENT_NODE
							  || !XmlFunc::XMLChCmp( l_tNode ->getLocalName(),		_X( "t" ) )
							  || !XmlFunc::XMLChCmp( l_tNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
								break;

							l_textNode	= ( xercesc::DOMText* ) l_tNode ->getFirstChild();
							l_textValue	= XmlFunc::XMLChToUtf8( l_textNode ->getNodeValue() );
						}



						l_tStart		= l_tNodeList.end();
						l_tStartIndex	= 0;
						l_tStartFound	= false;
						l_found			= false;
						l_escape		= false;

						l_fieldText		= "";

						FOR_EACH( l_tIterator, &l_tNodeList )
						{
							l_textValue	= get<3>( *l_tIterator );

							l_len		= l_textValue.length();
							l_pos		= ( l_escape ? 1 : 0 );
							l_escape	= false;

							while ( l_pos < l_len )
							{
								l_ch		= l_textValue[ l_pos ];
								l_chNext	= '\0';

								if ( l_pos + 1 < l_len )
									l_chNext = l_textValue[ l_pos + 1 ];

								else
								{
									l_tNext = l_tIterator;
									l_tNext++;

									if ( l_tNext != l_tNodeList.end() )
										 l_chNext = ( get<3>( *l_tIterator ) )[0];
								}

								switch ( l_ch )
								{
								case '{':

									if ( l_chNext == l_ch )
									{
										if ( l_pos == l_len - 1 )
											l_escape = true;
									}

									else
									{
										l_fieldText		= l_ch;

										l_tStart		= l_tIterator;
										l_tStartIndex	= l_pos;
										l_tStartFound	= true;
									}

									break;

								case '}':

									if ( l_chNext == l_ch )
									{
										if ( l_pos == l_len - 1 )
											l_escape = true;
									}

									else
									{
										l_fieldText	+= l_ch;

										if ( l_tStartFound )
										{
											l_tEnd		= l_tIterator;
											l_tEndIndex	= l_pos;
											l_found		= true;

											goto _fieldBreak;
										}
									}

									break;

								default	:

									if ( l_tStartFound )
										l_fieldText += l_ch;

									break;
								}

								l_pos++;
							}
						}

						_fieldBreak:

						l_fieldName	= "";
						l_format	= "";

						if ( l_found )
						{
							l_fieldText	= l_fieldText.substr( 1, l_fieldText.length() - 2 );
							l_fieldText	= StrFunc::trim( l_fieldText );

							i = l_fieldText.find( ' ' );
							if ( i != l_fieldText.npos )
							{
								l_fieldName = StrFunc::rightTrim( l_fieldText.substr( 0, i ) );
								l_format	= StrFunc::leftTrim ( l_fieldText.substr( i + 1 ) );
							}

							else
							{
								l_fieldName	= l_fieldText;
								l_format	= "";
							}
						}

						l_placeHolderNode = NULL;

						if ( !l_fieldName.empty() )
						{
							if ( l_tStart == l_tEnd  )
							{
								l_rNode		= get<0>( *l_tStart );
								l_tNode		= get<1>( *l_tStart );
								l_textNode	= get<2>( *l_tStart );
								l_textValue	= get<3>( *l_tStart );

								if ( l_tStartIndex > 0 )
								{
									l_str = l_textValue.substr( 0, l_tStartIndex );
									l_textNode ->setNodeValue( Utf8ToXMLCh( l_str ) );
									get<3>( *l_tStart ) = l_str;
								}

								else
								{
									l_tNode ->removeChild( l_textNode );
									get<3>( *l_tStart ) = "";
								}

								l_placeHolderNode = l_doc ->createElement( _X( "placeholder" ) );
								l_tNode ->appendChild( l_placeHolderNode );

								if ( l_tEndIndex + 1 < l_textValue.length() )
								{
									l_str = l_textValue.substr( l_tEndIndex + 1 );

									l_textNode = l_doc ->createTextNode( Utf8ToXMLCh( l_str ) );
									l_tNode ->appendChild( l_textNode );
								}
							}

							else
							{
								l_rNode		= get<0>( *l_tStart );
								l_tNode		= get<1>( *l_tStart );
								l_textNode	= get<2>( *l_tStart );
								l_textValue	= get<3>( *l_tStart );

								if ( l_tStartIndex > 0 )
								{
									l_textValue = l_textValue.substr( 0, l_tStartIndex );
									l_textNode ->setNodeValue( Utf8ToXMLCh( l_textValue ) );
									get<3>( *l_tStart ) = l_textValue;
								}

								else
								{
									l_tNode ->removeChild( l_textNode );
									get<3>( *l_tStart ) = "";
								}

								l_placeHolderNode = l_doc ->createElement( _X( "placeholder" ) );
								l_tNode ->appendChild( l_placeHolderNode );

								l_tIterator = l_tStart;
								l_tIterator++;

								for (; l_tIterator != l_tEnd; l_tIterator++ )
								{
									l_rNode		= get<0>( *l_tIterator );
									l_tNode		= get<1>( *l_tIterator );
									l_textNode	= get<2>( *l_tIterator );

									l_tNode ->removeChild( l_textNode );

									if( l_tNode ->getChildNodes() ->getLength() == 0 ) {
										l_rNode ->removeChild( l_tNode );

										if ( l_rNode ->getChildNodes() ->getLength() == 1 /* rpr */ )
											 l_pNode ->removeChild( l_rNode );
									}

									get<3>( *l_tIterator ) = "";
								}

								l_rNode		= get<0>( *l_tEnd );
								l_tNode		= get<1>( *l_tEnd );
								l_textNode	= get<2>( *l_tEnd );
								l_textValue	= get<3>( *l_tEnd );

								if ( l_tEndIndex + 1 < l_textValue.length() )
								{
									l_textValue = l_textValue.substr( l_tEndIndex + 1 );
									l_textNode ->setNodeValue( Utf8ToXMLCh( l_textValue ) );
									get<3>( *l_tEnd ) = l_textValue;
								}

								else
								{
									l_tNode ->removeChild( l_textNode );

									if( l_tNode ->getChildNodes() ->getLength() == 0 ) {
										l_rNode ->removeChild( l_tNode );

										if ( l_rNode ->getChildNodes() ->getLength() == 1 /* rpr */ )
											 l_pNode ->removeChild( l_rNode );
									}

									get<3>( *l_tEnd ) = "";
								}
							}

							l_tIterator = l_tNodeList.end();
							l_tIterator--;

							while ( l_tIterator != l_tStart )
							{
								l_tIterator--;
								l_tNodeList.pop_back();
							}

							if ( get<3>( *l_tStart ).empty() )
								l_tNodeList.pop_back();

							DocxCompilerField::getTypeAndFormat( 
								l_format,
								l_fieldType,
								&l_valueByParam );

							switch ( l_fieldType )
							{
							case DocxCompilerField::TYPE_TEXT:

								l_field = new DocxCompilerTextField(
									l_item,
									l_fieldName,
									l_format,
									&l_valueByParam,
									l_placeHolderNode );

								l_placeHolderNode ->setAttribute( _X( "type" ), _X( "text" ) );
								l_placeHolderNode ->setAttribute( _X( "ptr" ), Utf8ToXMLCh( StrFunc::intToStr( ( size_t ) l_field ) ) );
											
								insertField( l_field );
								break;

							case DocxCompilerField::TYPE_NUMBER:

								l_field = new DocxCompilerNumberField(
									l_item,
									l_fieldName,
									l_format,
									&l_valueByParam,
									l_placeHolderNode );

								l_placeHolderNode ->setAttribute( _X( "type" ), _X( "number" ) );
								l_placeHolderNode ->setAttribute( _X( "ptr" ), Utf8ToXMLCh( StrFunc::intToStr( ( size_t ) l_field ) ) );

								insertField( l_field );
								break;

							case DocxCompilerField::TYPE_DATE_TIME:

								l_field = new DocxCompilerDateTimeField(
									l_item,
									l_fieldName,
									l_format,
									&l_valueByParam,
									l_placeHolderNode );

								l_placeHolderNode ->setAttribute( _X( "type" ), _X( "datetime" ) );
								l_placeHolderNode ->setAttribute( _X( "ptr" ), Utf8ToXMLCh( StrFunc::intToStr( ( size_t ) l_field ) ) );

								insertField( l_field );
								break;

							// there are xml manipulation that can disturb the xml iteration when creating altchunk and barcode fields.
							// so the fields are saved and created afterwards.

							// note that the placeholder attributes are not set because the the field created here is only a dummy.
							// when we create the actual field later we can set the ptr attribute.

							case DocxCompilerField::TYPE_ALT_CHUNK:

								l_field = new DocxCompilerField(
									l_item,
									l_fieldType,
									l_fieldName,
									l_format,
									&l_valueByParam,
									l_placeHolderNode );

								l_altChunkFields.push_back( l_field );
								break;

							case DocxCompilerField::TYPE_BARCODE:

								l_field = new DocxCompilerField(
									l_item,
									l_fieldType,
									l_fieldName,
									l_format,
									&l_valueByParam,
									l_placeHolderNode );

								l_barcodeFields.push_back( l_field );
								break;
							}

						}



						FOR_EACH( l_tIterator, &l_tNodeList )
						{
							l_textNode	= get<2>( *l_tIterator );
							l_textValue	= get<3>( *l_tIterator );
							
							l_str		= "";
							l_len		= l_textValue.length();
							l_pos		= 0;
							
							while ( l_pos < l_len )
							{
								l_ch		= l_textValue[ l_pos ];
								l_chNext	= '\0';

								if ( l_pos + 1 < l_len )
									l_chNext = l_textValue[ l_pos + 1 ];

								else
								{
									l_tNext = l_tIterator;
									l_tNext++;

									if ( l_tNext != l_tNodeList.end() )
										 l_chNext = ( get<3>( *l_tIterator ) )[0];
								}

								switch ( l_ch )
								{
								case '{': if ( l_chNext != l_ch ) l_str += l_ch; break;
								case '}': if ( l_chNext != l_ch ) l_str += l_ch; break;
								default	: l_str += l_ch; break;
								}

								l_pos++;
							}

							l_textValue = l_str;
							l_textNode ->setNodeValue( Utf8ToXMLCh( l_textValue ) );
							get<3>( *l_tIterator ) = l_textValue;
						}

						if( l_placeHolderNode )
							l_cursorDriller ->setCurrentNode( l_placeHolderNode );
						else
							l_cursorDriller ->setCurrentNode( get<2>( l_tNodeList.back() ) );

 						/***
						l_cursorNode ->removeChild( l_textNode );

						l_text		= "";
						l_fieldText	= "";

						l_len		= l_textValue.length();
						l_pos		= 0;

						while ( l_pos < l_len )
						{
							l_ch = l_textValue[ l_pos ];

							if ( l_ch == '{' && l_pos + 1 < l_len && l_textValue[ l_pos + 1 ] == '{' )
							{
								l_text	+= l_ch;
								l_pos	+= 2;
							}

							else 
							if ( l_ch == '}' && l_pos + 1 < l_len && l_textValue[ l_pos + 1 ] == '}' )
							{
								l_text	+= l_ch;
								l_pos	+= 2;
							}

							else 
							if ( l_ch == '{' )
							{
								l_fieldText += l_ch;
								l_pos		+= 1;
								l_ok		= false;

								while ( l_pos < l_len )
								{
									l_ch = l_textValue[ l_pos ];
									if ( l_ch == '{' && l_pos + 1 < l_len && l_textValue[ l_pos + 1 ] == '{' )
									{
										l_fieldText	+= l_ch;
										l_pos		+= 2;
									}

									else
									if ( l_ch == '}' && l_pos + 1 < l_len && l_textValue[ l_pos + 1 ] == '}' )
									{
										l_fieldText	+= l_ch;
										l_pos		+= 2;
									}

									else
									if ( l_ch == '{' )
									{
										l_fieldText += l_ch;
										l_pos		+= 1;
										l_ok		= false;

										break;
									}

									else
									if ( l_ch == '}' )
									{
										l_fieldText += l_ch;
										l_pos		+= 1;
										l_ok		= true;

										break;
									}

									else
									{
										l_fieldText += l_ch;
										l_pos		+= 1;
									}
								}

								l_fieldName	= "";
								l_format	= "";

								if ( l_ok )
								{
									l_fieldText	= l_fieldText.substr( 1, l_fieldText.length() - 2 );
									l_fieldText	= StrFunc::trim( l_fieldText );

									i = l_fieldText.find( ' ' );
									if ( i != l_fieldText.npos )
									{
										l_fieldName = StrFunc::rightTrim( l_fieldText.substr( 0, i ) );
										l_format	= StrFunc::leftTrim ( l_fieldText.substr( i + 1 ) );
									}

									else
									{
										l_fieldName	= l_fieldText;
										l_format	= "";
									}
								}

								if ( !l_fieldName.empty() )
								{
									if ( !l_text.empty() )
									{
										l_textNode = l_doc ->createTextNode( Utf8ToXMLCh( l_text ) );
										l_cursorNode ->appendChild( l_textNode );
									}

									l_placeHolderNode = l_doc ->createElement( _X( "placeholder" ) );
									l_cursorNode ->appendChild( l_placeHolderNode );

									DocxCompilerField::getTypeAndFormat( 
										l_format,
										l_fieldType,
										l_valueByParam );

									switch ( l_fieldType )
									{
									case DocxCompilerField::TYPE_TEXT:

										l_field = new DocxCompilerTextField(
											l_item,
											l_fieldName,
											l_format,
											l_valueByParam,
											l_placeHolderNode );

										l_placeHolderNode ->setAttribute( _X( "type" ), _X( "text" ) );
										l_placeHolderNode ->setAttribute( _X( "ptr" ), Utf8ToXMLCh( StrFunc::intToStr( ( size_t ) l_field ) ) );
											
										insertField( l_field );
										break;

									case DocxCompilerField::TYPE_NUMBER:

										l_field = new DocxCompilerNumberField(
											l_item,
											l_fieldName,
											l_format,
											l_valueByParam,
											l_placeHolderNode );

										l_placeHolderNode ->setAttribute( _X( "type" ), _X( "number" ) );
										l_placeHolderNode ->setAttribute( _X( "ptr" ), Utf8ToXMLCh( StrFunc::intToStr( ( size_t ) l_field ) ) );

										insertField( l_field );
										break;

									case DocxCompilerField::TYPE_DATE_TIME:

										l_field = new DocxCompilerDateTimeField(
											l_item,
											l_fieldName,
											l_format,
											l_valueByParam,
											l_placeHolderNode );

										l_placeHolderNode ->setAttribute( _X( "type" ), _X( "datetime" ) );
										l_placeHolderNode ->setAttribute( _X( "ptr" ), Utf8ToXMLCh( StrFunc::intToStr( ( size_t ) l_field ) ) );

										insertField( l_field );
										break;

									// there are xml manipulation that can disturb the xml iteration when creating altchunk and barcode fields.
									// so the fields are saved and created after the xml iteration.

									case DocxCompilerField::TYPE_ALT_CHUNK:

										l_field = new DocxCompilerField(
											l_item,
											l_fieldType,
											l_fieldName,
											l_format,
											l_valueByParam,
											l_placeHolderNode );

										l_altChunkFields.push_back( l_field );
										break;

									case DocxCompilerField::TYPE_BARCODE:

										l_field = new DocxCompilerField(
											l_item,
											l_fieldType,
											l_fieldName,
											l_format,
											l_valueByParam,
											l_placeHolderNode );

										l_barcodeFields.push_back( l_field );
										break;
									}

									l_text		= "";
									l_fieldText	= "";
								}

								else
								{
									l_text	   += l_fieldText;
									l_fieldText	= "";
								}
							}

							else
							{
								l_text += l_ch;
								l_pos++;
							}
						}
					}

					if ( !l_text.empty() )
					{
						l_textNode = l_doc ->createTextNode( Utf8ToXMLCh( l_text ) );
						l_cursorNode ->appendChild( l_textNode );
					}
					***/

					}
				}


				else
				if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "drawing" ) )
				  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
				{
					l_fieldText	= "";

					l_treeDriller.reset( new XmlTreeDriller(

						l_cursorNode,
						"", "",
						"http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing", "docPr",
						"", "", "" ) );

					if ( l_node = l_treeDriller ->nextNode() )
					{
						l_fieldText = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_node ) ->getAttribute( _X( "title" ) ) );
						l_fieldText = StrFunc::trim( l_fieldText );

						// alt text has to be covered by funny brackets {}
						// otherwise every picture with alt text will be removed and made a pic field.

						if ( l_fieldText.length() < 2
						  || l_fieldText[0] != '{'
						  || l_fieldText[ l_fieldText.length() - 1 ] != '}' )
							 l_fieldText = "";

						if ( l_fieldText.empty() )
						{
							l_fieldText = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_node ) ->getAttribute( _X( "descr" ) ) );
							l_fieldText = StrFunc::trim( l_fieldText );

							if ( l_fieldText.length() < 2
							  || l_fieldText[0] != '{'
							  || l_fieldText[ l_fieldText.length() - 1 ] != '}' )
								 l_fieldText = "";
						}
					}

					if ( l_fieldText.empty() )
					{
						l_treeDriller.reset( new XmlTreeDriller(

							l_cursorNode,
							"", "",
							"http://schemas.openxmlformats.org/drawingml/2006/picture", "cNvPr",
							"", "", "" ) );

						if ( l_node = l_treeDriller ->nextNode() )
						{
							l_fieldText = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_node ) ->getAttribute( _X( "title" ) ) );
							l_fieldText = StrFunc::trim( l_fieldText );

							if ( l_fieldText.length() < 2
							  || l_fieldText[0] != '{'
							  || l_fieldText[ l_fieldText.length() - 1 ] != '}' )
								 l_fieldText = "";

							if ( l_fieldText.empty() )
							{
								l_fieldText = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_node ) ->getAttribute( _X( "descr" ) ) );
								l_fieldText = StrFunc::trim( l_fieldText );

								if ( l_fieldText.length() < 2
								  || l_fieldText[0] != '{'
								  || l_fieldText[ l_fieldText.length() - 1 ] != '}' )
									 l_fieldText = "";
							}
						}
					}

					l_fieldName	= "";
					l_format	= "";

					if ( !l_fieldText.empty() )
					{
						l_fieldText = StrFunc::trim( l_fieldText.substr( 1, l_fieldText.length() - 2 ) );

						i = l_fieldText.find( ' ' );
						if ( i != l_fieldText.npos )
						{
							l_fieldName = StrFunc::rightTrim( l_fieldText.substr( 0, i ) );
							l_format	= StrFunc::leftTrim ( l_fieldText.substr( i + 1 ) );
						}

						else
						{
							l_fieldName	= l_fieldText;
							l_format	= "";
						}
					}

					l_treeDriller.reset( new XmlTreeDriller(

						l_cursorNode,
						"", "",
						"http://schemas.openxmlformats.org/drawingml/2006/chart", "chart",
						"", "", "" ) );

					l_chartNode = ( xercesc::DOMElement* ) l_treeDriller ->nextNode();

					if ( !l_fieldName.empty() )
					{
						l_placeHolderNode = l_doc ->createElement( _X( "placeholder" ) );

						l_node = l_cursorNode ->getParentNode();
						l_node ->insertBefore( l_placeHolderNode, l_cursorNode );

						if ( !l_chartNode )
						{
							DocxCompilerPicField::getFormat(
								l_format,
								&l_valueByParam );

							l_field = new DocxCompilerPicField(
								l_item,
								l_fieldName,
								l_format,
								&l_valueByParam,
								l_placeHolderNode,
								l_cursorNode );

							l_placeHolderNode ->setAttribute( _X( "type" ), _X( "pic" ) );
							l_placeHolderNode ->setAttribute( _X( "ptr" ), Utf8ToXMLCh( StrFunc::intToStr( ( size_t ) l_field ) ) );

							l_fileRId = ( ( DocxCompilerPicField* ) l_field ) ->getFileRId();

							if ( !l_fileRId.empty()
							  && l_deleteFileRIds.find( l_fileRId ) == l_deleteFileRIds.end() )
								 l_deleteFileRIds.insert( l_fileRId );
						}

						else
						{
							l_fileRId = XmlFunc::XMLChToUtf8( l_chartNode ->getAttribute( Utf8ToXMLCh( m_relMlPrefix + ":id" ) ) );

							l_relationships	= m_part ->getRelationships();
							l_relationship	= l_relationships ->getRelationshipsById() ->find( l_fileRId ) ->second;

							l_part = l_relationship ->getTargetPart();
							l_part ->loadDoc();

							switch ( DocxCompilerChartField::getChartType( l_part ) )
							{
							case DocxCompilerChartField::CHART_SERIES:

								l_field	= new DocxCompilerChartFieldSeries(
									l_item,
									l_fieldName,
									l_placeHolderNode,
									l_cursorNode,
									l_part );

								break;

							case DocxCompilerChartField::CHART_SINGLE_SERIES:

								l_field	= new DocxCompilerChartFieldSingleSeries(
									l_item,
									l_fieldName,
									l_placeHolderNode,
									l_cursorNode,
									l_part );

								break;

							case DocxCompilerChartField::CHART_FIXED_SERIES:

								l_field	= new DocxCompilerChartFieldFixedSeries(
									l_item,
									l_fieldName,
									l_placeHolderNode,
									l_cursorNode,
									l_part );

								break;

							case DocxCompilerChartField::CHART_X_Y:

								l_field	= new DocxCompilerChartFieldXY(
									l_item,
									l_fieldName,
									l_placeHolderNode,
									l_cursorNode,
									l_part );

								break;

							case DocxCompilerChartField::CHART_X_Y_SIZE:

								l_field	= new DocxCompilerChartFieldXYSize(
									l_item,
									l_fieldName,
									l_placeHolderNode,
									l_cursorNode,
									l_part );

								break;
							}

							l_placeHolderNode ->setAttribute( _X( "type" ), _X( "chart" ) );
							l_placeHolderNode ->setAttribute( _X( "ptr" ), Utf8ToXMLCh( StrFunc::intToStr( ( size_t ) l_field ) ) );

							l_relationships ->deleteRelationship( l_relationship );
						}

						insertField( l_field );
						l_cursorDriller ->deleteNode();
					}

					else
					{
						if ( !l_chartNode )
						{
							l_treeDriller.reset( new XmlTreeDriller(

								l_cursorNode,
								"", "",
								"http://schemas.openxmlformats.org/drawingml/2006/main",				"blip",
								"http://schemas.openxmlformats.org/officeDocument/2006/relationships",	"embed,link", "" ) );

							if ( l_node = l_treeDriller ->nextNode() )
							{
								l_fileRId = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_node ) ->getAttributeNS(
									_X( "http://schemas.openxmlformats.org/officeDocument/2006/relationships" ),
									_X( "embed" ) ) );

								if ( l_fileRId.empty() )

								l_fileRId = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_node ) ->getAttributeNS(
									_X( "http://schemas.openxmlformats.org/officeDocument/2006/relationships" ),
									_X( "link" ) ) );

								if ( !l_fileRId.empty()
								  && l_inuseFileRIds.find( l_fileRId ) == l_inuseFileRIds.end() )
									 l_inuseFileRIds.insert( l_fileRId );
							}
						}
					}
				}



				// compatibility mode pictures (vml:pict) are not supported because of their performance issues.
				// documents with even hundreds of pictures could take many minutes to open.

				// pict are only read to collect picture relationships
				// which are needed for deleting picture field relationships.

				else
				if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "pict" ) )
				  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
				{
					l_treeDriller.reset( new XmlTreeDriller(

						l_cursorNode,
						"", "",
						"urn:schemas-microsoft-com:vml", "imagedata",
						"http://schemas.openxmlformats.org/officeDocument/2006/relationships", "id", "" ) );

					if ( l_node = l_treeDriller ->nextNode() )
					{
						l_fileRId = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_node ) ->getAttributeNS(
							_X( "http://schemas.openxmlformats.org/officeDocument/2006/relationships" ),
							_X( "id" ) ) );

						if ( !l_fileRId.empty()
						  && l_inuseFileRIds.find( l_fileRId ) == l_inuseFileRIds.end() )
							 l_inuseFileRIds.insert( l_fileRId );
					}
				}
			}
		}

		// delete all the picture field relationships that are not in use by any other non-field pictures
		// this can only be done after going through all the pictures in the part.

		FOR_EACH( l_fileRIdIterator, &l_deleteFileRIds )
		{
			l_fileRId = *l_fileRIdIterator;

			if ( l_inuseFileRIds.find( l_fileRId ) == l_inuseFileRIds.end() )
			{
				l_relationships	= m_part ->getRelationships();
				l_relationship	= l_relationships ->getRelationshipsById() ->find( l_fileRId ) ->second;

				if ( l_relationship )
					 l_relationships ->deleteRelationship( l_relationship );
			}
		}

		

		// after the xml iteration is done the altchunk and barcode fields can be created
		// and the xml manipulation will not disturb the xml iteration.
		
		FOR_EACH( l_fieldIterator, &l_altChunkFields )
		{
			l_field = new DocxCompilerAltChunkField(
				( *l_fieldIterator ) ->getItem(),
				( *l_fieldIterator ) ->getName(),
				( *l_fieldIterator ) ->getFormat(),
				( *l_fieldIterator ) ->getValueByParam(),
				( *l_fieldIterator ) ->getPlaceHolderNode() );

			l_placeHolderNode = ( *l_fieldIterator ) ->getPlaceHolderNode();
			l_placeHolderNode ->setAttribute( _X( "type" ), _X( "altChunk" ) );
			l_placeHolderNode ->setAttribute( _X( "ptr" ), Utf8ToXMLCh( StrFunc::intToStr( ( size_t ) l_field ) ) );

			insertField( l_field );
			delete *l_fieldIterator;
		}

		l_altChunkFields.clear();

		FOR_EACH( l_fieldIterator, &l_barcodeFields )
		{
			l_field = new DocxCompilerBarcodeField(
				( *l_fieldIterator ) ->getItem(),
				( *l_fieldIterator ) ->getName(),
				( *l_fieldIterator ) ->getFormat(),
				( *l_fieldIterator ) ->getValueByParam(),
				( *l_fieldIterator ) ->getPlaceHolderNode() );

			l_placeHolderNode = ( *l_fieldIterator ) ->getPlaceHolderNode();
			l_placeHolderNode ->setAttribute( _X( "type" ), _X( "barcode" ) );
			l_placeHolderNode ->setAttribute( _X( "ptr" ), Utf8ToXMLCh( StrFunc::intToStr( ( size_t ) l_field ) ) );

			insertField( l_field );
			delete *l_fieldIterator;
		}

		l_barcodeFields.clear();
	}

	catch ( ... )
	{
		FOR_EACH( l_fieldIterator, &l_altChunkFields )
		{
			delete *l_fieldIterator;
		}

		FOR_EACH( l_fieldIterator, &l_barcodeFields )
		{
			delete *l_fieldIterator;
		}

		FOR_EACH( l_fieldIterator, &m_fields )
		{
			delete *l_fieldIterator;
		}

		m_fields.clear();
		throw;
	}
} // loadFields

void DocxCompilerItemFile::loadStripes()
{
	list<DocxCompilerItemGroup*>::iterator	l_itemGroupIterator;
	DocxCompilerItemGroup*					l_itemGroup;

	FOR_EACH( l_itemGroupIterator, &m_itemGroups )
	{
		l_itemGroup = *l_itemGroupIterator;
		l_itemGroup ->loadStripes();
	}

	FOR_EACH( l_itemGroupIterator, &m_itemGroups )
	{
		l_itemGroup = *l_itemGroupIterator;

		if( l_itemGroup ->isStriped() )
			l_itemGroup ->createStripePlaceHolders();
	}
} // loadShades

void DocxCompilerItemFile::loadXmlStrings()
{
	list<DocxCompilerItem*>::iterator	l_itemIterator;
	DocxCompilerXmlString*				l_xmlString;
	size_t								i;

	trimItems();

	for ( i = 0; i < m_sectLen; i++ )
	{
		l_xmlString = new DocxCompilerXmlString(
			NULL,
			DocxCompilerXmlString::TYPE_SECTION,
			"",
			NULL );

		m_file ->insertXmlString( l_xmlString );
		m_sects.push_back( l_xmlString );
	}

	FOR_EACH( l_itemIterator, &m_items )
	{
		( *l_itemIterator ) ->loadXmlStrings();
	}
} // loadXmlStrings



void DocxCompilerItemFile::normalize()
{
	boost::scoped_ptr<XmlTreeDriller>	l_treeDriller;
	boost::scoped_ptr<XmlTreeDriller>	l_cursorDriller;
	xercesc::DOMElement*				l_cursorNode;

	xercesc::DOMDocument*				l_doc;
	xercesc::DOMElement*				l_rootNode;
	xercesc::DOMElement*				l_pNode;
	xercesc::DOMElement*				l_pNode2;
	xercesc::DOMElement*				l_rNode;
	xercesc::DOMElement*				l_rNode2;
	xercesc::DOMElement*				l_prNode;
	xercesc::DOMElement*				l_prNode2;
	xercesc::DOMElement*				l_fldChar;
	xercesc::DOMElement*				l_fldChar2;
	xercesc::DOMText*					l_textNode;

	xercesc::DOMNode*					l_parentNode;
	xercesc::DOMNode*					l_childNode;
	xercesc::DOMNode*					l_node;
	XmlFunc::TagPos						l_nodePos;
	string								l_stringValue;

	// size_t							l_idSeq = 0;
	string								l_str;
	bool								l_ok;

	m_part ->loadDoc();
	m_part ->setChangeStatus( OpcPart::PART_CHANGED_IN_DOM );

	l_doc			= m_part ->getDoc();
	l_rootNode		= l_doc ->getDocumentElement();

	m_wordMlPrefix	= XmlFunc::normalizeNsPrefix( l_rootNode, "w", "http://schemas.openxmlformats.org/wordprocessingml/2006/main" );
	m_relMlPrefix	= XmlFunc::normalizeNsPrefix( l_rootNode, "r", "http://schemas.openxmlformats.org/officeDocument/2006/relationships" );
	m_vmlPrefix		= XmlFunc::normalizeNsPrefix( l_rootNode, "v", "urn:schemas-microsoft-com:vml" );

	switch ( m_type )
	{
	case DOCUMENT:

		if ( !XmlFunc::XMLChCmp( l_rootNode ->getLocalName(),		_X( "document" ) )
		  || !XmlFunc::XMLChCmp( l_rootNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

			throw NodeNotFoundException( "document", __FILE__, __LINE__ );

		m_bodyNode = XmlFunc::getChildByTagName(
			l_rootNode,
			NULL,
			"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
			"body" );

		if ( !m_bodyNode )
			throw NodeNotFoundException( "body", __FILE__, __LINE__ );

		break;

	case HEADER:

		if ( !XmlFunc::XMLChCmp( l_rootNode ->getLocalName(),		_X( "hdr" ) )
		  || !XmlFunc::XMLChCmp( l_rootNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

			throw NodeNotFoundException( "hdr", __FILE__, __LINE__ );

		m_bodyNode = l_rootNode;
		break;

	case FOOTER:

		if ( !XmlFunc::XMLChCmp( l_rootNode ->getLocalName(),		_X( "ftr" ) )
		  || !XmlFunc::XMLChCmp( l_rootNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

			throw NodeNotFoundException( "ftr", __FILE__, __LINE__ );

		m_bodyNode = l_rootNode;
		break;
	}

	m_body1		= "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n";
	m_body2		= "";

	l_node		= l_rootNode;
	l_nodePos	= XmlFunc::TAG_BEFORE;

	while ( XmlFunc::getNextTag( l_rootNode, l_node, l_nodePos, l_stringValue ) )
	{
		if ( l_node == m_bodyNode )
			break;

		m_body1 += l_stringValue;
	}

	if ( l_nodePos == XmlFunc::TAG_EMPTY )
		m_body1 += l_stringValue.substr( 0, l_stringValue.length() - 2 ) + ">";

	else
		m_body1 += l_stringValue;

	m_body2 += "</"	+ XmlFunc::XMLChToUtf8( l_node ->getNodeName() ) + ">";
	l_nodePos = XmlFunc::TAG_AFTER;

	while ( XmlFunc::getNextTag( l_rootNode, l_node, l_nodePos, l_stringValue ) )
	{
		m_body2 += l_stringValue;
	}



	l_cursorDriller.reset( new XmlTreeDriller(
		m_bodyNode,

		// i decided to drill the entire tree
		// to cover all cases like message boxes that have complex tree structures and others.

		// originally we did not drill all nodes to improve performance
		// but because the file is compiled only once it's not an issue.

		// "http://schemas.openxmlformats.org/wordprocessingml/2006/main", "p,r,tbl,tr,tc,fldSimple",

		"", "",
		"http://schemas.openxmlformats.org/wordprocessingml/2006/main", "p,r,t,tbl,tr,tc,instrText,br,sectPr,sdt,ins,moveTo,del,delText,moveFrom,moveToRangeStart,moveToRangeEnd,moveFromRangeStart,moveFromRangeEnd,proofErr,lastRenderedPageBreak", // drawing
		"", "", "" ) );

	while ( l_cursorNode = ( xercesc::DOMElement* ) l_cursorDriller ->nextNode() )
	{
		if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "tbl" ) ) )
		{
			l_prNode = XmlFunc::getChildByTagName(
				l_cursorNode,
				NULL,
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				"tblPr" );

			if ( l_prNode )
			{
				if( l_cursorNode ->getFirstChild() != l_prNode )
				{
					l_cursorNode ->removeChild( l_prNode );

					if( l_cursorNode ->getFirstChild() )
						l_cursorNode ->insertBefore( l_prNode, l_cursorNode ->getFirstChild() );
					else
						l_cursorNode ->appendChild( l_prNode );
				}
			}

			else
			{
				l_prNode = l_doc ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "tblPr" ) );
				l_prNode ->setPrefix( Utf8ToXMLCh( m_wordMlPrefix ) );

				if( l_cursorNode ->getFirstChild() )
					l_cursorNode ->insertBefore( l_prNode, l_cursorNode ->getFirstChild() );
				else
					l_cursorNode ->appendChild( l_prNode );
			}

			l_prNode2 = XmlFunc::getChildByTagName(
				l_prNode,
				NULL,
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				"tblBorders" );

			if ( !l_prNode2 )
			{
				l_prNode2 = l_doc ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "tblBorders" ) );
				l_prNode2 ->setPrefix( Utf8ToXMLCh( m_wordMlPrefix ) );

				l_node = XmlFunc::getChildByTagName(
					l_prNode,
					NULL,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"shd,tblLayout,tblCellMar,tblLook" );

				if( l_node )
					l_prNode ->insertBefore( l_prNode2, l_node );
				else
					l_prNode ->appendChild( l_prNode2 );
			}

			l_prNode2 = XmlFunc::getChildByTagName(
				l_cursorNode,
				l_prNode,
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				"tblGrid" );

			if ( l_prNode2 )
			{
				if( l_prNode ->getNextSibling() != l_prNode2 )
				{
					l_cursorNode ->removeChild( l_prNode2 );

					if( l_prNode ->getNextSibling() )
						l_cursorNode ->insertBefore( l_prNode2, l_prNode ->getNextSibling() );
					else
						l_cursorNode ->appendChild( l_prNode2 );
				}
			}

			else
			{
				l_prNode2 = l_doc ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "tblGrid" ) );
				l_prNode2 ->setPrefix( Utf8ToXMLCh( m_wordMlPrefix ) );

				if( l_prNode ->getNextSibling() )
					l_cursorNode ->insertBefore( l_prNode2, l_prNode ->getNextSibling() );
				else
					l_cursorNode ->appendChild( l_prNode2 );
			}

			XmlFunc::removeAttrByNamespace(
				l_cursorNode,
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				"rsidP,rsidR,rsidTr,rsidRPr,rsidDel,rsidRDefault,rsidSect" );
		}

		else
		if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "tr" ) ) )
		{
			l_prNode = XmlFunc::getChildByTagName(
				l_cursorNode,
				NULL,
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				"tblPrEx" );

			if ( l_prNode )
			{
				if( l_cursorNode ->getFirstChild() != l_prNode )
				{
					l_cursorNode ->removeChild( l_prNode );

					if( l_cursorNode ->getFirstChild() )
						l_cursorNode ->insertBefore( l_prNode, l_cursorNode ->getFirstChild() );
					else
						l_cursorNode ->appendChild( l_prNode );
				}
			}

			else
			{
				l_prNode = l_doc ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "tblPrEx" ) );
				l_prNode ->setPrefix( Utf8ToXMLCh( m_wordMlPrefix ) );

				if( l_cursorNode ->getFirstChild() )
					l_cursorNode ->insertBefore( l_prNode, l_cursorNode ->getFirstChild() );
				else
					l_cursorNode ->appendChild( l_prNode );
			}

			l_prNode2 = XmlFunc::getChildByTagName(
				l_prNode,
				NULL,
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				"tblBorders" );

			if ( !l_prNode2 )
			{
				l_prNode2 = l_doc ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "tblBorders" ) );
				l_prNode2 ->setPrefix( Utf8ToXMLCh( m_wordMlPrefix ) );

				l_node = XmlFunc::getChildByTagName(
					l_prNode,
					NULL,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"shd,tblLayout,tblCellMar,tblLook" );

				if( l_node )
					l_prNode ->insertBefore( l_prNode2, l_node );
				else
					l_prNode ->appendChild( l_prNode2 );
			}



			l_prNode2 = XmlFunc::getChildByTagName(
				l_cursorNode,
				l_prNode,
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				"trPr" );

			if ( l_prNode2 )
			{
				if( l_prNode ->getNextSibling() != l_prNode2 )
				{
					l_cursorNode ->removeChild( l_prNode2 );

					if( l_prNode ->getNextSibling() )
						l_cursorNode ->insertBefore( l_prNode2, l_prNode ->getNextSibling() );
					else
						l_cursorNode ->appendChild( l_prNode2 );
				}
			}

			else
			{
				l_prNode2 = l_doc ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "trPr" ) );
				l_prNode2 ->setPrefix( Utf8ToXMLCh( m_wordMlPrefix ) );

				if( l_prNode ->getNextSibling() )
					l_cursorNode ->insertBefore( l_prNode2, l_prNode ->getNextSibling() );
				else
					l_cursorNode ->appendChild( l_prNode2 );
			}

			XmlFunc::removeAttrByNamespace(
				l_cursorNode,
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				"rsidP,rsidR,rsidTr,rsidRPr,rsidDel,rsidRDefault,rsidSect" );
		}

		else
		if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "tc" ) ) )
		{
			l_prNode = XmlFunc::getChildByTagName(
				l_cursorNode,
				NULL,
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				"tcPr" );

			if ( l_prNode )
			{
				if( l_cursorNode ->getFirstChild() != l_prNode )
				{
					l_cursorNode ->removeChild( l_prNode );

					if( l_cursorNode ->getFirstChild() )
						l_cursorNode ->insertBefore( l_prNode, l_cursorNode ->getFirstChild() );
					else
						l_cursorNode ->appendChild( l_prNode );
				}
			}

			else
			{
				l_prNode = l_doc ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), Utf8ToXMLCh( l_str ) );
				l_prNode ->setPrefix( Utf8ToXMLCh( m_wordMlPrefix ) );

				if( l_cursorNode ->getFirstChild() )
					l_cursorNode ->insertBefore( l_prNode, l_cursorNode ->getFirstChild() );
				else
					l_cursorNode ->appendChild( l_prNode );
			}

			l_prNode2 = XmlFunc::getChildByTagName(
				l_prNode,
				NULL,
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				"tcBorders" );

			if ( !l_prNode2 )
			{
				l_prNode2 = l_doc ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "tcBorders" ) );
				l_prNode2 ->setPrefix( Utf8ToXMLCh( m_wordMlPrefix ) );

				l_node = XmlFunc::getChildByTagName(
					l_prNode,
					NULL,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"shd,tblLayout,tblCellMar,tblLook" );

				if( l_node )
					l_prNode ->insertBefore( l_prNode2, l_node );
				else
					l_prNode ->appendChild( l_prNode2 );
			}

			XmlFunc::removeAttrByNamespace(
				l_cursorNode,
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				"rsidP,rsidR,rsidTr,rsidRPr,rsidDel,rsidRDefault,rsidSect" );
		}

		else
		if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "p" ) ) 
		  || XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "r" ) ) )
		{
			l_str = XmlFunc::XMLChToUtf8( l_cursorNode ->getLocalName() ) + "Pr";

			l_prNode = XmlFunc::getChildByTagName(
				l_cursorNode,
				NULL,
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				l_str );

			if ( l_prNode )
			{
				if( l_cursorNode ->getFirstChild() != l_prNode )
				{
					l_cursorNode ->removeChild( l_prNode );

					if( l_cursorNode ->getFirstChild() )
						l_cursorNode ->insertBefore( l_prNode, l_cursorNode ->getFirstChild() );
					else
						l_cursorNode ->appendChild( l_prNode );
				}
			}

			else
			{
				l_prNode = l_doc ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), Utf8ToXMLCh( l_str ) );
				l_prNode ->setPrefix( Utf8ToXMLCh( m_wordMlPrefix ) );

				if( l_cursorNode ->getFirstChild() )
					l_cursorNode ->insertBefore( l_prNode, l_cursorNode ->getFirstChild() );
				else
					l_cursorNode ->appendChild( l_prNode );
			}

			// remove empty r's with only rPr (whether the rPr was created by us or already existed
			if( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "r" ) )
			 && l_cursorNode ->getChildNodes() ->getLength() == 1 )
				l_cursorDriller ->deleteNode();

			else
				XmlFunc::removeAttrByNamespace(
					l_cursorNode,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"rsidP,rsidR,rsidTr,rsidRPr,rsidDel,rsidRDefault,rsidSect" );
		}

		else
		if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "t" ) )
		  || XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "instrText" ) ) )
		{
			l_node = l_cursorNode ->getFirstChild();
			if ( l_node && l_node ->getNodeType() == xercesc::DOMNode::TEXT_NODE )
			{
				l_textNode = ( xercesc::DOMText* ) l_node;

				while ( l_node = l_textNode ->getNextSibling() )
				{
					l_cursorNode ->removeChild( l_node );
				}

				if ( !XmlFunc::XMLChCmp( l_cursorNode ->getAttribute( _X( "xml:space" ) ), _X( "preserve" ) ) )
				{
					l_str = XmlFunc::XMLChToUtf8( l_textNode ->getNodeValue() );
					l_str = StrFunc::removeDuplicate( l_str, "\n\r\t " );

					l_textNode ->setNodeValue( Utf8ToXMLCh( l_str ) );
					l_cursorNode ->setAttribute( _X( "xml:space" ),  _X( "preserve" ) );
				}
			}

			else
			{
				while ( l_node = l_cursorNode ->getFirstChild() )
				{
					l_cursorNode ->removeChild( l_node );
				}

				l_textNode = l_doc ->createTextNode( _X( "" ) );

				l_cursorNode ->appendChild( l_textNode );
				l_cursorNode ->setAttribute( _X( "xml:space" ), _X( "preserve" ) );
			}
		}

		else
		if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "br" ) ) )
		{
			if ( XmlFunc::XMLChCmp( l_cursorNode ->getAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":type" ) ), _X( "page" ) ) )
			{
				l_rNode			= ( xercesc::DOMElement* ) l_cursorNode ->getParentNode();
				l_pNode			= ( xercesc::DOMElement* ) l_rNode ->getParentNode();
				l_parentNode	= l_pNode ->getParentNode();

				if ( l_parentNode == m_bodyNode )
				{
					if ( l_rNode ->getChildNodes() ->getLength() == 2 )
					{
						l_rNode ->removeChild( l_cursorNode );
						l_pNode ->insertBefore( l_cursorNode, l_rNode );
						l_pNode ->removeChild( l_rNode );
					}

					else
					if ( l_cursorNode == l_rNode ->getChildNodes() ->item(1) )
					{
						l_rNode ->removeChild( l_cursorNode );
						l_pNode ->insertBefore( l_cursorNode, l_rNode );
					}

					else
					if ( l_cursorNode == l_rNode ->getLastChild() )
					{
						l_rNode ->removeChild( l_cursorNode );

						if ( l_rNode == l_pNode ->getLastChild() )
							 l_pNode ->appendChild( l_cursorNode );

						else
						{
							l_node = l_rNode ->getNextSibling();
							l_pNode ->insertBefore( l_cursorNode, l_node );
						}
					}

					else
					{
						l_prNode = ( xercesc::DOMElement* ) l_rNode ->getFirstChild();

						l_rNode2 = ( xercesc::DOMElement* ) l_rNode ->cloneNode( false );
						l_pNode ->insertBefore( l_rNode2, l_rNode );

						l_prNode2 = ( xercesc::DOMElement* ) l_prNode ->cloneNode( true );
						l_rNode2 ->appendChild( l_prNode2 );

						while ( l_cursorNode != l_rNode ->getChildNodes() ->item(1) )
						{
							l_node = l_cursorNode ->getPreviousSibling();
							l_rNode ->removeChild( l_node );

							if( l_rNode2 ->getChildNodes() ->getLength() == 1 )
								l_rNode2 ->appendChild( l_node );
							else
								l_rNode2 ->insertBefore( l_node, l_rNode2 ->getChildNodes() ->item(1) );
						}

						l_rNode ->removeChild( l_cursorNode );
						l_pNode ->insertBefore( l_cursorNode, l_rNode );
					}



					l_ok = false;

					if ( l_pNode ->getChildNodes() ->getLength() > 2 )
					{
						l_childNode = l_pNode ->getChildNodes() ->item(1);

						while ( true )
						{
							if ( l_childNode != l_cursorNode
							&& ( l_childNode ->getNodeType() != xercesc::DOMNode::ELEMENT_NODE
							  || !XmlFunc::XMLChCmp( l_childNode ->getLocalName(),		_X( "bookmarkStart" ) )
							  && !XmlFunc::XMLChCmp( l_childNode ->getLocalName(),		_X( "bookmarkEnd" ) )
							  || !XmlFunc::XMLChCmp( l_childNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) ) )
								break;

							l_childNode = l_childNode ->getNextSibling();
							if ( !l_childNode )
							{
								l_ok = true;
								break;
							}
						}
					}

					if ( l_ok )
					{
						while ( l_pNode ->getChildNodes() ->getLength() > 1 )
						{
							l_childNode = l_pNode ->getChildNodes() ->item(1);
							l_pNode ->removeChild( l_childNode );
							l_parentNode ->insertBefore( l_childNode, l_pNode );
						}
					}

					else
					if ( l_pNode ->getChildNodes() ->getLength() == 2 )
					{
						l_pNode ->removeChild( l_cursorNode );
						l_parentNode ->insertBefore( l_cursorNode, l_pNode );
						l_parentNode ->removeChild( l_pNode );
					}

					else
					if ( l_cursorNode == l_pNode ->getChildNodes() ->item(1) )
					{
						l_pNode ->removeChild( l_cursorNode );
						l_parentNode ->insertBefore( l_cursorNode, l_pNode );
					}

					else
					if ( l_cursorNode == l_pNode ->getLastChild() )
					{
						l_pNode ->removeChild( l_cursorNode );

						if ( l_pNode == l_parentNode ->getLastChild() )
							 l_parentNode ->appendChild( l_cursorNode );

						else
						{
							l_node = l_pNode ->getNextSibling();
							l_parentNode ->insertBefore( l_cursorNode, l_node );
						}
					}

					else
					{
						l_prNode = ( xercesc::DOMElement* ) l_pNode ->getFirstChild();

						l_pNode2 = ( xercesc::DOMElement* ) l_pNode ->cloneNode( false );
						l_parentNode ->insertBefore( l_pNode2, l_pNode );

						l_prNode2 = ( xercesc::DOMElement* ) l_prNode ->cloneNode( true );
						l_pNode2 ->appendChild( l_prNode2 );

						while ( l_cursorNode != l_pNode ->getChildNodes() ->item(1) )
						{
							l_node = l_cursorNode ->getPreviousSibling();
							l_pNode ->removeChild( l_node );

							if( l_pNode2 ->getChildNodes() ->getLength() == 1 )
								l_pNode2 ->appendChild( l_node );
							else
								l_pNode2 ->insertBefore( l_node, l_pNode2 ->getChildNodes() ->item(1) );
						}

						l_pNode ->removeChild( l_cursorNode );
						l_parentNode ->insertBefore( l_cursorNode, l_pNode );
					}

					l_cursorNode ->setAttribute( _X( "sectId" ), Utf8ToXMLCh( StrFunc::intToStr( m_sectLen ) ) );
				}
			}
		}

		else
		if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "sectPr" ) ) )
		{
			XmlFunc::removeAttrByNamespace(
				l_cursorNode,               
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main", 
				"rsidP,rsidR,rsidTr,rsidRPr,rsidDel,rsidRDefault,rsidSect" );

			if ( l_cursorNode ->getParentNode() != m_bodyNode )
			{
				l_prNode		= ( xercesc::DOMElement* ) l_cursorNode ->getParentNode();
				l_pNode			= ( xercesc::DOMElement* ) l_prNode ->getParentNode();
				l_parentNode	= l_pNode ->getParentNode();

				if ( l_parentNode == m_bodyNode )
				{
					l_prNode ->removeChild( l_cursorNode );

					if( l_parentNode ->getLastChild() == l_pNode )
						l_parentNode ->appendChild( l_cursorNode );
					else
						l_parentNode ->insertBefore( l_cursorNode, l_pNode ->getNextSibling() );

					l_ok = false;

					if ( l_pNode ->getChildNodes() ->getLength() > 1 )
					{
						l_childNode = l_prNode ->getNextSibling();

						while ( true )
						{
							if ( l_childNode ->getNodeType() != xercesc::DOMNode::ELEMENT_NODE
							  || !XmlFunc::XMLChCmp( l_childNode ->getLocalName(),		_X( "bookmarkStart" ) )
							  && !XmlFunc::XMLChCmp( l_childNode ->getLocalName(),		_X( "bookmarkEnd" ) )
							  || !XmlFunc::XMLChCmp( l_childNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

								break;

							l_childNode = l_childNode ->getNextSibling();
							if ( !l_childNode )
							{
								l_ok = true;
								break;
							}
						}
					}

					if ( l_ok )
					{
						while ( l_pNode ->getChildNodes() ->getLength() > 1 )
						{
							l_childNode = l_pNode ->getChildNodes() ->item(1);
							l_pNode ->removeChild( l_childNode );
							l_parentNode ->insertBefore( l_childNode, l_pNode );
						}
					}

					if( l_pNode ->getChildNodes() ->getLength() == 1 )
						l_parentNode ->removeChild( l_pNode );

					l_pNode = NULL;

					if ( l_cursorNode != l_parentNode ->getFirstChild() )
					{
						l_node = l_cursorNode ->getPreviousSibling();

						if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
						  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "br" ) )
						  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) )
						  && XmlFunc::XMLChCmp( ( ( xercesc::DOMElement* ) l_node ) ->getAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":type" ) ), _X( "page" ) ) )

							  l_pNode = ( xercesc::DOMElement* ) l_node;
					}

					if ( l_pNode )
					{
						l_parentNode ->removeChild( l_pNode );

						/***
						l_pNode = l_doc ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "br" ) ) ;
						l_pNode ->setPrefix( Utf8ToXMLCh( m_wordMlPrefix ) );
						l_pNode ->setAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":type" ), _X( "page" ) );

						l_parentNode ->insertBefore( l_pNode, l_cursorNode );
						***/
					}

					// l_pNode ->setAttribute( _X( "sectId" ), Utf8ToXMLCh( StrFunc::intToStr( m_sectLen + 1 ) ) );
				}
			}

			l_cursorNode ->setAttribute( _X( "sectId" ), Utf8ToXMLCh( StrFunc::intToStr( m_sectLen ) ) );
			m_sectLen++;
		}

		/***
		// all id's are now generated
		// for all pictures even if they are generated or not
		// so there is no chance they will run into each other.

		else
		if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "drawing" ) ) )
		{
			l_node = l_cursorNode;

			l_treeDriller.reset( new XmlTreeDriller(
				l_node,
				"http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing", "inline",
				"http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing", "docPr",
				"", "", "" ) );

			if ( l_node = l_treeDriller ->nextNode() )
			{	
				l_str = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_node ) ->getAttribute( _X( "id" ) ) );
				if ( StrFunc::isNum( l_str ) )
				{				
					l_idSeq = StrFunc::strToInt<size_t>( l_str );

					if( l_idSeq > m_idSeq )
						m_idSeq = l_idSeq;
				}
			}
		}
		*/

		else
		if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "sdt" ) ) )
		{
			l_ok = false;

			l_node = XmlFunc::getChildByTagName(
				l_cursorNode,
				NULL,
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				"sdtPr" );

			if ( l_node )
			{
				l_node = XmlFunc::getChildByTagName(
					l_node,
					NULL,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"docPartObj" );

				if ( l_node )
				{
					l_node = XmlFunc::getChildByTagName(
						l_node,
						NULL,
						"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
						"docPartGallery" );

					if ( l_node )
					{
						if ( StrFunc::lc( XmlFunc::XMLChToUtf8(
								( ( xercesc::DOMElement* ) l_node ) ->getAttributeNS(
									_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ),
									_X( "val" ) ) ) ) == "table of contents" )

							l_ok = true;
					}
				}
			}

			if ( l_ok )
			{
				l_treeDriller.reset( new XmlTreeDriller(
					l_cursorNode,
					"", "",
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main", "fldChar",
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main", "fldCharType", "separate,end" ) );

				l_fldChar	= NULL;
				l_fldChar2	= NULL;

				while ( l_node = l_treeDriller ->nextNode() )
				{
					if ( XmlFunc::XMLChCmp(
						( ( xercesc::DOMElement* ) l_node ) ->getAttributeNS(
							_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ),
							_X( "fldCharType" ) ), _X( "separate" ) ) )
					{
						if(!l_fldChar )
							l_fldChar = ( xercesc::DOMElement* ) l_node;
					}
		
					else
					{
						l_fldChar2 = ( xercesc::DOMElement* ) l_node;
					}
				}

				l_rNode			= ( xercesc::DOMElement* ) l_fldChar ->getParentNode();
				l_pNode			= ( xercesc::DOMElement* ) l_rNode ->getParentNode();
				l_rNode2		= ( xercesc::DOMElement* ) l_fldChar2 ->getParentNode();
				l_pNode2		= ( xercesc::DOMElement* ) l_rNode2 ->getParentNode();
				l_parentNode	= ( xercesc::DOMElement* ) l_pNode2 ->getParentNode();

				if ( l_rNode == l_rNode2 )
				{
					while ( l_node = l_fldChar ->getNextSibling() )
					{
						if ( l_node == l_fldChar2 )
							break;

						l_rNode ->removeChild( l_node );
					}
				}

				else
				{
					while ( l_node = l_fldChar ->getNextSibling() )
					{
						l_rNode ->removeChild( l_node );
					}

					while ( l_node = l_fldChar2 ->getPreviousSibling() )
					{
						l_rNode2 ->removeChild( l_node );
					}

					if ( l_pNode == l_pNode2 )
					{
						while ( l_node = l_rNode ->getNextSibling() )
						{
							if ( l_node == l_rNode2 )
								break;

							l_pNode ->removeChild( l_node );
						}
					}

					else
					{
						while ( l_node = l_rNode ->getNextSibling() )
						{
							l_pNode ->removeChild( l_node );
						}

						while ( l_node = l_rNode2 ->getPreviousSibling() )
						{
							l_pNode2 ->removeChild( l_node );
						}

						while ( l_node = l_pNode ->getNextSibling() )
						{
							if ( l_node == l_pNode2 )
								break;

							l_parentNode ->removeChild( l_node );
						}
					}
				}

				m_file ->setToc( true );
			}

			/*** removed watermark parts ***
			else
			{
				l_node = XmlFunc::getChildByTagName(
					l_cursorNode,
					NULL,
					"sdtContent",
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main" );

				if ( l_node )
				{
					l_parentNode = l_cursorNode ->getParentNode();

					while ( l_childNode = l_node ->getFirstChild() )
					{
						l_node ->removeChild( l_childNode );
						l_parentNode ->insertBefore( l_childNode, l_cursorNode );
					}
				}

				l_cursorDriller ->deleteNode();
			}
			***/
		}

		else
		if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "ins" ) )
		  || XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "moveTo" ) ) )
		{
			l_parentNode = l_cursorNode ->getParentNode();
				
			while ( l_childNode = l_cursorNode ->getFirstChild() )
			{
				l_cursorNode ->removeChild( l_childNode );
				l_parentNode ->insertBefore( l_childNode, l_cursorNode );
			}

			l_cursorDriller ->deleteNode();
		}

		else
			l_cursorDriller ->deleteNode();
	}

	// also removes bookmarks beginning with "_" that are in the middle of runs 
	// which might be joined into a single run if removed.
	normalizeBookmarksStartEnd();
	normalizeTableBorders();
	normalizeRuns();
} // normalize

void DocxCompilerItemFile::normalizeRuns()
{
	boost::scoped_ptr<XmlTreeDriller>	l_cursorDriller;
	xercesc::DOMElement*				l_cursorNode;

	xercesc::DOMElement*				l_rNode;
	xercesc::DOMElement*				l_rNode2;
	xercesc::DOMElement*				l_rPrNode;
	xercesc::DOMElement*				l_rPrNode2;
	xercesc::DOMElement*				l_tNode;
	xercesc::DOMElement*				l_tNode2;
	xercesc::DOMText*					l_textNode;
	xercesc::DOMText*					l_textNode2;

	xercesc::DOMNode*					l_node;
	string								l_str;

	l_cursorDriller.reset( new XmlTreeDriller(
		m_bodyNode,
		"", "",
		"http://schemas.openxmlformats.org/wordprocessingml/2006/main", "p,instrText,fldChar",
		"", "", "" ) );

	while ( l_cursorNode = ( xercesc::DOMElement* ) l_cursorDriller ->nextNode() )
	{
		if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "p" ) ) )
		{
			l_rNode = NULL;

			while ( l_rNode = XmlFunc::getChildByTagName(

				l_cursorNode,
				l_rNode,
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				"r" ) )
			{
				l_rPrNode = ( xercesc::DOMElement* ) l_rNode ->getFirstChild();

				if ( l_tNode = XmlFunc::getChildByTagName(

					l_rNode,
					NULL,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"t" ) )
				{
					l_textNode = ( xercesc::DOMText* ) l_tNode ->getFirstChild();

					while ( true ) // <r> loop
					{
						while ( true ) // <t> loop
						{
							l_node = l_tNode ->getNextSibling();

							if ( !l_node )
								break;

							if ( l_node ->getNodeType() != xercesc::DOMNode::ELEMENT_NODE )
								break; // not an element, leave <t> loop

							l_tNode2 = ( xercesc::DOMElement* ) l_node;

							if ( !XmlFunc::XMLChCmp( l_tNode2 ->getLocalName(),		_X( "t" ) )
							  || !XmlFunc::XMLChCmp( l_tNode2 ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
								break; // not a <t> element, leave <t> loop

							l_textNode2	= ( xercesc::DOMText* ) l_tNode2 ->getFirstChild();

							l_str  = XmlFunc::XMLChToUtf8( l_textNode ->getNodeValue() );
							l_str += XmlFunc::XMLChToUtf8( l_textNode2 ->getNodeValue() );

							l_textNode ->setNodeValue( Utf8ToXMLCh( l_str ) );

							l_rNode ->removeChild( l_tNode2 );
						}

						l_node = l_rNode ->getNextSibling();

						if ( !l_node )
							break; // reached last child, leave <r> loop

						if ( l_node ->getNodeType() != xercesc::DOMNode::ELEMENT_NODE )
							break; // not an element, leave <r> loop

						l_rNode2 = ( xercesc::DOMElement* ) l_node;

						if ( !XmlFunc::XMLChCmp( l_rNode2 ->getLocalName(),		_X( "r" ) )
						  || !XmlFunc::XMLChCmp( l_rNode2 ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
							break; // not a <r> element, leave <r> loop

						l_rPrNode2 = ( xercesc::DOMElement* ) l_rNode2 ->getFirstChild();

						if ( !l_rPrNode2 ->isEqualNode( l_rPrNode ) )
							break;	// different properties for l_rNode and l_rNode2, leave <r> loop.
									// isEqual() returns true only if children are in the same order.
									// for now it's enough.

						l_node = l_rPrNode2 ->getNextSibling();

						if ( l_node ->getNodeType() != xercesc::DOMNode::ELEMENT_NODE )
							break; // not an element, leave <r> loop

						l_tNode2 = ( xercesc::DOMElement* ) l_node;

						if ( !XmlFunc::XMLChCmp( l_tNode2 ->getLocalName(),		_X( "t" ) )
						  || !XmlFunc::XMLChCmp( l_tNode2 ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
							break; // not a <t> element, leave <t> loop

						do
						{
							l_rNode2 ->removeChild( l_node );
							l_rNode ->appendChild( l_node );
						} while ( l_node = l_node ->getNextSibling() );
							
						l_cursorNode ->removeChild( l_rNode2 );
					}

					if ( XmlFunc::getChildByTagName(

						l_rNode,
						l_tNode,
						"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
						"t" ) ) // we still have more <t>'s in our <r>
					{
						l_rNode2 = ( xercesc::DOMElement* ) l_rNode ->cloneNode( false );

						if( l_rNode == l_cursorNode ->getLastChild() )
							l_cursorNode ->appendChild( l_rNode2 );

						else
						{
							l_node = l_rNode ->getNextSibling();
							l_cursorNode ->insertBefore( l_rNode2, l_node );
						}

						l_rPrNode2 = ( xercesc::DOMElement* ) l_rPrNode ->cloneNode( true );
						l_rNode2 ->appendChild( l_rPrNode2 );

						while ( l_node = l_tNode ->getNextSibling() )
						{
							l_rNode ->removeChild( l_node );
							l_rNode2 ->appendChild( l_node );
						}
					}
				}
			}
		}

		else
		if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "instrText" ) ) )
		{
			l_textNode = ( xercesc::DOMText* ) l_cursorNode ->getFirstChild();

			while ( l_cursorNode = ( xercesc::DOMElement* ) l_cursorDriller ->nextNode() )
			{
				if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "instrText" ) ) )
				{
					l_textNode2 = ( xercesc::DOMText* ) l_cursorNode ->getFirstChild();

					if ( !XmlFunc::XMLChToUtf8( l_textNode2 ->getNodeValue() ).empty() )
					{
						l_str  = XmlFunc::XMLChToUtf8( l_textNode ->getNodeValue() );
						l_str += XmlFunc::XMLChToUtf8( l_textNode2 ->getNodeValue() );
						l_textNode ->setNodeValue( Utf8ToXMLCh( l_str ) );
					}

					l_cursorDriller ->deleteNode();
				}
				
				else
				if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "fldChar" ) ) )
					break;
			}
		}
	}
} // normalizeRuns

void DocxCompilerItemFile::normalizeTableBorders()
{
	map<string, DocxCompilerTableStyle*>::const_iterator	l_tableStyleIterator;
	const map<string, DocxCompilerTableStyle*>*				l_tableStyles = m_file ->getStyleFile() ->getTableStyles();
	DocxCompilerTableStyle*									l_tableStyle;

	boost::scoped_ptr<XmlTreeDriller>	l_tblDriller;
	xercesc::DOMElement*				l_tblNode;
	xercesc::DOMElement*				l_tblPrNode;
	xercesc::DOMElement*				l_tblStyleNode;
	xercesc::DOMElement*				l_tblBordersNode;
	xercesc::DOMElement*				l_tblTopNode;
	xercesc::DOMElement*				l_tblLeftNode;
	xercesc::DOMElement*				l_tblBottomNode;
	xercesc::DOMElement*				l_tblRightNode;
	xercesc::DOMElement*				l_tblInsideHNode;
	xercesc::DOMElement*				l_tblInsideVNode;
	bool								l_conditionalStyle;

	xercesc::DOMElement*				l_trNode;
	xercesc::DOMElement*				l_trNodePrev;
	xercesc::DOMElement*				l_trNodeNext;
	xercesc::DOMElement*				l_trPrNode;
	xercesc::DOMElement*				l_trBordersNode;
	xercesc::DOMElement*				l_trTopNode;
	xercesc::DOMElement*				l_trLeftNode;
	xercesc::DOMElement*				l_trBottomNode;
	xercesc::DOMElement*				l_trRightNode;
	xercesc::DOMElement*				l_trInsideHNode;
	xercesc::DOMElement*				l_trInsideVNode;

	xercesc::DOMElement*				l_tcNode;
	xercesc::DOMElement*				l_tcNodePrev;
	xercesc::DOMElement*				l_tcNodeNext;
	xercesc::DOMElement*				l_tcPrNode;
	xercesc::DOMElement*				l_tcBordersNode;
	xercesc::DOMElement*				l_tcTopNode;
	xercesc::DOMElement*				l_tcLeftNode;
	xercesc::DOMElement*				l_tcBottomNode;
	xercesc::DOMElement*				l_tcRightNode;
	bool								l_merge;

	xercesc::DOMDocument*				l_doc = m_part ->getDoc();
	xercesc::DOMNode*					l_node;
	string								l_str;

	l_tblDriller.reset( new XmlTreeDriller(
		m_bodyNode,
		"", "",
		"http://schemas.openxmlformats.org/wordprocessingml/2006/main", "tbl",
		"", "", "" ) );

	while ( l_tblNode = ( xercesc::DOMElement* ) l_tblDriller ->nextNode() )
	{
		l_tblPrNode			= ( xercesc::DOMElement* ) l_tblNode ->getFirstChild();
		l_tblTopNode		= NULL;
		l_tblLeftNode		= NULL;
		l_tblBottomNode		= NULL;
		l_tblRightNode		= NULL;
		l_tblInsideHNode	= NULL;
		l_tblInsideVNode	= NULL;
		l_conditionalStyle	= false;

		l_tblStyleNode = XmlFunc::getChildByTagName(
			l_tblPrNode,
			NULL,
			"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
			"tblStyle" );

		if ( l_tblStyleNode )
		{
			l_str = StrFunc::lc( XmlFunc::XMLChToUtf8( l_tblStyleNode ->getAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":val" ) ) ) );

			l_tableStyleIterator = l_tableStyles ->find( l_str );
			if( l_tableStyleIterator != l_tableStyles ->end() )
			{
				l_tableStyle = l_tableStyleIterator ->second;

				l_tblTopNode		= l_tableStyle ->getTopNode();
				l_tblLeftNode		= l_tableStyle ->getLeftNode();
				l_tblBottomNode		= l_tableStyle ->getBottomNode();
				l_tblRightNode		= l_tableStyle ->getRightNode();
				l_tblInsideHNode	= l_tableStyle ->getInsideHNode();
				l_tblInsideVNode	= l_tableStyle ->getInsideVNode();
				l_conditionalStyle	= l_tableStyle ->isConditionalStyle();
			}
		}

		if ( l_conditionalStyle )
			continue;



		l_tblBordersNode = XmlFunc::getChildByTagName(
			l_tblPrNode,
			NULL,
			"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
			"tblBorders" );

		l_node = l_tblBordersNode ->getFirstChild();

		while ( l_node )
		{
			if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
			  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "top" ) )
			  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

				l_tblTopNode = ( xercesc::DOMElement* ) l_node;

			else
			if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
			  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "left" ) )
			  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

				l_tblLeftNode = ( xercesc::DOMElement* ) l_node;

			else
			if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
			  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "bottom" ) )
			  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

				l_tblBottomNode = ( xercesc::DOMElement* ) l_node;

			else
			if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
			  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "right" ) )
			  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

				l_tblRightNode = ( xercesc::DOMElement* ) l_node;

			else
			if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
			  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "insideH" ) )
			  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

				l_tblInsideHNode = ( xercesc::DOMElement* ) l_node;

			else
			if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
			  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "insideV" ) )
			  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

				l_tblInsideVNode = ( xercesc::DOMElement* ) l_node;

			l_node = l_node ->getNextSibling();
		}



		l_trNodePrev	= NULL;
		l_trNode		= XmlFunc::getChildByTagName(
			l_tblNode,
			l_tblPrNode ->getNextSibling(),
			"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
			"tr" );

		while ( l_trNode )
		{
			l_trNodeNext = XmlFunc::getChildByTagName(
				l_tblNode,
				l_trNode,
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				"tr" );

			l_trPrNode		= ( xercesc::DOMElement* ) l_trNode ->getFirstChild();
			l_trTopNode		= NULL;
			l_trLeftNode	= NULL;
			l_trBottomNode	= NULL;
			l_trRightNode	= NULL;
			l_trInsideHNode	= NULL;
			l_trInsideVNode	= NULL;

			l_trBordersNode = XmlFunc::getChildByTagName(
				l_trPrNode,
				NULL,
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				"tblBorders" );

			l_node = l_trBordersNode ->getFirstChild();

			while ( l_node )
			{
				if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
				  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "top" ) )
				  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

					l_trTopNode = ( xercesc::DOMElement* ) l_node;

				else
				if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
				  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "left" ) )
				  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

					l_trLeftNode = ( xercesc::DOMElement* ) l_node;

				else
				if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
				  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "bottom" ) )
				  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

					l_trBottomNode = ( xercesc::DOMElement* ) l_node;

				else
				if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
				  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "right" ) )
				  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

					l_trRightNode = ( xercesc::DOMElement* ) l_node;

				else
				if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
				  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "insideH" ) )
				  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

					l_trInsideHNode = ( xercesc::DOMElement* ) l_node;

				else
				if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
				  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "insideV" ) )
				  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

					l_trInsideVNode = ( xercesc::DOMElement* ) l_node;

				l_node = l_node ->getNextSibling();
			}

			if( !l_trTopNode )
			{
				l_trTopNode = l_doc ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "top" ) );
				l_trTopNode ->setPrefix( Utf8ToXMLCh( m_wordMlPrefix ) );

				l_node = ( !l_trNodePrev ? l_tblTopNode : l_tblInsideHNode );
				if ( l_node )
					XmlFunc::importAttributes( ( xercesc::DOMElement* ) l_node, l_trTopNode );
				else
					l_trTopNode ->setAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":val" ), _X( "nil" ) );

				l_node = XmlFunc::getChildByTagName(
					l_trBordersNode,
					NULL,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"left,bottom,right,insideH,insideV,tl2br,tr2bl" );

				if( l_node )
					l_trBordersNode ->insertBefore( l_trTopNode, l_node );
				else
					l_trBordersNode ->appendChild( l_trTopNode );
			}

			if( !l_trLeftNode )
			{
				l_trLeftNode = l_doc ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "left" ) );
				l_trLeftNode ->setPrefix( Utf8ToXMLCh( m_wordMlPrefix ) );

				l_node = l_tblLeftNode;
				if ( l_node )
					XmlFunc::importAttributes( ( xercesc::DOMElement* ) l_node, l_trLeftNode );
				else
					l_trLeftNode ->setAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":val" ), _X( "nil" ) );

				l_node = XmlFunc::getChildByTagName(
					l_trBordersNode,
					NULL,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"bottom,right,insideH,insideV,tl2br,tr2bl" );

				if( l_node )
					l_trBordersNode ->insertBefore( l_trLeftNode, l_node );
				else
					l_trBordersNode ->appendChild( l_trLeftNode );
			}

			if( !l_trBottomNode )
			{
				l_trBottomNode = l_doc ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "bottom" ) );
				l_trBottomNode ->setPrefix( Utf8ToXMLCh( m_wordMlPrefix ) );

				l_node = ( !l_trNodeNext ? l_tblBottomNode : l_tblInsideHNode );
				if ( l_node )
					XmlFunc::importAttributes( ( xercesc::DOMElement* ) l_node, l_trBottomNode );
				else
					l_trBottomNode ->setAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":val" ), _X( "nil" ) );

				l_node = XmlFunc::getChildByTagName(
					l_trBordersNode,
					NULL,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"right,insideH,insideV,tl2br,tr2bl" );

				if( l_node )
					l_trBordersNode ->insertBefore( l_trBottomNode, l_node );
				else
					l_trBordersNode ->appendChild( l_trBottomNode );
			}

			if( !l_trRightNode )
			{
				l_trRightNode = l_doc ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "right" ) );
				l_trRightNode ->setPrefix( Utf8ToXMLCh( m_wordMlPrefix ) );

				l_node = l_tblRightNode;
				if ( l_node )
					XmlFunc::importAttributes( ( xercesc::DOMElement* ) l_node, l_trRightNode );
				else
					l_trRightNode ->setAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":val" ), _X( "nil" ) );

				l_node = XmlFunc::getChildByTagName(
					l_trBordersNode,
					NULL,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"insideH,insideV,tl2br,tr2bl" );

				if( l_node )
					l_trBordersNode ->insertBefore( l_trRightNode, l_node );
				else
					l_trBordersNode ->appendChild( l_trRightNode );
			}

			if( !l_trInsideHNode )
			{
				l_trInsideHNode = l_doc ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "insideH" ) );
				l_trInsideHNode ->setPrefix( Utf8ToXMLCh( m_wordMlPrefix ) );

				l_node = l_tblInsideHNode;
				if ( l_node )
					XmlFunc::importAttributes( ( xercesc::DOMElement* ) l_node, l_trInsideHNode );
				else
					l_trInsideHNode ->setAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":val" ), _X( "nil" ) );

				l_node = XmlFunc::getChildByTagName(
					l_trBordersNode,
					NULL,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"insideV,tl2br,tr2bl" );

				if( l_node )
					l_trBordersNode ->insertBefore( l_trInsideHNode, l_node );
				else
					l_trBordersNode ->appendChild( l_trInsideHNode );
			}

			if( !l_trInsideVNode )
			{
				l_trInsideVNode = l_doc ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "insideV" ) );
				l_trInsideVNode ->setPrefix( Utf8ToXMLCh( m_wordMlPrefix ) );

				l_node = l_tblInsideVNode;
				if ( l_node )
					XmlFunc::importAttributes( ( xercesc::DOMElement* ) l_node, l_trInsideVNode );
				else
					l_trInsideVNode ->setAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":val" ), _X( "nil" ) );

				l_node = XmlFunc::getChildByTagName(
					l_trBordersNode,
					NULL,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"tl2br,tr2bl" );

				if( l_node )
					l_trBordersNode ->insertBefore( l_trInsideVNode, l_node );
				else
					l_trBordersNode ->appendChild( l_trInsideVNode );
			}



			l_tcNodePrev	= NULL;
			l_tcNode		= XmlFunc::getChildByTagName(
				l_trNode,
				l_trNode ->getFirstChild(),
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				"tc" );

			while ( l_tcNode )
			{
				l_tcNodeNext = XmlFunc::getChildByTagName(
					l_trNode,
					l_tcNode,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"tc" );

				l_tcPrNode		= ( xercesc::DOMElement* ) l_tcNode ->getFirstChild();
				l_tcTopNode		= NULL;
				l_tcLeftNode	= NULL;
				l_tcBottomNode	= NULL;
				l_tcRightNode	= NULL;
				l_merge			= false;

				while ( l_node = XmlFunc::getChildByTagName(
					l_tcPrNode,
					l_node,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"vMerge,hMerge" ) )
				{
					l_str = XmlFunc::XMLChToUtf8( ( ( xercesc::DOMElement* ) l_node ) ->getAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":val" ) ) );
					if ( l_str.empty() || StrFunc::lc( l_str ) == "continue" )
					{
						l_merge = true;
						break;
					}
				}

				if( l_merge )
				{
					l_tcNode = l_tcNodeNext;
					continue;
				}



				l_tcBordersNode	= XmlFunc::getChildByTagName(
					l_tcPrNode,
					NULL,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"tcBorders" );

				l_node = l_tcBordersNode ->getFirstChild();

				while ( l_node )
				{
					if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
					  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "top" ) )
					  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

						l_tcTopNode = ( xercesc::DOMElement* ) l_node;

					else
					if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
					  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "left" ) )
					  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

						l_tcLeftNode = ( xercesc::DOMElement* ) l_node;

					else
					if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
					  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "bottom" ) )
					  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

						l_tcBottomNode = ( xercesc::DOMElement* ) l_node;

					else
					if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
					  && XmlFunc::XMLChCmp( l_node ->getLocalName(),	_X( "right" ) )
					  && XmlFunc::XMLChCmp( l_node ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

						l_tcRightNode = ( xercesc::DOMElement* ) l_node;

					l_node = l_node ->getNextSibling();
				}



				if( !l_tcTopNode )
				{
					l_tcTopNode = l_doc ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "top" ) );
					l_tcTopNode ->setPrefix( Utf8ToXMLCh( m_wordMlPrefix ) );

					l_node = l_trTopNode;
					if ( l_node )
						XmlFunc::importAttributes( ( xercesc::DOMElement* ) l_node, l_tcTopNode );
					else
						l_tcTopNode ->setAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":val" ), _X( "nil" ) );

					l_node = XmlFunc::getChildByTagName(
						l_tcBordersNode,
						NULL,
						"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
						"left,bottom,right,insideH,insideV,tl2br,tr2bl" );

					if( l_node )
						l_tcBordersNode ->insertBefore( l_tcTopNode, l_node );
					else
						l_tcBordersNode ->appendChild( l_tcTopNode );
				}

				if( !l_tcLeftNode )
				{
					l_tcLeftNode = l_doc ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "left" ) );
					l_tcLeftNode ->setPrefix( Utf8ToXMLCh( m_wordMlPrefix ) );

					l_node = ( !l_tcNodePrev ? l_trLeftNode : l_trInsideVNode );
					if ( l_node )
						XmlFunc::importAttributes( ( xercesc::DOMElement* ) l_node, l_tcLeftNode );
					else
						l_tcLeftNode ->setAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":val" ), _X( "nil" ) );

					l_node = XmlFunc::getChildByTagName(
						l_tcBordersNode,
						NULL,
						"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
						"bottom,right,insideH,insideV,tl2br,tr2bl" );

					if( l_node )
						l_tcBordersNode ->insertBefore( l_tcLeftNode, l_node );
					else
						l_tcBordersNode ->appendChild( l_tcLeftNode );
				}

				if( !l_tcBottomNode )
				{
					l_tcBottomNode = l_doc ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "bottom" ) );
					l_tcBottomNode ->setPrefix( Utf8ToXMLCh( m_wordMlPrefix ) );

					l_node = l_trBottomNode;
					if ( l_node )
						XmlFunc::importAttributes( ( xercesc::DOMElement* ) l_node, l_tcBottomNode );
					else
						l_tcBottomNode ->setAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":val" ), _X( "nil" ) );

					l_node = XmlFunc::getChildByTagName(
						l_tcBordersNode,
						NULL,
						"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
						"right,insideH,insideV,tl2br,tr2bl" );

					if( l_node )
						l_tcBordersNode ->insertBefore( l_tcBottomNode, l_node );
					else
						l_tcBordersNode ->appendChild( l_tcBottomNode );
				}

				if( !l_tcRightNode )
				{
					l_tcRightNode = l_doc ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "right" ) );
					l_tcRightNode ->setPrefix( Utf8ToXMLCh( m_wordMlPrefix ) );

					l_node = ( !l_tcNodeNext ? l_trRightNode : l_trInsideVNode );
					if ( l_node )
						XmlFunc::importAttributes( ( xercesc::DOMElement* ) l_node, l_tcRightNode );
					else
						l_tcRightNode ->setAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":val" ), _X( "nil" ) );

					l_node = XmlFunc::getChildByTagName(
						l_tcBordersNode,
						NULL,
						"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
						"insideH,insideV,tl2br,tr2bl" );

					if( l_node )
						l_tcBordersNode ->insertBefore( l_tcRightNode, l_node );
					else
						l_tcBordersNode ->appendChild( l_tcRightNode );
				}

				l_tcNodePrev	= l_tcNode;
				l_tcNode		= l_tcNodeNext;
			}

			l_trNodePrev	= l_trNode;
			l_trNode		= l_trNodeNext;
		}
	}
} // normalizeTableBorders

void DocxCompilerItemFile::normalizeBookmarksStartEnd()
{
	map<size_t, Bookmark*>::iterator	l_bookmarksByIdIterator;
	map<size_t, Bookmark*>				l_bookmarksById;

	list<Bookmark*>::iterator			l_bookmarkIterator;
	list<Bookmark*>						l_bookmarks;
	Bookmark*							l_bookmark;
	Bookmark*							l_prevBookmark;

	boost::scoped_ptr<XmlTreeDriller>	l_cursorDriller;
	xercesc::DOMElement*				l_cursorNode;

	xercesc::DOMElement*				l_bookmarkStartNode	= NULL;
	xercesc::DOMElement*				l_bookmarkEndNode	= NULL;

	xercesc::DOMElement*				l_rPrNode;
	xercesc::DOMNode*					l_childNode;
	xercesc::DOMNode*					l_lastChildNode;
	xercesc::DOMNode*					l_parentNode;
	xercesc::DOMNode*					l_node;

	string								l_bookmarkName;
	size_t								l_bookmarkId;
	size_t								l_startGroupSeq	= 0;
	size_t								l_endGroupSeq	= 0;

	try
	{
		l_cursorDriller.reset( new XmlTreeDriller(
			m_bodyNode,
			"", "",
			"http://schemas.openxmlformats.org/wordprocessingml/2006/main", "bookmarkStart,bookmarkEnd",
			"", "", "" ) );
		
		while ( l_cursorNode = ( xercesc::DOMElement* ) l_cursorDriller ->nextNode() )
		{
			if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "bookmarkStart" ) ) )
			{
				l_bookmarkId	= StrFunc::strToInt<size_t>	( XmlFunc::XMLChToUtf8( l_cursorNode ->getAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":id" ) ) ) );
				l_bookmarkName	= StrFunc::lc				( XmlFunc::XMLChToUtf8( l_cursorNode ->getAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":name" ) ) ) );

				if( l_bookmarkStartNode
				 && l_bookmarkStartNode ->getNextSibling() != l_cursorNode )
					l_startGroupSeq++;

				l_bookmark = new Bookmark();
				l_bookmark ->m_startNode		= l_cursorNode;
				l_bookmark ->m_endNode			= NULL;
				l_bookmark ->m_name				= l_bookmarkName;
				l_bookmark ->m_id				= l_bookmarkId;
				l_bookmark ->m_startGroupSeq	= l_startGroupSeq;
				l_bookmark ->m_endGroupSeq		= USHRT_MAX;

				l_bookmarks.push_back( l_bookmark );
				l_bookmarksById.insert( make_pair( l_bookmarkId, l_bookmark ) );
				l_bookmarkStartNode	= l_cursorNode;
			}

			else
			if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "bookmarkEnd" ) ) )
			{
				l_bookmarkId = StrFunc::strToInt<size_t>( XmlFunc::XMLChToUtf8( l_cursorNode ->getAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":id" ) ) ) );
				l_bookmarksByIdIterator = l_bookmarksById.find( l_bookmarkId );

				if ( l_bookmarksByIdIterator == l_bookmarksById.end() )
					throw NodeNotFoundException( "bookmarkStart", __FILE__, __LINE__ );

				if( l_bookmarkEndNode
				 && l_bookmarkEndNode ->getNextSibling() != l_cursorNode )
					l_endGroupSeq++;

				l_bookmark					= l_bookmarksByIdIterator ->second;
				l_bookmark ->m_endNode		= l_cursorNode;
				l_bookmark ->m_endGroupSeq	= l_endGroupSeq;
				l_bookmarkEndNode			= l_cursorNode;
			}
		}



		l_bookmarkIterator = l_bookmarks.begin();

		while ( l_bookmarkIterator != l_bookmarks.end() )
		{
			l_bookmark = *l_bookmarkIterator;

			if ( StrFunc::begins( l_bookmark ->m_name, "_" )
			  || l_bookmark ->m_endNode == l_bookmark ->m_startNode ->getNextSibling() )
			{
				l_parentNode = l_bookmark ->m_startNode ->getParentNode();
				l_parentNode ->removeChild( l_bookmark ->m_startNode );

				l_parentNode = l_bookmark ->m_endNode ->getParentNode(); // start and end node might not have the same parent with "_" bookmarks
				l_parentNode ->removeChild( l_bookmark ->m_endNode );

				l_bookmarks.erase( l_bookmarkIterator++ );
				l_bookmarksById.erase( l_bookmarksById.find( l_bookmark ->m_id ) );

				delete l_bookmark;
			}

			else
				l_bookmarkIterator++;
		}

		l_bookmarks.sort( Bookmark::BookmarkCmp );

		l_prevBookmark = NULL;

		FOR_EACH( l_bookmarkIterator, &l_bookmarks )
		{
			l_bookmark = *l_bookmarkIterator;

			// check if it's a new start group
			if (!l_prevBookmark
			  || l_prevBookmark ->m_startGroupSeq != l_bookmark ->m_startGroupSeq )
			{
				l_bookmarkStartNode = l_bookmark ->m_startNode;
				l_parentNode		= l_bookmarkStartNode ->getParentNode();
			}

			else
			{
				l_parentNode ->removeChild( l_bookmark ->m_startNode );

				if( l_bookmarkStartNode == l_parentNode ->getLastChild() )
					l_parentNode ->appendChild( l_bookmark ->m_startNode );

				else
				{
					l_node = l_bookmarkStartNode ->getNextSibling();
					l_parentNode ->insertBefore( l_bookmark ->m_startNode, l_node );
				}

				l_bookmarkStartNode = l_bookmark ->m_startNode;
			}
			
			l_prevBookmark = l_bookmark;
		}

		l_bookmarkIterator = l_bookmarks.begin();

		while ( l_bookmarkIterator != l_bookmarks.end() )
		{
			l_bookmark			= *l_bookmarkIterator;
			l_bookmarkStartNode	= l_bookmark ->m_startNode;
			l_bookmarkEndNode	= l_bookmark ->m_endNode;

			l_parentNode		= l_bookmarkEndNode ->getParentNode();
			l_childNode			= l_bookmarkEndNode;

			while ( true )
			{
				if ( l_childNode == l_bookmarkEndNode )
				{
					if ( XmlFunc::XMLChCmp( l_parentNode ->getLocalName(), _X( "tbl" ) )
					  || XmlFunc::XMLChCmp( l_parentNode ->getLocalName(), _X( "tr" ) ) )

						l_rPrNode = ( xercesc::DOMElement* ) l_parentNode ->getChildNodes() ->item(1);

					else
					if ( XmlFunc::XMLChCmp( l_parentNode ->getLocalName(), _X( "tc" ) )
					  || XmlFunc::XMLChCmp( l_parentNode ->getLocalName(), _X( "p" ) )
					  || XmlFunc::XMLChCmp( l_parentNode ->getLocalName(), _X( "r" ) ) )

						l_rPrNode = ( xercesc::DOMElement* ) l_parentNode ->getChildNodes() ->item(0);

					else
						l_rPrNode = NULL;
				}

				l_lastChildNode = l_childNode;
				l_childNode		= l_childNode ->getPreviousSibling();

				if ( l_childNode == l_rPrNode )
				{
					if ( l_parentNode == m_bodyNode )
						break;

					l_node			= l_parentNode;
					l_parentNode	= l_node ->getParentNode();

					l_node ->removeChild( l_bookmarkEndNode );
					l_parentNode ->insertBefore( l_bookmarkEndNode, l_node );

					l_childNode		= l_bookmarkEndNode;
				}

				else
				// if reached it's own start node
				if ( l_childNode == l_bookmarkStartNode

				// or childnode is not an empty node (start or end nodes)
				  || !( l_childNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
				   && ( XmlFunc::XMLChCmp( l_childNode ->getLocalName(),	_X( "bookmarkStart" ) )
				     || XmlFunc::XMLChCmp( l_childNode ->getLocalName(),	_X( "bookmarkEnd" ) ) )
					 && XmlFunc::XMLChCmp( l_childNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) ) )
					break;
			}

			// if moved backwards
			if ( l_lastChildNode != l_bookmarkEndNode )
			{
				l_parentNode ->removeChild( l_bookmarkEndNode );
				l_parentNode ->insertBefore( l_bookmarkEndNode, l_lastChildNode );
			}



			if ( l_bookmarkEndNode == l_bookmarkStartNode ->getNextSibling() )
			{
				l_bookmarks.erase( l_bookmarkIterator++ );
				l_bookmarksById.erase( l_bookmarksById.find( l_bookmark ->m_id ) );

				delete l_bookmark;
			}

			else
				l_bookmarkIterator++;
		}

		/***
		FOR_EACH( l_bookmarksByIdIterator, &l_bookmarksById )
		{
			l_bookmark			= l_bookmarksByIdIterator ->second;
			l_bookmarkStartNode	= l_bookmark ->m_startNode;
			l_bookmarkEndNode	= l_bookmark ->m_endNode;
			l_parentNode		= l_bookmark ->m_endNode ->getParentNode();

			while ( l_parentNode != m_bodyNode )
			{
				if ( XmlFunc::XMLChCmp( l_parentNode ->getLocalName(), _X( "tbl" ) )
				  || XmlFunc::XMLChCmp( l_parentNode ->getLocalName(), _X( "tr" ) ) )

					l_node = l_parentNode ->getChildNodes() ->item(1); // last rpr node
				else
					l_node = l_parentNode ->getChildNodes() ->item(0);

				l_childNode = l_bookmark ->m_endNode ->getPreviousSibling();

				while ( l_childNode != l_node )
				{
					if ( l_childNode == l_bookmarkStartNode )

						goto _break;

					else
					if ( l_childNode ->getNodeType() != xercesc::DOMNode::ELEMENT_NODE
					|| ( !XmlFunc::XMLChCmp( l_childNode ->getLocalName(),		_X( "bookmarkStart" ) )
					  && !XmlFunc::XMLChCmp( l_childNode ->getLocalName(),		_X( "bookmarkEnd" ) ) )
					  || !XmlFunc::XMLChCmp( l_childNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

						goto _break;

					l_childNode = l_childNode ->getPreviousSibling();
				}

				l_node			= l_parentNode;
				l_parentNode	= l_node ->getParentNode();

				l_node ->removeChild( l_bookmarkEndNode );
				l_parentNode ->insertBefore( l_bookmarkEndNode, l_node );
			}

			_break:;
		}
		***/

		FOR_EACH( l_bookmarkIterator, &l_bookmarks )
		{
			delete *l_bookmarkIterator;
		}

		l_bookmarks.clear();
		l_bookmarksById.clear();
	}

	catch ( ... )
	{
		FOR_EACH( l_bookmarkIterator, &l_bookmarks )
		{
			delete *l_bookmarkIterator;
		}

		l_bookmarks.clear();
		l_bookmarksById.clear();

		throw;
	}
} // normalizeBookmarksStartEnd

void DocxCompilerItemFile::normalizeBookmarksHierarchy( DocxCompilerBookmark* p_bookmark )
{
	list<DocxCompilerBookmark*>::const_iterator	l_childBookmarkIterator;
	const list<DocxCompilerBookmark*>*			l_childBookmarks;
	DocxCompilerBookmark*						l_childBookmark;
	xercesc::DOMElement*						l_lastEndNode	= NULL;
	size_t										l_groupId		= ++m_groupIdSeq;

	if ( !p_bookmark )
		l_childBookmarks = &m_rootBookmarks;
	else
		l_childBookmarks = p_bookmark ->getChildren();

	FOR_EACH( l_childBookmarkIterator, l_childBookmarks )
	{
		l_childBookmark = *l_childBookmarkIterator;
		l_childBookmark ->normalize();

		normalizeBookmarksHierarchy( l_childBookmark );

		if ( !l_lastEndNode || l_childBookmark ->getStartNode() != l_lastEndNode ->getNextSibling() )
			l_groupId = ++m_groupIdSeq;

		l_childBookmark ->setGroupId( l_groupId );
		l_lastEndNode = l_childBookmark ->getEndNode();
	}
} // normalizeBookmarksHierarchy

void DocxCompilerItemFile::normalizeBookmarksBr()
{
	map<size_t, DocxCompilerBookmark*>::iterator	l_bookmarkIterator;
	DocxCompilerBookmark*							l_bookmark;

	boost::scoped_ptr<XmlTreeDriller>				l_cursorDriller;
	xercesc::DOMElement*							l_cursorNode;
	xercesc::DOMDocument*							l_doc		= m_part ->getDoc();

	xercesc::DOMElement*							l_brNode	= NULL;
	xercesc::DOMElement*							l_startNode	= NULL;
	xercesc::DOMElement*							l_endNode;
	size_t											l_bookmarkId;

	l_cursorDriller.reset( new XmlTreeDriller(
		m_bodyNode,
		"", "",
		"http://schemas.openxmlformats.org/wordprocessingml/2006/main", "bookmarkStart,bookmarkEnd,br,sectPr,t,tbl,pict,drawing",
		"", "", "" ) );

	while ( l_cursorNode = ( xercesc::DOMElement* ) l_cursorDriller ->nextNode() )
	{
		if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "bookmarkStart" ) ) )
		{
			l_bookmarkId		= StrFunc::strToInt<size_t>( XmlFunc::XMLChToUtf8( l_cursorNode ->getAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":id" ) ) ) );
			l_bookmarkIterator	= m_bookmarksById.find( l_bookmarkId );

			if( l_bookmarkIterator != m_bookmarksById.end() ) {
				l_bookmark = l_bookmarkIterator ->second;

				if (!l_bookmark ->getParent() )
					l_startNode = l_bookmark ->getStartNode();
				else
					l_startNode = NULL;
			}

			l_brNode = NULL;
		}

		else
		if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "bookmarkEnd" ) ) )
		{
			if ( l_brNode )
			{
				l_bookmarkId		= StrFunc::strToInt<size_t>( XmlFunc::XMLChToUtf8( l_cursorNode ->getAttribute( Utf8ToXMLCh( m_wordMlPrefix + ":id" ) ) ) );
				l_bookmarkIterator	= m_bookmarksById.find( l_bookmarkId );

				if( l_bookmarkIterator != m_bookmarksById.end() ) {
					l_bookmark = l_bookmarkIterator ->second;

					if ( !l_bookmark ->getParent() )
					{
						l_cursorDriller ->setCurrentNode( XmlFunc::getPrevNode( l_cursorNode ) );

						l_endNode = l_bookmark ->getEndNode();
						l_endNode ->getParentNode() ->removeChild( l_endNode );
						m_bodyNode ->insertBefore( l_endNode, l_brNode );
					}

					else
						l_brNode = NULL;
				}
			}

			l_startNode = NULL;
		}

		else
		if ( ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "br" ) )
			|| XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "sectPr" ) ) )
			&& l_cursorNode ->getParentNode() == m_bodyNode )
		{
			l_brNode = l_cursorNode;

			if( l_startNode ) {
				l_startNode ->getParentNode() ->removeChild( l_startNode );

				if ( l_brNode == m_bodyNode ->getLastChild() )
					m_bodyNode ->appendChild( l_startNode );
				else
					m_bodyNode ->insertBefore( l_startNode, l_brNode ->getNextSibling() );

				l_cursorDriller ->setCurrentNode( l_startNode );
			}

			l_startNode	= NULL;
		}

		else
		{
			l_brNode	= NULL;
			l_startNode	= NULL;
		}
	}
} // normalizeBookmarksBr



void DocxCompilerItemFile::trimItems()
{
	list<DocxCompilerItem*>::iterator	l_itemIterator;
	DocxCompilerItem*					l_item;

	boost::scoped_ptr<XmlTreeDriller>	l_cursorDriller;
	xercesc::DOMNode*					l_cursorNode;
	xercesc::DOMNode*					l_node;

	FOR_EACH( l_itemIterator, &m_items )
	{
		l_item = *l_itemIterator;

		l_cursorDriller.reset( new XmlTreeDriller(
			l_item ->getBodyNode(),
			"", "",
			"http://schemas.openxmlformats.org/wordprocessingml/2006/main", "pPr,rPr,tblPr,tblGrid,tblPrEx,trPr,tcPr",
			"", "", "" ) );
			
		while ( l_cursorNode = l_cursorDriller ->nextNode() )
		{
			if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "tblPr" ) ) )
			{
				l_node = XmlFunc::getChildByTagName(
					l_cursorNode,
					NULL,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"tblBorders" );

				if(!l_node ->hasChildNodes() )
					l_cursorNode ->removeChild( l_node );
			}

			else
			if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "tblPrEx" ) ) )
			{
				l_node = XmlFunc::getChildByTagName(
					l_cursorNode,
					NULL,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"tblBorders" );

				if(!l_node ->hasChildNodes() )
					l_cursorNode ->removeChild( l_node );
			}

			else
			if ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "tcPr" ) ) )
			{
				l_node = XmlFunc::getChildByTagName(
					l_cursorNode,
					NULL,
					"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
					"tcBorders" );

				if(!l_node ->hasChildNodes() )
					l_cursorNode ->removeChild( l_node );
			}

			if(!l_cursorNode ->hasChildNodes() )
				l_cursorDriller ->deleteNode();
		}
	}
} // trimItems

void DocxCompilerItemFile::insertItem( DocxCompilerItem* p_item )
{
	m_items.push_back( p_item );
} // insertItem

void DocxCompilerItemFile::insertField( DocxCompilerField* p_field )
{
	m_fields.push_back( p_field );
	p_field ->getItem() ->insertField( p_field );
	m_file ->insertField( p_field );
} // insertField



void DocxCompilerItemFile::serialize( ZipFile* p_zipFile )
{
	list<DocxCompilerItemGroup*>::iterator	l_itemGroupIterator;
	list<DocxCompilerItem*>::iterator		l_itemIterator;
	list<DocxCompilerField*>::iterator		l_fieldIterator;
	size_t									i;

	p_zipFile ->writePtr		( this );
	p_zipFile ->writePtr		( m_file );
	p_zipFile ->writeStr		( m_part ->getFullPath() );
	p_zipFile ->writeNum<int16>	( m_type );

	p_zipFile ->writeStr		( m_body1 );
	p_zipFile ->writeStr		( m_body2 );
	p_zipFile ->writeStr		( m_wordMlPrefix );
	p_zipFile ->writeStr		( m_relMlPrefix );
	p_zipFile ->writeStr		( m_vmlPrefix );

	p_zipFile ->writePtr( m_rootItemGroup );

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_itemGroups.size() );
	FOR_EACH( l_itemGroupIterator, &m_itemGroups )
	{
		p_zipFile ->writePtr( *l_itemGroupIterator );
	}

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_items.size() );	
	FOR_EACH( l_itemIterator, &m_items )
	{
		p_zipFile ->writePtr( *l_itemIterator );
	}

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_fields.size() );
	FOR_EACH( l_fieldIterator, &m_fields )
	{
		p_zipFile ->writePtr( *l_fieldIterator );
	}

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_sects.size() );
	for ( i = 0; i < m_sects.size(); i++ )
	{
		p_zipFile ->writePtr( m_sects[i] );
	}
} // serialize



DocxCompilerFile* DocxCompilerItemFile::getFile() const
{
	return m_file;
} // getFile

OpcPart* DocxCompilerItemFile::getPart() const
{
	return m_part;
} // getPart

DocxCompilerItemFile::ItemFileType DocxCompilerItemFile::getType() const
{
	return m_type;
} // getType

xercesc::DOMElement* DocxCompilerItemFile::getBodyNode() const
{
	return m_bodyNode;
} // getBodyNode

string DocxCompilerItemFile::getWordMlPrefix() const
{
	return m_wordMlPrefix;
} // getWordMlPrefix

string DocxCompilerItemFile::getRelMlPrefix() const
{
	return m_relMlPrefix;
} // getRelMlPrefix

string DocxCompilerItemFile::getVmlPrefix() const
{
	return m_vmlPrefix;
} // getVmlPrefix

const vector<DocxCompilerXmlString*>* DocxCompilerItemFile::getSects() const
{
	return &m_sects;
} // getSects
