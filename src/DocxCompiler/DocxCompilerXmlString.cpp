
#include "DocxFactory/DocxCompiler/DocxCompilerItemFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItem.h"
#include "DocxFactory/DocxCompiler/DocxCompilerId.h"
#include "DocxFactory/DocxCompiler/DocxCompilerXmlString.h"

#include "DocxFactory/zip/ZipFile.h"

using namespace DocxFactory;
using namespace std;



DocxCompilerXmlString::DocxCompilerXmlString(
	DocxCompilerItem*	p_item,
	XmlStringType		p_type, 
	const string&		p_str,
	void*				p_ptr )
{
	m_itemFile	= ( p_item ? p_item ->getItemFile() : NULL );
	m_item		= p_item;
	m_type		= p_type;
	m_ptr		= p_ptr;
	m_str		= p_str;
} // c'tor

DocxCompilerXmlString::~DocxCompilerXmlString()
{

} // d'tor



void DocxCompilerXmlString::serialize( ZipFile* p_zipFile )
{
	p_zipFile ->writePtr		( this );
	p_zipFile ->writePtr		( m_itemFile );
	p_zipFile ->writePtr		( m_item );
	p_zipFile ->writeNum<int16>	( m_type );
	p_zipFile ->writeStr		( m_str );
	p_zipFile ->writePtr		( m_ptr );
} // serialize



void DocxCompilerXmlString::concatStr( const string& p_str )
{
	m_str += p_str;
} // concatStr



DocxCompilerItemFile* DocxCompilerXmlString::getItemFile() const
{
	return m_itemFile;
} // getItemFile

DocxCompilerItem* DocxCompilerXmlString::getItem() const
{
	return m_item;
} // getItem

DocxCompilerXmlString::XmlStringType DocxCompilerXmlString::getType() const
{
	return m_type;
} // getType

const string* DocxCompilerXmlString::getStr() const
{
	return &m_str;
} // getStr

void* DocxCompilerXmlString::getPtr() const
{
	return m_ptr;
} // getPtr
