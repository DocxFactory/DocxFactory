
#include "DocxFactory/DocxMerger/DocxMergerFile.h"
#include "DocxFactory/DocxMerger/DocxMergerItemFile.h"
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerId.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/UnzipFile.h"

#include "DocxFactory/str/StrFunc.h"

using namespace DocxFactory;
using namespace std;



DocxMergerId::DocxMergerId()
{

} // c'tor

DocxMergerId::~DocxMergerId()
{

} // d'tor



void DocxMergerId::deserialize( UnzipFile* p_unzipFile )
{
	p_unzipFile ->insertPtrBySeq( p_unzipFile ->readNum<uint32>(), this );

	m_item		= ( DocxMergerItem* ) p_unzipFile ->readNum<uint32>();
	m_tag		= p_unzipFile ->readStr();
	m_id		= p_unzipFile ->readStr();
	m_prefix	= p_unzipFile ->readStr();
	m_seq		= p_unzipFile ->readNum<int32>();
	m_isNum		= p_unzipFile ->readNum<uint8>() != 0;
} // deserialize

void DocxMergerId::link( UnzipFile* p_unzipFile )
{
	const map<uint32, void*>*	l_ptrsBySeq = p_unzipFile ->getPtrsBySeq();
	uint32						l_ptrSeq;

	l_ptrSeq	= ( uint32 ) m_item;
	m_item		= ( DocxMergerItem* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;
	m_file		= m_item ->getItemFile() ->getFile();
} // link



string DocxMergerId::getNextId()
{
	m_seq = m_file ->nextIdSeq();

	return getCurrId();
} // getNextId

string DocxMergerId::getCurrId()
{
	if ( m_isNum )
		return StrFunc::intToStr( m_seq );
	else
		return m_prefix + StrFunc::intToStr( m_seq );
} // getCurrId