
#include "DocxFactory/DocxCompiler/DocxCompilerItemFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItemGroup.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItemLayout.h"

#include "DocxFactory/xml/XmlFunc.h"
#include "DocxFactory/xml/XmlTreeDriller.h"
#include "DocxFactory/xml/Utf8ToXMLCh.h"

#include "DocxFactory/zip/ZipFile.h"

#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include "xercesc/util/XercesDefs.hpp"
#include "xercesc/dom/DOM.hpp"

#include "boost/scoped_ptr.hpp"

using namespace DocxFactory;
using namespace std;



DocxCompilerItemLayout* DocxCompilerItemLayout::createItemLayout(
	DocxCompilerItemFile*	p_itemFile,
	xercesc::DOMElement*	p_containerNode,
	xercesc::DOMNode*		p_fromNode,
	double					p_fixedTrCnt )
{
	DocxCompilerItemLayout* l_itemLayout = new DocxCompilerItemLayout(
		p_itemFile,
		p_containerNode,
		p_fromNode,
		p_fixedTrCnt );

	if ( l_itemLayout ->empty() )
	{
		delete l_itemLayout;
		l_itemLayout = NULL;
	}

	return l_itemLayout;
} // createItemLayout

DocxCompilerItemLayout::DocxCompilerItemLayout(
	DocxCompilerItemFile*	p_itemFile,
	xercesc::DOMElement*	p_containerNode,
	xercesc::DOMNode*		p_fromNode,
	double					p_fixedTrCnt )
{
	list<pair<bool, vector<pair<unsigned short, DocxCompilerItemLayout*>>>>::iterator	l_trIterator;
	pair<bool, vector<pair<unsigned short, DocxCompilerItemLayout*>>>*					l_tr;
	pair<bool, vector<pair<unsigned short, DocxCompilerItemLayout*>>>*					l_trLast;

	vector<pair<unsigned short, DocxCompilerItemLayout*>>::iterator						l_tcIterator;
	pair<unsigned short, DocxCompilerItemLayout*>*										l_tc;
	pair<unsigned short, DocxCompilerItemLayout*>*										l_tcLast;

	boost::scoped_ptr<XmlTreeDriller>	l_cursorDriller;
	xercesc::DOMElement*				l_cursorNode;
	string								l_wordMlPrefix = p_itemFile ->getWordMlPrefix();

	xercesc::DOMNode*					l_tcNode;
	xercesc::DOMNode*					l_tcPrNode;
	unsigned short						l_tcPos;

	bool								l_vMerge;
	unsigned short						l_span;

	xercesc::DOMElement*				l_node;
	string								l_str;

	l_trLast = NULL;

	l_cursorDriller.reset( new XmlTreeDriller(
		p_containerNode,
		"", "",
		"", "placeholder,tr",
		"", "", "" ) );

	if ( p_fromNode )
		l_cursorDriller ->setCurrentNode( p_fromNode );

	while ( l_cursorNode = ( xercesc::DOMElement* ) l_cursorDriller ->nextNode() )
	{
		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
		  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "tr" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
		{
			m_trList.push_back( make_pair( false, vector<pair<unsigned short, DocxCompilerItemLayout*>>() ) );
			l_tr		= &m_trList.back();

			l_tcNode	= l_cursorNode ->getFirstChild() ->getNextSibling(); // tblPrEx + trPr
			l_tcPos		= 0;

			while ( l_tcNode = l_tcNode ->getNextSibling() )
			{
				if ( l_tcNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
				  && XmlFunc::XMLChCmp( l_tcNode ->getLocalName(),		_X( "tc" ) )
				  && XmlFunc::XMLChCmp( l_tcNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
				{
					l_vMerge	= false;
					l_span		= 0;

					l_tcPrNode	= l_tcNode ->getFirstChild();
					l_node		= NULL;

					while ( l_node = XmlFunc::getChildByTagName( l_tcPrNode, l_node, "http://schemas.openxmlformats.org/wordprocessingml/2006/main", "vMerge,gridSpan" ) )
					{
						if ( XmlFunc::XMLChCmp( l_node ->getLocalName(), _X( "vMerge" ) ) )
						{
							l_str		= XmlFunc::XMLChToUtf8( l_node ->getAttribute( Utf8ToXMLCh( l_wordMlPrefix + ":val" ) ) );
							l_vMerge	= ( l_str.empty() || StrFunc::lc( l_str ) == "continue" );
						}

						else
						if ( XmlFunc::XMLChCmp( l_node ->getLocalName(), _X( "gridSpan" ) ) )
						{
							l_str		= XmlFunc::XMLChToUtf8( l_node ->getAttribute( Utf8ToXMLCh( l_wordMlPrefix + ":val" ) ) );
							l_span		= StrFunc::strToInt<unsigned short>( l_str );
						}
					}

					l_tr ->second.push_back( make_pair( 0, ( DocxCompilerItemLayout* ) NULL ) );
					l_tc = &l_tr ->second.back();

					if ( l_vMerge

					  && l_trLast
					  && l_trLast ->second.size() > l_tcPos
					  && l_trLast ->second[ l_tcPos ].second )
					{
						l_tcLast = &l_trLast ->second[ l_tcPos ];
						l_tc ->first		= l_tcLast ->first + 1;
						l_tc ->second		= l_tcLast ->second;

						l_trLast ->first	= true;
						l_tcLast ->first	= 0;
						l_tcLast ->second	= NULL;
					}

					else
					{
						l_tc ->second = DocxCompilerItemLayout::createItemLayout(
							p_itemFile, 
							( xercesc::DOMElement* ) l_tcNode,
							l_tcPrNode,
							0.0f );
					}

					l_tcPos++;
					for ( ; l_span > 1; l_span-- )
					{
						l_tr ->second.push_back( make_pair( 0, ( DocxCompilerItemLayout* ) NULL ) );
						l_tcPos++;
					}
				}
			}

			l_trLast = l_tr;
			l_cursorDriller ->setCurrentNode( l_cursorNode, XmlTreeDriller::CURSOR_AFTER );
		}

		else
		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
  		  && XmlFunc::XMLChCmp( l_cursorNode ->getNodeName(), _X( "placeholder" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getAttribute( _X( "type" ) ), _X( "item-group" ) ) )
		{
			l_str = XmlFunc::XMLChToUtf8( l_cursorNode ->getAttribute( _X( "ptr" ) ) );
			m_childItemGroups.push_back( ( DocxCompilerItemGroup* ) StrFunc::strToInt<size_t>( l_str ) );
		}
	}

	m_fixedTrCnt = 0.0f;

	l_trIterator = m_trList.begin();
	while ( l_trIterator != m_trList.end() )
	{
		l_tcIterator = l_trIterator ->second.begin();
		while ( l_tcIterator != l_trIterator ->second.end() )
		{
			if( l_tcIterator ->first	== 0
			 && l_tcIterator ->second	== NULL )
				l_trIterator ->second.erase( l_tcIterator );

			else
				l_tcIterator++;
		}

		if( l_trIterator ->first == false
		 && l_trIterator ->second.empty() )
		{
			m_trList.erase( l_trIterator++ );
			m_fixedTrCnt++;
		}

		else
			l_trIterator++;
	}

	if( p_fixedTrCnt > 0 )
		m_fixedTrCnt = p_fixedTrCnt;
} // c'tor

DocxCompilerItemLayout::~DocxCompilerItemLayout()
{
	list<pair<bool, vector<pair<unsigned short, DocxCompilerItemLayout*>>>>::iterator	l_trIterator;
	vector<pair<unsigned short, DocxCompilerItemLayout*>>::iterator						l_tcIterator;

	FOR_EACH( l_trIterator, &m_trList )
	{
		FOR_EACH( l_tcIterator, &l_trIterator ->second )
		{
			delete l_tcIterator ->second;
			l_tcIterator ->second = NULL;
		}
	}
}; // d'tor



void DocxCompilerItemLayout::serializeItemLayout(
	ZipFile*				p_zipFile,
	DocxCompilerItemLayout*	p_itemLayout )
{
	if ( !p_itemLayout )
		p_zipFile ->writeNum<uint8>( false );
	
	else
	{
		p_zipFile ->writeNum<uint8>( true );
		p_itemLayout ->serialize( p_zipFile );
	}
} // serializeItemLayout

void DocxCompilerItemLayout::serialize( ZipFile* p_zipFile )
{
	list<pair<bool, vector<pair<unsigned short, DocxCompilerItemLayout*>>>>::iterator	l_trIterator;
	vector<pair<unsigned short, DocxCompilerItemLayout*>>::iterator						l_tcIterator;
	list<DocxCompilerItemGroup*>::iterator												l_itemGroupIterator;

	p_zipFile ->writeNum<double>( m_fixedTrCnt );
	p_zipFile ->writeNum<uint32>( ( uint32 ) m_trList.size() );
	FOR_EACH( l_trIterator, &m_trList )
	{
		p_zipFile ->writeNum<uint32>( ( uint32 ) l_trIterator ->second.size() );
		FOR_EACH( l_tcIterator, &l_trIterator ->second )
		{
			p_zipFile ->writeNum<uint16>( l_tcIterator ->first );
			DocxCompilerItemLayout::serializeItemLayout( p_zipFile, l_tcIterator ->second );
		}
	}

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_childItemGroups.size() );
	FOR_EACH( l_itemGroupIterator, &m_childItemGroups )
	{
		p_zipFile ->writePtr( *l_itemGroupIterator );
	}
} // serialize



double DocxCompilerItemLayout::getFixedTrCnt() const
{
	return m_fixedTrCnt;
} // getFixedTrCnt

const list<pair<bool, vector<pair<unsigned short, DocxCompilerItemLayout*>>>>* DocxCompilerItemLayout::getTrList() const
{
	return &m_trList;
} // getTrList

const list<DocxCompilerItemGroup*>* DocxCompilerItemLayout::getChildItemGroups() const
{
	return &m_childItemGroups;
} // getChildItemGroups

bool DocxCompilerItemLayout::empty() const
{
	if ( m_fixedTrCnt == 0
	  && m_trList.empty()
	  && m_childItemGroups.empty() )
		return true;
	else
		return false;
} // empty
