
#include "DocxFactory/DocxMerger/DocxMergerFile.h"
#include "DocxFactory/DocxMerger/DocxMergerSettingsFile.h"

#include "DocxFactory/opc/OpcPart.h"
#include "DocxFactory/opc/OpcFunc.h"

#include "DocxFactory/xml/XmlFunc.h"
#include "DocxFactory/xml/Utf8ToXMLCh.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/UnzipFile.h"

#include "xercesc/dom/DOM.hpp"

using namespace DocxFactory;
using namespace std;




DocxMergerSettingsFile::DocxMergerSettingsFile()
{

} // c'tor

DocxMergerSettingsFile::~DocxMergerSettingsFile()
{

} // d'tor



void DocxMergerSettingsFile::setUpdateFields( bool p_val )
{
	xercesc::DOMElement* l_node;

	m_part ->setChangeStatus( OpcPart::PART_CHANGED_IN_DOM );

	l_node = XmlFunc::getChildByTagName(
		m_settingsNode,
		NULL,
		"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
		"updateFields" );

	if(!l_node )
	{
		l_node = m_part ->getDoc() ->createElementNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X( "updateFields" ) ) ;
		l_node ->setPrefix( Utf8ToXMLCh( m_wordMlPrefix ) );

		m_settingsNode ->appendChild( l_node );
	}

	l_node ->setAttribute(
		Utf8ToXMLCh( m_wordMlPrefix + ":val" ),
		Utf8ToXMLCh( p_val ? "true" : "false" ) );
} // setUpdateFields



void DocxMergerSettingsFile::deserialize( UnzipFile* p_unzipFile )
{
	m_file			= ( DocxMergerFile* )	p_unzipFile ->readNum<uint32>();
	m_part			= ( OpcPart* )			new string( p_unzipFile ->readStr() );

	m_wordMlPrefix	= p_unzipFile ->readStr();
	m_relMlPrefix	= p_unzipFile ->readStr();
} // deserialize

void DocxMergerSettingsFile::link( UnzipFile* p_unzipFile )
{
	const map<uint32, void*>*	l_ptrsBySeq = p_unzipFile ->getPtrsBySeq();
	uint32						l_ptrSeq;
	string*						l_str;

	l_ptrSeq		= ( uint32 ) m_file;
	m_file			= ( DocxMergerFile* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;

	l_str			= ( string* )	m_part;
	m_part			= ( OpcPart* )	m_file ->getPartsByFullPath() ->find( *l_str ) ->second;
	m_part ->loadDoc();
	delete l_str;

	m_settingsNode	= m_part ->getDoc() ->getDocumentElement();
} // link



DocxMergerFile* DocxMergerSettingsFile::getFile() const
{
	return m_file;
} // getFile

OpcPart* DocxMergerSettingsFile::getPart() const
{
	return m_part;
} // getPart

xercesc::DOMElement* DocxMergerSettingsFile::getSettingsNode() const
{
	return m_settingsNode;
} // getStylesNode

string DocxMergerSettingsFile::getWordMlPrefix() const
{
	return m_wordMlPrefix;
} // getWordMlPrefix

string DocxMergerSettingsFile::getRelMlPrefix() const
{
	return m_relMlPrefix;
} // getRelMlPrefix
