
#include "DocxFactory/DocxMerger/DocxMergerFile.h"
#include "DocxFactory/DocxMerger/DocxMergerItemFile.h"
#include "DocxFactory/DocxMerger/DocxMergerItemGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerXmlString.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteItemGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteItem.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteFieldGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"

#include "DocxFactory/opc/OpcPart.h"
#include "DocxFactory/opc/OpcFunc.h"

#include "DocxFactory/xml/XmlFunc.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/UnzipFile.h"
#include "DocxFactory/os/OsFunc.h"

#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/math/MathFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include "xercesc/dom/DOM.hpp"

using namespace DocxFactory;
using namespace std;



DocxMergerItemFile::DocxMergerItemFile()
{
	m_topLevelItem			= NULL;
	m_topLevelPasteItem		= NULL;
	m_fillItemParent		= NULL;
	m_fillPasteItemParent	= NULL;
	m_keepTogetherItem		= NULL;
	m_keepTogetherPasteItem	= NULL;
	m_pasteFieldGroup		= NULL;

	m_rootPasteItemGroup	= new DocxMergerPasteItemGroup( m_rootItemGroup, NULL );
	m_pasteFieldGroup		= new DocxMergerPasteFieldGroup();
} // c'tor

DocxMergerItemFile::~DocxMergerItemFile()
{
	list<DocxMergerXmlString*>::iterator l_xmlStringIterator;
	FOR_EACH( l_xmlStringIterator, &m_sects )
	{
		delete *l_xmlStringIterator;
	}

	delete m_rootItemGroup;
	delete m_rootPasteItemGroup;
	delete m_pasteFieldGroup;
} // d'tor



void DocxMergerItemFile::save( ZipFile* p_zipFile )
{
	DocxMergerItem*	l_item;
	size_t			l_len;
	size_t			l_pos;

	if ( m_rootPasteItemGroup ->getChildPasteItems() ->empty() )
	{
		l_item = m_rootItemGroup ->getChildItemsByName() ->begin() ->second;
		paste( l_item );
	}

	l_len = m_itemPath.size();
	for ( l_pos = 0; l_pos < l_len; ++l_pos )
	{
		closeItem();
	}



	m_zipFile	= p_zipFile;
	m_firstPage	= true;
	m_sect		= NULL;

	m_openTc.clear();
	m_shade.clear();

	m_zipFile ->openStream(
		OpcFunc::opcToZipPath( m_part ->getFullPath() ),
		Z_DEFLATED,
		Z_BEST_SPEED );

	( *m_zipFile ) << m_body1;

	if( m_rootPasteItemGroup )
		m_rootItemGroup ->save( m_rootPasteItemGroup );

	if ( m_type == DOCUMENT )
	{
		if(!m_sect )
			m_sect = m_sects.front();

		( *m_zipFile ) << *m_sect ->getStr();
	}

	( *m_zipFile ) << m_body2;

	m_zipFile ->closeStream();
	m_part ->setChangeStatus( OpcPart::PART_SAVED );
} // save



void DocxMergerItemFile::paste( DocxMergerItem* p_item )
{
	list<DocxMergerItem*>::iterator			l_itemFilePathIterator;
	list<DocxMergerItem*>::const_iterator	l_itemPathIterator;
	const list<DocxMergerItem*>*			l_itemPath;

	DocxMergerItem*							l_keepWithParentItem;
	DocxMergerPasteItem*					l_keepWithParentPasteItem;

	size_t									l_len;
	size_t									l_pos;

	l_itemPath					= p_item ->getPath();
	l_itemPathIterator			= l_itemPath ->begin();
	l_itemFilePathIterator		= m_itemPath.begin();

	l_len = min( l_itemPath ->size() - 1, m_itemPath.size() );
	for ( l_pos = 0; l_pos < l_len && *l_itemPathIterator == *l_itemFilePathIterator; ++l_pos )
	{
		++l_itemPathIterator;
		++l_itemFilePathIterator;
	}

	l_len = m_itemPath.size();
	for ( ; l_pos < l_len; ++l_pos )
	{
		closeItem();
	}

	l_keepWithParentItem		= NULL;
	l_keepWithParentPasteItem	= NULL;

	for ( ; l_itemPathIterator != l_itemPath ->end(); ++l_itemPathIterator )
	{
		pasteItem(
			*l_itemPathIterator,
			m_pasteFieldGroup,
			l_keepWithParentItem,
			l_keepWithParentPasteItem );

		if(!l_keepWithParentItem )
		{
			l_keepWithParentItem		= m_itemPath.back();
			l_keepWithParentPasteItem	= m_pasteItemPath.back();
		}
	}

	m_file ->insertPasteFieldGroup( m_pasteFieldGroup );
	m_pasteFieldGroup = new DocxMergerPasteFieldGroup();
} // paste

