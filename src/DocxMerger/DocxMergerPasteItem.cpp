
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerItemLayout.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteItemGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteItem.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteFieldGroup.h"

#include "DocxFactory/util/DocxFactoryDefs.h"

using namespace DocxFactory;
using namespace std;



DocxMergerPasteItem::DocxMergerPasteItem( DocxMergerItem* p_item, DocxMergerPasteFieldGroup* p_pasteFieldGroup )
{
	m_item					= p_item;
	m_pasteFieldGroup		= p_pasteFieldGroup;
	m_fillPasteItemParent	= NULL;
	m_trCnt					= 0.0f;
} // c'tor

DocxMergerPasteItem::~DocxMergerPasteItem()
{
	map<DocxMergerItemGroup*, DocxMergerPasteItemGroup*>::iterator l_pasteItemGroupIterator;
	FOR_EACH( l_pasteItemGroupIterator, &m_childPasteItemGroupsByGroup )
	{
		delete l_pasteItemGroupIterator ->second;
	}
} // d'tor



void DocxMergerPasteItem::pushBackChildPasteItem( DocxMergerPasteItem* p_pasteItem )
{
	DocxMergerPasteItemGroup* l_pasteItemGroup;

	l_pasteItemGroup = insertChildPasteItemGroup( p_pasteItem ->getItem() ->getItemGroup() );
	l_pasteItemGroup ->pushBackChildPasteItem( p_pasteItem );
} // pushBackChildPasteItem

void DocxMergerPasteItem::popBackChildPasteItem( DocxMergerPasteItemGroup* p_pasteItemGroup )
{
	p_pasteItemGroup ->popBackChildPasteItem();

	if( p_pasteItemGroup ->getChildPasteItems() ->empty() )
		removeChildPasteItemGroup( p_pasteItemGroup ->getItemGroup() );
} // popBackChildPasteItem

void DocxMergerPasteItem::pushFrontChildPasteItem( DocxMergerPasteItem* p_pasteItem )
{
	DocxMergerPasteItemGroup* l_pasteItemGroup;

	l_pasteItemGroup = insertChildPasteItemGroup( p_pasteItem ->getItem() ->getItemGroup() );
	l_pasteItemGroup ->pushFrontChildPasteItem( p_pasteItem );
} // pushFrontChildPasteItem

void DocxMergerPasteItem::popFrontChildPasteItem( DocxMergerPasteItemGroup* p_pasteItemGroup )
{
	p_pasteItemGroup ->popFrontChildPasteItem();

	if( p_pasteItemGroup ->getChildPasteItems() ->empty() )
		removeChildPasteItemGroup( p_pasteItemGroup ->getItemGroup() );
} // popFrontChildPasteItem



DocxMergerPasteItemGroup* DocxMergerPasteItem::insertChildPasteItemGroup( DocxMergerItemGroup* p_itemGroup )
{
	map<DocxMergerItemGroup*, DocxMergerPasteItemGroup*>::iterator	l_pasteItemGroupIterator;
	DocxMergerPasteItemGroup*										l_pasteItemGroup;

	l_pasteItemGroupIterator = m_childPasteItemGroupsByGroup.find( p_itemGroup );
	if ( l_pasteItemGroupIterator == m_childPasteItemGroupsByGroup.end() )
	{
		l_pasteItemGroup = new DocxMergerPasteItemGroup( p_itemGroup, this );
		m_childPasteItemGroupsByGroup.insert( make_pair( p_itemGroup, l_pasteItemGroup ) );
	}

	else
		l_pasteItemGroup = l_pasteItemGroupIterator ->second;

	return l_pasteItemGroup;
} // insertChildPasteItemGroup

void DocxMergerPasteItem::removeChildPasteItemGroup( DocxMergerItemGroup* p_itemGroup )
{
	map<DocxMergerItemGroup*, DocxMergerPasteItemGroup*>::iterator l_pasteItemGroupIterator = m_childPasteItemGroupsByGroup.find( p_itemGroup );
	if( l_pasteItemGroupIterator == m_childPasteItemGroupsByGroup.end() )
	{
		delete l_pasteItemGroupIterator ->second;
		m_childPasteItemGroupsByGroup.erase( l_pasteItemGroupIterator );
	}
} // removeChildPasteItem



void DocxMergerPasteItem::setPasteItemGroup( DocxMergerPasteItemGroup* p_pasteItemGroup )
{
	m_pasteItemGroup = p_pasteItemGroup;
} // setPasteItemGroup

void DocxMergerPasteItem::setFillPasteItemParent( DocxMergerPasteItem* p_pasteItem )
{
	m_fillPasteItemParent = p_pasteItem;
} // setFillPasteItemParent

void DocxMergerPasteItem::setTrCnt()
{
	m_trCnt = m_item ->getItemLayout() ->calcTrCnt( this );
} // setTrCnt



DocxMergerItem* DocxMergerPasteItem::getItem() const
{
	return m_item;
} // getItem

DocxMergerPasteItemGroup* DocxMergerPasteItem::getPasteItemGroup() const
{
	return m_pasteItemGroup;
} // getPasteItemGroup

DocxMergerPasteFieldGroup* DocxMergerPasteItem::getPasteFieldGroup() const
{
	return m_pasteFieldGroup;
} // getPasteFieldGroup

const map<DocxMergerItemGroup*, DocxMergerPasteItemGroup*>* DocxMergerPasteItem::getChildPasteItemGroupsByGroup() const
{
	return &m_childPasteItemGroupsByGroup;
} // getChildPasteItemGroupsByGroup

DocxMergerPasteItem* DocxMergerPasteItem::getFillPasteItemParent() const
{
	return m_fillPasteItemParent;
} // getFillPasteItemParent

double DocxMergerPasteItem::getTrCnt() const
{
	return m_trCnt;
} // getTrCnt
