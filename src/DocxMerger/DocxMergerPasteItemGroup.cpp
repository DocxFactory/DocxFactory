
#include "DocxFactory/DocxMerger/DocxMergerItemGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerItemLayout.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteItemGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteItem.h"

#include "DocxFactory/util/DocxFactoryDefs.h"

using namespace DocxFactory;
using namespace std;



DocxMergerPasteItemGroup::DocxMergerPasteItemGroup( DocxMergerItemGroup* p_itemGroup, DocxMergerPasteItem* p_pasteItem )
{
	m_itemGroup	= p_itemGroup;
	m_pasteItem	= p_pasteItem;
	m_trCnt		= 0.0f;
	m_fill		= 0;
} // c'tor

DocxMergerPasteItemGroup::~DocxMergerPasteItemGroup()
{
	list<DocxMergerPasteItem*>::iterator l_pasteItemIterator;
	FOR_EACH( l_pasteItemIterator, &m_childPasteItems )
	{
		delete *l_pasteItemIterator;
	}
} // d'tor



void DocxMergerPasteItemGroup::pushBackChildPasteItem( DocxMergerPasteItem* p_pasteItem )
{
	m_childPasteItems.push_back( p_pasteItem );
	p_pasteItem ->setPasteItemGroup( this );

	if( p_pasteItem ->getItem() ->getItemLayout() )
	{
		p_pasteItem ->setTrCnt();

		if ( !p_pasteItem ->getItem() ->isTopLevel() )
		setTrCnt( m_trCnt + p_pasteItem ->getTrCnt() 
			+ ( ( m_itemGroup ->getSpacerItem() && m_childPasteItems.size() > 1 ) ? m_itemGroup ->getSpacerTrCnt() : 0.0f ) );
	}
} // pushBackChildPasteItem

void DocxMergerPasteItemGroup::popBackChildPasteItem()
{
	DocxMergerPasteItem* l_pasteItem = m_childPasteItems.back();
	m_childPasteItems.pop_back();

	if( l_pasteItem ->getItem() ->getItemLayout() )
		setTrCnt( m_trCnt - l_pasteItem ->getTrCnt()
			- ( ( m_itemGroup ->getSpacerItem() && m_childPasteItems.size() > 0 ) ? m_itemGroup ->getSpacerTrCnt() : 0.0f ) );
} // popBackChildPasteItem

void DocxMergerPasteItemGroup::pushFrontChildPasteItem( DocxMergerPasteItem* p_pasteItem )
{
	m_childPasteItems.push_front( p_pasteItem );
	p_pasteItem ->setPasteItemGroup( this );

	if( p_pasteItem ->getItem() ->getItemLayout() )
	{
		p_pasteItem ->setTrCnt();

		if ( !p_pasteItem ->getItem() ->isTopLevel() )
		setTrCnt( m_trCnt + p_pasteItem ->getTrCnt()
			+ ( ( m_itemGroup ->getSpacerItem() && m_childPasteItems.size() > 1 ) ? m_itemGroup ->getSpacerTrCnt() : 0.0f ) );
	}
} // pushFrontChildPasteItem

void DocxMergerPasteItemGroup::popFrontChildPasteItem()
{
	DocxMergerPasteItem* l_pasteItem = m_childPasteItems.front();
	m_childPasteItems.pop_front();

	if( l_pasteItem ->getItem() ->getItemLayout() )
		setTrCnt( m_trCnt - l_pasteItem ->getTrCnt()
			- ( ( m_itemGroup ->getSpacerItem() && m_childPasteItems.size() > 0 ) ? m_itemGroup ->getSpacerTrCnt() : 0.0f ) );
} // popFrontChildPasteItem



void DocxMergerPasteItemGroup::setTrCnt( double p_trCnt )
{
	double l_trCnt;

	m_trCnt = p_trCnt;

	if( m_trCnt < 0 )
		m_trCnt = 0;

	l_trCnt = m_pasteItem ->getTrCnt();

	m_pasteItem ->setTrCnt();
		
	if (!m_pasteItem ->getItem() ->isTopLevel() )
		 m_pasteItem ->getPasteItemGroup() ->setTrCnt( m_pasteItem ->getPasteItemGroup() ->getTrCnt() - l_trCnt + m_pasteItem ->getTrCnt() );
} // setTrCnt

void DocxMergerPasteItemGroup::setFill( unsigned int p_fill )
{
	if ( m_pasteItem ->getItem() ->getItemLayout() )
	{
		if ( m_itemGroup ->getFillItemEqual() )
		{
			double l_trCnt = m_itemGroup ->getFillItemTrCnt();

			if ( m_fill > 0 )
				setTrCnt( m_trCnt + p_fill * l_trCnt - m_fill * l_trCnt );
			else
				setTrCnt( m_trCnt + p_fill * l_trCnt );
		}

		else
			setTrCnt( m_trCnt - m_fill + p_fill );
	}

	m_fill = p_fill;
} // setFill



DocxMergerItemGroup* DocxMergerPasteItemGroup::getItemGroup() const
{
	return m_itemGroup;
} // getItemGroup

DocxMergerPasteItem* DocxMergerPasteItemGroup::getPasteItem() const
{
	return m_pasteItem;
} // getPasteItem

const list<DocxMergerPasteItem*>* DocxMergerPasteItemGroup::getChildPasteItems() const
{
	return &m_childPasteItems;
} // getChildPasteItems

double DocxMergerPasteItemGroup::getTrCnt() const
{
	return m_trCnt;
} // getTrCnt

unsigned int DocxMergerPasteItemGroup::getFill() const
{
	return m_fill;
} // getFill
