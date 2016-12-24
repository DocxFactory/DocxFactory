
#include "DocxFactory/DocxCompiler/DocxCompilerItemFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItem.h"
#include "DocxFactory/DocxCompiler/DocxCompilerId.h"

#include "DocxFactory/zip/ZipFile.h"

#include "DocxFactory/str/StrFunc.h"

using namespace DocxFactory;
using namespace std;



DocxCompilerId::DocxCompilerId(
	DocxCompilerItem*	p_item,
	const string&		p_tag,
	const string&		p_id )
{
	m_item	= p_item;
	m_tag	= p_tag;
	m_id	= p_id;

	if ( p_tag == "docPr" )
		m_isNum = true;
	else
		m_isNum = StrFunc::isNum( p_id );

	if ( m_isNum )
	{
		m_prefix	= "";
		m_seq		= StrFunc::strToInt<int>( m_id ) - 1;
	}

	else
	{
		m_prefix	= "dfw_" + m_tag + "_";
		m_seq		= 0;
	}
} // c'tor

DocxCompilerId::~DocxCompilerId()
{

} // d'tor



void DocxCompilerId::serialize( ZipFile* p_zipFile )
{
	p_zipFile ->writePtr		( this );
	p_zipFile ->writePtr		( m_item );
	p_zipFile ->writeStr		( m_tag );
	p_zipFile ->writeStr		( m_id );
	p_zipFile ->writeStr		( m_prefix );
	p_zipFile ->writeNum<int32>	( m_seq );
	p_zipFile ->writeNum<uint8>	( m_isNum );
} // serialize



string DocxCompilerId::getTag() const
{
	return m_tag;
} // getTag

string DocxCompilerId::getId() const
{
	return m_id;
} // getId