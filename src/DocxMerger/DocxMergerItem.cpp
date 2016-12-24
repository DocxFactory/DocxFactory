
#include "DocxFactory/DocxMerger/DocxMergerItemFile.h"
#include "DocxFactory/DocxMerger/DocxMergerItemGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerItemLayout.h"
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerXmlString.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteItem.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteItemGroup.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/UnzipFile.h"

#include "DocxFactory/math/MathFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

using namespace DocxFactory;
using namespace std;



DocxMergerItem::DocxMergerItem()
{

} // c'tor

DocxMergerItem::~DocxMergerItem()
{
	list<DocxMergerItemGroup*>::iterator l_itemGroupIterator;
	FOR_EACH( l_itemGroupIterator, &m_childItemGroups )
	{
		delete *l_itemGroupIterator;
	}

	multimap<string, DocxMergerField*>::iterator l_fieldIterator;
	FOR_EACH( l_fieldIterator, &m_fieldsByName )
	{
		delete l_fieldIterator ->second;
	}

	list<DocxMergerXmlString*>::iterator l_xmlStringIterator;
	FOR_EACH( l_xmlStringIterator, &m_xmlStrings )
	{
		delete *l_xmlStringIterator;
	}

	if ( m_itemLayout )
		delete m_itemLayout;
} // d'tor



void DocxMergerItem::save( DocxMergerPasteItem* p_pasteItem )
{
	ZipFile* l_zipFile = m_itemFile ->getZipFile();

	map<DocxMergerItemGroup*, DocxMergerPasteItemGroup*>::const_iterator	l_pasteItemGroupIterator;
	const map<DocxMergerItemGroup*, DocxMergerPasteItemGroup*>*				l_pasteItemGroups;
	DocxMergerPasteItemGroup*												l_pasteItemGroup;
	DocxMergerItemGroup*													l_itemGroup;

	list<DocxMergerXmlString*>::const_iterator								l_xmlStringIterator;
	DocxMergerXmlString*													l_xmlString;
	DocxMergerXmlString*													l_tableStartString;
	DocxMergerXmlString*													l_tableEndString;

	if ( p_pasteItem )
	{
		if ( p_pasteItem ->getFillPasteItemParent()
		  && MathFunc::cmpLt( p_pasteItem ->getTrCnt(), m_pageSize ) )

			fill( p_pasteItem );

		FOR_EACH( l_xmlStringIterator, &m_xmlStrings )
		{
			l_xmlString = *l_xmlStringIterator;

			if ( l_xmlString ->getType() == DocxMergerXmlString::TYPE_EMPTY_TABLE_START )
			{
				l_tableStartString	= *l_xmlStringIterator;
				l_xmlStringIterator++;

				l_xmlString			= *l_xmlStringIterator;
				l_xmlStringIterator++;

				l_tableEndString	= *l_xmlStringIterator;
				l_xmlStringIterator++;
			}

			else
			{
				l_tableStartString	= NULL;
				l_tableEndString	= NULL;
			}

			if ( l_xmlString ->getType() == DocxMergerXmlString::TYPE_ITEM_GROUP )
			{
				l_itemGroup					= ( DocxMergerItemGroup* ) l_xmlString ->getPtr();
				l_pasteItemGroups			= p_pasteItem ->getChildPasteItemGroupsByGroup();
				l_pasteItemGroupIterator	= l_pasteItemGroups ->find( l_itemGroup );

				if( l_pasteItemGroupIterator != l_pasteItemGroups ->end() ) {
					l_pasteItemGroup = l_pasteItemGroupIterator ->second;

					if ( l_tableStartString )
						( *l_zipFile ) << *l_tableStartString ->getStr();

					l_itemGroup ->save( l_pasteItemGroup );

					if ( l_tableEndString )
						( *l_zipFile ) << *l_tableEndString ->getStr();
				}
			}

			else
			{
				l_xmlString ->save( p_pasteItem ->getPasteFieldGroup() );
			}
		}
	}

	else
	{
		FOR_EACH( l_xmlStringIterator, &m_xmlStrings )
		{
			l_xmlString = *l_xmlStringIterator;

			if( l_xmlString ->getType() != DocxMergerXmlString::TYPE_ITEM_GROUP
			 && l_xmlString ->getType() != DocxMergerXmlString::TYPE_EMPTY_TABLE_START
			 && l_xmlString ->getType() != DocxMergerXmlString::TYPE_EMPTY_TABLE_END )
				l_xmlString ->save( NULL );
		}
	}
} // save