void DocxMergerItemFile::pasteItem(
	DocxMergerItem*				p_item,
	DocxMergerPasteFieldGroup*	p_pasteFieldGroup,
	DocxMergerItem*				p_keepWithParentItem,
	DocxMergerPasteItem*		p_keepWithParentPasteItem )
{
	map<DocxMergerItemGroup*, DocxMergerPasteItemGroup*>::const_iterator	l_pasteItemGroupIterator;
	const map<DocxMergerItemGroup*, DocxMergerPasteItemGroup*>*				l_pasteItemGroups;
	DocxMergerPasteItemGroup*												l_pasteItemGroup;

	list<DocxMergerItem*>::iterator			l_itemPathIterator;
	list<DocxMergerPasteItem*>::iterator	l_pasteItemPathIterator;

	DocxMergerItem*							l_oldItem;
	DocxMergerPasteItem*					l_oldPasteItem;
	list<DocxMergerItem*>					l_oldItemPath;
	list<DocxMergerPasteItem*>				l_oldPasteItemPath;
	DocxMergerPasteItem*					l_oldParentPasteItem;
	DocxMergerPasteItemGroup*				l_oldPasteItemGroup;

	DocxMergerItemGroup*					l_itemGroup;
	DocxMergerItem*							l_item;
	DocxMergerItem*							l_parentItem;
	DocxMergerPasteItem*					l_pasteItem;
	DocxMergerPasteItem*					l_parentPasteItem;

	size_t									l_len;
	size_t									l_pos;

	l_itemGroup	= p_item ->getItemGroup();

	if ( p_item ->isTopLevel() )
	{
		l_parentItem				= m_itemPath.back();
		l_parentPasteItem			= m_pasteItemPath.back();

		l_pasteItemGroups			= l_parentPasteItem ->getChildPasteItemGroupsByGroup();
		l_pasteItemGroupIterator	= l_pasteItemGroups ->find( l_itemGroup );

		if( l_pasteItemGroupIterator != l_pasteItemGroups ->end() )
		{
			l_pasteItemGroup = l_pasteItemGroupIterator ->second;
			if ( l_pasteItemGroup ->getChildPasteItems() ->size() > 0 )
			{
				closeItem();
				pasteItem(
					l_parentItem,
					l_parentPasteItem ->getPasteFieldGroup(),
					NULL,
					NULL );
			}
		}

		if( p_item ->getFillItemParent() )
		{
			m_fillItemParent		= p_item ->getFillItemParent();
			m_fillPasteItemParent	= NULL;
		}
	}

	else
	if ( !p_item ->isPageBottom()
		
	  && m_topLevelItem
	  && m_topLevelItem ->getPageSize() > 0 )
	{
		l_parentItem				= m_itemPath.back();
		l_parentPasteItem			= m_pasteItemPath.back();

		l_pasteItemGroups			= l_parentPasteItem ->getChildPasteItemGroupsByGroup();
		l_pasteItemGroupIterator	= l_pasteItemGroups ->find( l_itemGroup );

		if ( l_pasteItemGroupIterator != l_pasteItemGroups ->end() // also means there is atleast one item in the group
		  && l_pasteItemGroupIterator ->second ->getChildPasteItems() ->back() ->getItem() ->isPageBottom() )
		{
			l_oldItemPath.clear();
			l_oldPasteItemPath.clear();

			l_itemPathIterator		= m_itemPath.begin();
			l_pasteItemPathIterator	= m_pasteItemPath.begin();

			l_len = p_item ->getPath() ->size() - 1;
			for ( l_pos = 0; l_pos < l_len; l_pos++ )
			{
				l_oldItemPath.push_back( *l_itemPathIterator );
				l_oldPasteItemPath.push_back( *l_pasteItemPathIterator );

				l_itemPathIterator++;
				l_pasteItemPathIterator++;
			}

			l_len = m_itemPath.size();
			for ( l_pos = 0; l_pos < l_len; l_pos++ )
			{
				closeItem();
			}

			l_itemPathIterator		= l_oldItemPath.begin();
			l_pasteItemPathIterator	= l_oldPasteItemPath.begin();

			l_len = l_oldItemPath.size();
			for ( l_pos = 0;l_pos < l_len; l_pos++ )
			{
				pasteItem(
					*l_itemPathIterator,
				  ( *l_pasteItemPathIterator ) ->getPasteFieldGroup(),
					NULL,
					NULL );

				l_itemPathIterator++;
				l_pasteItemPathIterator++;
			}
		}
	}



	l_pasteItem = new DocxMergerPasteItem( p_item, p_pasteFieldGroup );

	if(!m_pasteItemPath.empty() )
		m_pasteItemPath.back() ->pushBackChildPasteItem( l_pasteItem );

	else
		m_rootPasteItemGroup ->pushBackChildPasteItem( l_pasteItem );

	m_itemPath.push_back( p_item );
	m_pasteItemPath.push_back( l_pasteItem );



	if( p_item ->isTopLevel() )
	{
		if ( p_item ->getPageSize() > 0 )
		{
			m_topLevelItem			= p_item;
			m_topLevelPasteItem		= l_pasteItem;
		}
	}

	else
	if( p_item ->isKeepTogether() )
	{
		if ( m_topLevelItem
		 && !m_keepTogetherItem )
		{
			m_keepTogetherItem		= p_item;
			m_keepTogetherPasteItem	= l_pasteItem;
		}
	}

	if( m_fillItemParent && p_item == m_fillItemParent )
		m_fillPasteItemParent = l_pasteItem;

	if( m_topLevelPasteItem
	 && MathFunc::cmpGt( m_topLevelPasteItem ->getTrCnt(), m_topLevelItem ->getPageSize() ) )
	{
		l_oldItem		= p_item;
		l_oldPasteItem	= l_pasteItem;

		l_item = p_item;
		if ( m_keepTogetherItem )
		{
			l_item		= m_keepTogetherItem;
			l_pasteItem	= m_keepTogetherPasteItem;
		}

		if ( p_keepWithParentItem
		  && p_keepWithParentItem ->getPath() ->size() < l_item ->getPath() ->size() )
		{
			l_item		= p_keepWithParentItem;
			l_pasteItem	= p_keepWithParentPasteItem;
		}



		l_oldItemPath.clear();
		l_oldPasteItemPath.clear();

		l_itemPathIterator		= m_itemPath.begin();
		l_pasteItemPathIterator	= m_pasteItemPath.begin();

		l_len = l_item ->getPath() ->size() - 1;
		for ( l_pos = 0; l_pos < l_len; l_pos++ )
		{
			l_oldItemPath.push_back( *l_itemPathIterator );
			l_oldPasteItemPath.push_back( *l_pasteItemPathIterator );

			l_itemPathIterator++;
			l_pasteItemPathIterator++;
		}

		l_len = m_itemPath.size();
		for ( l_pos = 0; l_pos < l_len; l_pos++ )
		{
			closeItem();
		}

		l_itemPathIterator		= l_oldItemPath.begin();
		l_pasteItemPathIterator	= l_oldPasteItemPath.begin();

		l_len = l_oldItemPath.size();
		for ( l_pos = 0;l_pos < l_len; l_pos++ )
		{
			pasteItem(
				*l_itemPathIterator,
			  ( *l_pasteItemPathIterator ) ->getPasteFieldGroup(),
				NULL,
				NULL );

			l_itemPathIterator++;
			l_pasteItemPathIterator++;
		}



		l_oldPasteItemGroup		= l_pasteItem ->getPasteItemGroup();
		l_oldParentPasteItem	= l_oldPasteItemGroup ->getPasteItem();

		l_parentPasteItem		= m_pasteItemPath.back();

		l_oldParentPasteItem	->popBackChildPasteItem( l_oldPasteItemGroup );
		l_parentPasteItem		->pushFrontChildPasteItem( l_pasteItem );

		l_pasteItemGroup		= l_pasteItem ->getPasteItemGroup();

		while ( l_item ->isKeepWithPrev() && l_oldPasteItemGroup ->getChildPasteItems() ->size() > 1 )
		{
			l_pasteItem				= l_oldPasteItemGroup ->getChildPasteItems() ->back();
			l_item					= l_pasteItem ->getItem();

			l_oldParentPasteItem	->popBackChildPasteItem( l_oldPasteItemGroup );
			l_parentPasteItem		->pushFrontChildPasteItem( l_pasteItem );

			if ( MathFunc::cmpGe( m_topLevelPasteItem ->getTrCnt(), m_topLevelItem ->getPageSize() ) )
			{
				if ( MathFunc::cmpGt( m_topLevelPasteItem ->getTrCnt(), m_topLevelItem ->getPageSize() ) )
				{
					l_parentPasteItem		->popFrontChildPasteItem( l_pasteItemGroup );
					l_oldParentPasteItem	->pushBackChildPasteItem( l_pasteItem );
				}

				break;
			}
		}



		m_itemPath.clear();
		m_pasteItemPath.clear();

		l_len = l_oldItem ->getPath() ->size();
		for ( l_pos = 0; l_pos < l_len; l_pos++ )
		{
			m_itemPath.push_front( l_oldItem );
			m_pasteItemPath.push_front( l_oldPasteItem );

			l_oldItem		= l_oldItem ->getItemGroup() ->getItem();
			l_oldPasteItem	= l_oldPasteItem ->getPasteItemGroup() ->getPasteItem();
		}
	}
} // pasteItem

