
#include "DocxFactory/DocxMerger/DocxMergerItemFile.h"
#include "DocxFactory/DocxMerger/DocxMergerItemGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerXmlString.h"

#include "DocxFactory/DocxMerger/DocxMergerPasteItemGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteItem.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/UnzipFile.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

using namespace DocxFactory;
using namespace std;



DocxMergerItemGroup::DocxMergerItemGroup()
{

} // c'tor

DocxMergerItemGroup::~DocxMergerItemGroup()
{
	map<string, DocxMergerItem*>::iterator l_itemIterator;
	FOR_EACH( l_itemIterator, &m_childItemsByName )
	{
		delete l_itemIterator ->second;
	}
} // d'tor



void DocxMergerItemGroup::save( DocxMergerPasteItemGroup* p_pasteItemGroup )
{
	list<DocxMergerPasteItem*>::const_iterator	l_pasteItemIterator;
	const list<DocxMergerPasteItem*>*			l_pasteItems;
	DocxMergerPasteItem*						l_pasteItem;

	DocxMergerItem*								l_item;
	bool										l_shadeFlag;
	bool										l_filled = false;

	if ( m_striped )
	{
		l_shadeFlag = false;
		m_itemFile ->pushBackShade( m_shade2 );
	}

	if ( p_pasteItemGroup )
	{
		l_pasteItems = p_pasteItemGroup ->getChildPasteItems();
		FOR_EACH( l_pasteItemIterator, l_pasteItems )
		{
			l_pasteItem	= *l_pasteItemIterator;
			l_item		= l_pasteItem ->getItem();

			if ( l_item ->isPageBottom() 
			
			  && p_pasteItemGroup ->getFill() > 0
			  && !l_filled )
			{
				fill(
					p_pasteItemGroup,
					l_shadeFlag );

				l_filled = true;
			}

			if ( m_spacerItem && l_pasteItemIterator != l_pasteItems ->begin() )
			{
				if ( m_striped )
				{
					l_shadeFlag = !l_shadeFlag;
					m_itemFile ->setShadeBack( l_shadeFlag ? m_shade1 : m_shade2 );
				}

				m_spacerItem ->save( NULL );
			}

			if ( m_striped )
			{
				l_shadeFlag = !l_shadeFlag;
				m_itemFile ->setShadeBack( l_shadeFlag ? m_shade1 : m_shade2 );
			}

			l_item ->save( l_pasteItem );
		}
	}

	if ( p_pasteItemGroup ->getFill() > 0
	 && !l_filled )
	{
		fill(
			p_pasteItemGroup,
			l_shadeFlag );

		l_filled = true;
	}

	if ( m_striped )
		m_itemFile ->popBackShade();
} // save

void DocxMergerItemGroup::fill(
	DocxMergerPasteItemGroup*	p_pasteItemGroup,
	bool&						p_shadeFlag )
{
	DocxMergerItem*	l_item;
	size_t			l_len;
	size_t			l_pos;

	if ( m_striped && !m_fillItemEqual )
	{
		p_shadeFlag = !p_shadeFlag;
		m_itemFile ->setShadeBack( p_shadeFlag ? m_shade1 : m_shade2 );
	}

	l_len = p_pasteItemGroup ->getFill();
	for ( l_pos = 0; l_pos < l_len; l_pos++ )
	{
		if ( m_striped && m_fillItemEqual )
		{
			p_shadeFlag = !p_shadeFlag;
			m_itemFile ->setShadeBack( p_shadeFlag ? m_shade1 : m_shade2 );
		}

			 if ( l_len == 1 )			l_item = m_fillItemSingle;
		else if ( l_pos == 0 )			l_item = m_fillItemTop;
		else if ( l_pos == l_len - 1 )	l_item = m_fillItemBottom;
		else							l_item = m_fillItemMiddle;

		l_item ->save( NULL );
	}
} // fill