void DocxMergerItem::fill( DocxMergerPasteItem* p_pasteItem )
{
	DocxMergerPasteItem*		l_pasteItemParent;
	DocxMergerPasteItemGroup*	l_pasteItemGroup;
	double						l_trCnt;
	unsigned int				l_fill;

	l_pasteItemParent = p_pasteItem ->getFillPasteItemParent();

	if ( MathFunc::cmpLt( p_pasteItem ->getTrCnt(), m_pageSize ) )
	{
		l_pasteItemGroup = l_pasteItemParent ->insertChildPasteItemGroup( m_fillItemGroup );

		l_trCnt	= m_fillItemGroup ->getFillItemTrCnt();

		if ( m_fillItemGroup ->getFillItemEqual() )
		{
			if( l_trCnt == 1.0f )
				l_fill = ( unsigned int ) floor( ( m_pageSize - p_pasteItem ->getTrCnt() + MathFunc::m_epsilon ) + MathFunc::m_epsilon );
			else
				l_fill = ( unsigned int ) floor( ( ( m_pageSize - p_pasteItem ->getTrCnt() + MathFunc::m_epsilon ) / l_trCnt + MathFunc::m_epsilon ) + MathFunc::m_epsilon );

			if( l_fill > 0 )
				l_pasteItemGroup ->setFill( l_fill );

			while ( MathFunc::cmpLe( p_pasteItem ->getTrCnt() + l_trCnt, m_pageSize ) )
			{
				l_pasteItemGroup ->setFill( l_pasteItemGroup ->getFill() + 1 );
			}
		}

		else
		{
			l_fill = ( unsigned int ) floor( ( m_pageSize - p_pasteItem ->getTrCnt() + MathFunc::m_epsilon ) + MathFunc::m_epsilon );

			if( l_fill > 0 )
				l_pasteItemGroup ->setFill( l_fill );

			while ( MathFunc::cmpLe( p_pasteItem ->getTrCnt() + 1.0f, m_pageSize ) )
			{
				l_pasteItemGroup ->setFill( l_pasteItemGroup ->getFill() + 1 );
			}

			if( l_trCnt > 0.0f && l_fill < ( unsigned int ) ( l_trCnt + 0.5f ) )
				l_pasteItemGroup ->setFill( 0 );
		}
	}
} // fill



void DocxMergerItem::deserialize( UnzipFile* p_unzipFile )
{
	string	l_str;
	uint32	l_ptrSeq;
	uint32	l_size;
	uint32	i;

	p_unzipFile ->insertPtrBySeq( p_unzipFile ->readNum<uint32>(), this );

	m_itemFile			= ( DocxMergerItemFile* )	p_unzipFile ->readNum<uint32>();
	m_itemGroup			= ( DocxMergerItemGroup* )	p_unzipFile ->readNum<uint32>();
	m_name				=							p_unzipFile ->readStr();

	m_topLevel			=							p_unzipFile ->readNum<uint8> () != 0;
	m_fillItemParent	= ( DocxMergerItem* )		p_unzipFile ->readNum<uint32>();
	m_fillItemGroup		= ( DocxMergerItemGroup* )	p_unzipFile ->readNum<uint32>();

	m_pageSize			= p_unzipFile ->readNum<double>();
	m_pageBottom		= p_unzipFile ->readNum<uint8> () != 0;
	m_keepTogether		= p_unzipFile ->readNum<uint8> () != 0;
	m_keepWithPrev		= p_unzipFile ->readNum<uint8> () != 0;

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_ptrSeq	= p_unzipFile ->readNum<uint32>();
		m_path.push_back( ( DocxMergerItem* ) l_ptrSeq );
	}

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_ptrSeq	= p_unzipFile ->readNum<uint32>();
		m_childItemGroups.push_back( ( DocxMergerItemGroup* ) l_ptrSeq );
	}

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_str		= p_unzipFile ->readStr();
		l_ptrSeq	= p_unzipFile ->readNum<uint32>();
		m_fieldsByName.insert( make_pair( l_str, ( DocxMergerField* ) l_ptrSeq ) );
	}

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_ptrSeq	= p_unzipFile ->readNum<uint32>();
		m_xmlStrings.push_back( ( DocxMergerXmlString* ) l_ptrSeq );
	}

	m_itemLayout = DocxMergerItemLayout::deserializeItemLayout( p_unzipFile );
} // deserialize

