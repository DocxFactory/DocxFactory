
#include "DocxFactory/DocxMerger/DocxMergerItemGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerItemLayout.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteItemGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteItem.h"

#include "DocxFactory/zip/UnzipFile.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

using namespace DocxFactory;
using namespace std;



DocxMergerItemLayout::DocxMergerItemLayout()
{

} // c'tor

DocxMergerItemLayout::~DocxMergerItemLayout()
{
	list<list<pair<unsigned short, DocxMergerItemLayout*>>>::iterator	l_trIterator;
	list<pair<unsigned short, DocxMergerItemLayout*>>::iterator			l_tcIterator;

	FOR_EACH( l_trIterator, &m_trList )
	{
		FOR_EACH( l_tcIterator, l_trIterator )
		{
			delete l_tcIterator ->second;
			l_tcIterator ->second = NULL;
		}
	}
}; // d'tor



DocxMergerItemLayout* DocxMergerItemLayout::deserializeItemLayout( UnzipFile* p_unzipFile )
{
	DocxMergerItemLayout* l_itemLayout;

	if ( p_unzipFile ->readNum<uint8>() == 0 )
		l_itemLayout = NULL;

	else
	{
		l_itemLayout = new DocxMergerItemLayout();
		l_itemLayout ->deserialize( p_unzipFile );
	}

	return l_itemLayout;
} // deserializeItemLayout

void DocxMergerItemLayout::deserialize( UnzipFile* p_unzipFile )
{
	list<pair<unsigned short, DocxMergerItemLayout*>>*	l_tr;
	pair<unsigned short, DocxMergerItemLayout*>*		l_tc;

	uint32	l_trSize;
	uint32	l_trPos;

	uint32	l_tcSize;
	uint32	l_tcPos;

	uint32	l_ptrSeq;
	uint32	l_size;
	uint32	i;

	m_fixedTrCnt	= p_unzipFile ->readNum<double>();
	l_trSize		= p_unzipFile ->readNum<uint32>();
	for ( l_trPos = 0; l_trPos < l_trSize; ++l_trPos )
	{
		m_trList.push_back( list<pair<unsigned short, DocxMergerItemLayout*>>() );
		l_tr = &m_trList.back();

		l_tcSize = p_unzipFile ->readNum<uint32>();
		for ( l_tcPos = 0; l_tcPos < l_tcSize; ++l_tcPos )
		{
			l_tr ->push_back( make_pair( 0, ( DocxMergerItemLayout* ) NULL ) );
			l_tc = &l_tr ->back();

			l_tc ->first	= p_unzipFile ->readNum<uint16>();
			l_tc ->second	= DocxMergerItemLayout::deserializeItemLayout( p_unzipFile );
		}
	}

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_ptrSeq = p_unzipFile ->readNum<uint32>();
		m_childItemGroups.push_back( ( DocxMergerItemGroup* ) l_ptrSeq );
	}
} // deserialize

void DocxMergerItemLayout::link( UnzipFile* p_unzipFile )
{
	list<list<pair<unsigned short, DocxMergerItemLayout*>>>::iterator	l_trIterator;
	list<pair<unsigned short, DocxMergerItemLayout*>>::iterator			l_tcIterator;
	list<DocxMergerItemGroup*>::iterator								l_itemGroupIterator;

	const map<uint32, void*>*	l_ptrsBySeq = p_unzipFile ->getPtrsBySeq();
	uint32						l_ptrSeq;

	FOR_EACH( l_trIterator, &m_trList )
	{
		FOR_EACH( l_tcIterator, l_trIterator )
		{
			l_tcIterator ->second ->link( p_unzipFile );
		}
	}

	FOR_EACH( l_itemGroupIterator, &m_childItemGroups )
	{
		l_ptrSeq				= ( uint32 ) *l_itemGroupIterator;
		*l_itemGroupIterator	= ( DocxMergerItemGroup* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;
	}
} // link



double DocxMergerItemLayout::calcTrCnt( DocxMergerPasteItem* p_pasteItem ) const
{
	list<list<pair<unsigned short, DocxMergerItemLayout*>>>::const_iterator	l_trIterator;
	list<pair<unsigned short, DocxMergerItemLayout*>>::const_iterator		l_tcIterator;
	list<DocxMergerItemGroup*>::const_iterator								l_itemGroupIterator;

	map<DocxMergerItemGroup*, DocxMergerPasteItemGroup*>::const_iterator	l_pasteItemGroupIterator;
	const map<DocxMergerItemGroup*, DocxMergerPasteItemGroup*>*				l_pasteItemGroups;

	list<double>::reverse_iterator	l_rowIterator;
	list<double>					l_rowList;
	double							l_rowTrCnt;
	double							l_colTrCnt;
	double							l_trCnt;

	l_trCnt = m_fixedTrCnt;
	FOR_EACH( l_trIterator, &m_trList )
	{
		l_rowTrCnt = 1.0f;

		FOR_EACH( l_tcIterator, l_trIterator )
		{
			if( l_tcIterator ->second )
				l_colTrCnt = l_tcIterator ->second ->calcTrCnt( p_pasteItem );

			if( l_tcIterator ->first > 0 )
			{
				l_rowIterator = l_rowList.rbegin();

				for ( unsigned short i = 0; i < l_tcIterator ->first; i++ )
				{
					if( l_rowIterator == l_rowList.rend() )
						break;

					if( l_colTrCnt >= *l_rowIterator )
						l_colTrCnt -= *l_rowIterator;

					else
					{
						l_colTrCnt = 0.0f;
						break;
					}

					l_rowIterator++;
				}
			}

			if( l_rowTrCnt < l_colTrCnt )
				l_rowTrCnt = l_colTrCnt;
		}

		l_trCnt += l_rowTrCnt;
		l_rowList.push_back( l_rowTrCnt );
	}

	l_pasteItemGroups = p_pasteItem ->getChildPasteItemGroupsByGroup();
	FOR_EACH( l_itemGroupIterator, &m_childItemGroups )
	{
		l_pasteItemGroupIterator = l_pasteItemGroups ->find( *l_itemGroupIterator );
		if ( l_pasteItemGroupIterator != l_pasteItemGroups ->end() )

			l_trCnt += l_pasteItemGroupIterator ->second ->getTrCnt();
	}

	return l_trCnt;
} // calcTrCnt