void DocxMergerItemGroup::deserialize( UnzipFile* p_unzipFile )
{
	string	l_str;
	uint32	l_ptrSeq;
	uint32	l_size;
	uint32	i;

	p_unzipFile ->insertPtrBySeq( p_unzipFile ->readNum<uint32>(), this );

	m_itemFile			= ( DocxMergerItemFile* )	p_unzipFile ->readNum<uint32>();
	m_item				= ( DocxMergerItem* )		p_unzipFile ->readNum<uint32>();

	m_fillItemTop		= ( DocxMergerItem* )		p_unzipFile ->readNum<uint32>();
	m_fillItemMiddle	= ( DocxMergerItem* )		p_unzipFile ->readNum<uint32>();
	m_fillItemBottom	= ( DocxMergerItem* )		p_unzipFile ->readNum<uint32>();
	m_fillItemSingle	= ( DocxMergerItem* )		p_unzipFile ->readNum<uint32>();
	m_fillItemEqual		= p_unzipFile ->readNum<uint8>() != 0;
	m_fillItemTrCnt		= p_unzipFile ->readNum<double>();

	m_spacerItem		= ( DocxMergerItem* )		p_unzipFile ->readNum<uint32>();
	m_spacerTrCnt		= p_unzipFile ->readNum<double>();

	m_striped			= p_unzipFile ->readNum<uint8>() != 0;
	m_shade1			= p_unzipFile ->readStr();
	m_shade2			= p_unzipFile ->readStr();

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_str		= p_unzipFile ->readStr();
		l_ptrSeq	= p_unzipFile ->readNum<uint32>();
		m_childItemsByName.insert( make_pair( l_str, ( DocxMergerItem* ) l_ptrSeq ) );
	}
} // deserialize

void DocxMergerItemGroup::link( UnzipFile* p_unzipFile )
{
	map<string, DocxMergerItem*>::iterator	l_itemIterator;

	const map<uint32, void*>*	l_ptrsBySeq = p_unzipFile ->getPtrsBySeq();
	uint32						l_ptrSeq;

	l_ptrSeq			= ( uint32 ) m_itemFile;
	m_itemFile			= ( DocxMergerItemFile* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;

	l_ptrSeq			= ( uint32 ) m_item;
	m_item				= ( DocxMergerItem* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;

	l_ptrSeq			= ( uint32 ) m_fillItemTop;
	m_fillItemTop		= ( DocxMergerItem* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;

	l_ptrSeq			= ( uint32 ) m_fillItemMiddle;
	m_fillItemMiddle	= ( DocxMergerItem* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;

	l_ptrSeq			= ( uint32 ) m_fillItemBottom;
	m_fillItemBottom	= ( DocxMergerItem* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;

	l_ptrSeq			= ( uint32 ) m_fillItemSingle;
	m_fillItemSingle	= ( DocxMergerItem* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;

	l_ptrSeq			= ( uint32 ) m_spacerItem;
	m_spacerItem		= ( DocxMergerItem* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;

	FOR_EACH( l_itemIterator, &m_childItemsByName )
	{
		l_ptrSeq				= ( uint32 ) l_itemIterator ->second;
		l_itemIterator ->second	= ( DocxMergerItem* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;
	}
} // link



DocxMergerItem* DocxMergerItemGroup::getItem() const
{
	return m_item;
} // getItem;

const map<string, DocxMergerItem*>* DocxMergerItemGroup::getChildItemsByName() const
{
	return &m_childItemsByName;
} // getChildItemsByName

DocxMergerItem* DocxMergerItemGroup::getSpacerItem() const
{
	return m_spacerItem;
} // getSpacerItem;

double DocxMergerItemGroup::getSpacerTrCnt() const
{
	return m_spacerTrCnt;
} // getSpacerTrCnt;

bool DocxMergerItemGroup::getFillItemEqual() const
{
	return m_fillItemEqual;
} // getFillItemEqual

double DocxMergerItemGroup::getFillItemTrCnt() const
{
	return m_fillItemTrCnt;
} // getFillItemTrCnt;