void DocxMergerItemFile::closeItem()
{
	if( m_topLevelItem
	 && m_topLevelItem == m_itemPath.back() )
	{
		if( m_fillItemParent )
			m_topLevelPasteItem ->setFillPasteItemParent( m_fillPasteItemParent );

		m_topLevelItem			= NULL;
		m_topLevelPasteItem		= NULL;
		m_fillItemParent		= NULL;
		m_fillPasteItemParent	= NULL;
	}

	else
	if( m_keepTogetherItem
	 && m_keepTogetherItem == m_itemPath.back() )
	{
		m_keepTogetherItem		= NULL;
		m_keepTogetherPasteItem	= NULL;
	}

	m_itemPath.pop_back();
	m_pasteItemPath.pop_back();
} // closeItem



void DocxMergerItemFile::deserialize( UnzipFile* p_unzipFile )
{
	uint32	l_ptrSeq;
	uint32	l_size;
	uint32	i;

	p_unzipFile ->insertPtrBySeq( p_unzipFile ->readNum<uint32>(), this );

	m_file			= ( DocxMergerFile* )	p_unzipFile ->readNum<uint32>();
	m_part			= ( OpcPart* )			new string( p_unzipFile ->readStr() );
	m_type			= ( ItemFileType )		p_unzipFile ->readNum<int16>();

	m_body1			= p_unzipFile ->readStr();
	m_body2			= p_unzipFile ->readStr();
	m_wordMlPrefix	= p_unzipFile ->readStr();
	m_relMlPrefix	= p_unzipFile ->readStr();
	m_vmlPrefix		= p_unzipFile ->readStr();

	m_rootItemGroup	= ( DocxMergerItemGroup* ) p_unzipFile ->readNum<uint32>();

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_ptrSeq	= p_unzipFile ->readNum<uint32>();
		m_itemGroups.push_back( ( DocxMergerItemGroup*) l_ptrSeq );
	}

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_ptrSeq	= p_unzipFile ->readNum<uint32>();
		m_items.push_back( ( DocxMergerItem* ) l_ptrSeq );
	}

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_ptrSeq	= p_unzipFile ->readNum<uint32>();
		m_fields.push_back( ( DocxMergerField* ) l_ptrSeq );
	}

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_ptrSeq	= p_unzipFile ->readNum<uint32>();
		m_sects.push_back( ( DocxMergerXmlString* ) l_ptrSeq );
	}
} // deserialize