void DocxMergerItem::link( UnzipFile* p_unzipFile )
{
	list<DocxMergerItem*>::iterator					l_pathIterator;
	list<DocxMergerItemGroup*>::iterator			l_itemGroupIterator;
	multimap<string, DocxMergerField*>::iterator	l_fieldIterator;
	list<DocxMergerXmlString*>::iterator			l_xmlStringIterator;

	const map<uint32, void*>*	l_ptrsBySeq = p_unzipFile ->getPtrsBySeq();
	uint32						l_ptrSeq;

	l_ptrSeq			= ( uint32 ) m_itemFile;
	m_itemFile			= ( DocxMergerItemFile* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;

	l_ptrSeq			= ( uint32 ) m_itemGroup;
	m_itemGroup			= ( DocxMergerItemGroup* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;

	l_ptrSeq			= ( uint32 ) m_fillItemParent;
	m_fillItemParent	= ( DocxMergerItem* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;

	l_ptrSeq			= ( uint32 ) m_fillItemGroup;
	m_fillItemGroup		= ( DocxMergerItemGroup* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;

	FOR_EACH( l_pathIterator, &m_path )
	{
		l_ptrSeq					= ( uint32 ) *l_pathIterator;
		*l_pathIterator				= ( DocxMergerItem* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;
	}
	
	FOR_EACH( l_itemGroupIterator, &m_childItemGroups )
	{
		l_ptrSeq					= ( uint32 ) *l_itemGroupIterator;
		*l_itemGroupIterator		= ( DocxMergerItemGroup* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;
	}

	FOR_EACH( l_fieldIterator, &m_fieldsByName )
	{
		l_ptrSeq					= ( uint32 ) l_fieldIterator ->second;
		l_fieldIterator ->second	= ( DocxMergerField* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;
	}

	FOR_EACH( l_xmlStringIterator, &m_xmlStrings )
	{
		l_ptrSeq					= ( uint32 ) *l_xmlStringIterator;
		*l_xmlStringIterator		= ( DocxMergerXmlString* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;
	}

	if( m_itemLayout )
		m_itemLayout ->link( p_unzipFile );
} // link



DocxMergerItemFile* DocxMergerItem::getItemFile() const
{
	return m_itemFile;
} // getItemFile

DocxMergerItemGroup* DocxMergerItem::getItemGroup() const
{
	return m_itemGroup;
} // getItemGroup

string DocxMergerItem::getName() const
{
	return m_name;
} // getName

const list<DocxMergerItem*>* DocxMergerItem::getPath() const
{
	return &m_path;
} // getPath

const list<DocxMergerItemGroup*>* DocxMergerItem::getChildItemGroups() const
{
	return &m_childItemGroups;
} // getChildItemGroups

const multimap<string, DocxMergerField*>* DocxMergerItem::getFieldsByName() const
{
	return &m_fieldsByName;
} // getFieldsByName

const list<DocxMergerXmlString*>* DocxMergerItem::getXmlStrings() const
{
	return &m_xmlStrings;
} // getXmlStrings

bool DocxMergerItem::isTopLevel() const
{
	return m_topLevel;
} // isTopLevel

DocxMergerItem* DocxMergerItem::getFillItemParent() const
{
	return m_fillItemParent;
} // getFillItemParent

DocxMergerItemGroup* DocxMergerItem::getFillItemGroup() const
{
	return m_fillItemGroup;
} // getFillItemGroup

DocxMergerItemLayout* DocxMergerItem::getItemLayout() const
{
	return m_itemLayout;
} // getItemLayout

double DocxMergerItem::getPageSize() const
{
	return m_pageSize;
} // getPageSize

bool DocxMergerItem::isPageBottom() const
{
	return m_pageBottom;
} // isPageBottom

bool DocxMergerItem::isKeepTogether() const
{
	return m_keepTogether;
} // isKeepTogether

bool DocxMergerItem::isKeepWithPrev() const
{
	return m_keepWithPrev;
} // isKeepWithPrev