void DocxMergerItemFile::link( UnzipFile* p_unzipFile )
{
	list<DocxMergerItemGroup*>::iterator	l_itemGroupIterator;
	list<DocxMergerItem*>::iterator			l_itemIterator;
	list<DocxMergerField*>::iterator		l_fieldIterator;
	list<DocxMergerXmlString*>::iterator	l_xmlStringIterator;

	const map<uint32, void*>*	l_ptrsBySeq = p_unzipFile ->getPtrsBySeq();
	uint32						l_ptrSeq;
	string*						l_str;

	l_ptrSeq		= ( uint32 ) m_file;
	m_file			= ( DocxMergerFile* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;

	l_str			= ( string* )	m_part;
	m_part			= ( OpcPart* )	m_file ->getPartsByFullPath() ->find( *l_str ) ->second;
	m_part ->loadDoc();
	delete l_str;

	l_ptrSeq		= ( uint32 ) m_rootItemGroup;
	m_rootItemGroup	= ( DocxMergerItemGroup* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;

	FOR_EACH( l_itemGroupIterator, &m_itemGroups )
	{
		l_ptrSeq				= ( uint32 ) *l_itemGroupIterator;
		*l_itemGroupIterator	= ( DocxMergerItemGroup* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;
	}

	FOR_EACH( l_itemIterator, &m_items )
	{
		l_ptrSeq				= ( uint32 ) *l_itemIterator;
		*l_itemIterator			= ( DocxMergerItem* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;
	}

	FOR_EACH( l_fieldIterator, &m_fields )
	{
		l_ptrSeq				= ( uint32 ) *l_fieldIterator;
		*l_fieldIterator		= ( DocxMergerField* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;
	}

	FOR_EACH( l_xmlStringIterator, &m_sects )
	{
		l_ptrSeq				= ( uint32 ) *l_xmlStringIterator;
		*l_xmlStringIterator	= ( DocxMergerXmlString* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;
	}
} // link



void DocxMergerItemFile::pushBackOpenTc( bool p_openTc )
{
	m_openTc.push_back( p_openTc );
} // pushBackOpenTc

void DocxMergerItemFile::popBackOpenTc()
{
	m_openTc.pop_back();
} // popBackOpenTc

void DocxMergerItemFile::pushBackShade( const string& p_shade )
{
	m_shade.push_back( p_shade );
} // pushBackShade

void DocxMergerItemFile::popBackShade()
{
	m_shade.pop_back();
} // popBackShade

void DocxMergerItemFile::setOpenTcBack( bool p_openTc )
{
	if( m_openTc.size() > 0 )
		m_openTc.back() = p_openTc;
} // setOpenTcBack

void DocxMergerItemFile::setShadeBack( const string& p_shade )
{
	if( m_shade.size() > 0 )
		m_shade.back() = p_shade;
} // setShadeBack

void DocxMergerItemFile::setFirstPage( bool p_firstPage )
{
	m_firstPage = p_firstPage;
} // setFirstPage

void DocxMergerItemFile::setSect( DocxMergerXmlString* p_sect )
{
	m_sect = p_sect;
} // setSect



DocxMergerFile* DocxMergerItemFile::getFile() const
{
	return m_file;
} // getFile

DocxMergerItemGroup* DocxMergerItemFile::getRootItemGroup() const
{
	return m_rootItemGroup;
} // getRootItemGroup

OpcPart* DocxMergerItemFile::getPart() const
{
	return m_part;
} // getPart

DocxMergerItemFile::ItemFileType DocxMergerItemFile::getType() const
{
	return m_type;
} // getType

string DocxMergerItemFile::getWordMlPrefix() const
{
	return m_wordMlPrefix;
} // getWordMlPrefix

string DocxMergerItemFile::getRelMlPrefix() const
{
	return m_relMlPrefix;
} // getRelMlPrefix

string DocxMergerItemFile::getVmlPrefix() const
{
	return m_vmlPrefix;
} // getVmlPrefix

DocxMergerPasteFieldGroup* DocxMergerItemFile::getPasteFieldGroup() const
{
	return m_pasteFieldGroup;
} // getPasteFieldGroup

ZipFile* DocxMergerItemFile::getZipFile() const
{
	return m_zipFile;
} // getZipFile

bool DocxMergerItemFile::getOpenTcBack() const
{
	return m_openTc.back();
} // getOpenTcBack

string DocxMergerItemFile::getShadeBack() const
{
	return m_shade.back();
} // getShadeBack

bool DocxMergerItemFile::getFirstPage() const
{
	return m_firstPage;
} // getFirstPage

DocxMergerXmlString* DocxMergerItemFile::getSect() const
{
	return m_sect;
} // getSect