
#include "DocxFactory/DocxCompiler/DocxCompilerFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerSettingsFile.h"

#include "DocxFactory/opc/OpcFunc.h"
#include "DocxFactory/opc/OpcPackage.h"
#include "DocxFactory/opc/OpcPart.h"
#include "DocxFactory/opc/OpcRelationships.h"
#include "DocxFactory/opc/OpcRelationship.h"
#include "DocxFactory/opc/OpcContentTypes.h"
#include "DocxFactory/opc/PartTypeNotFoundException.h"
#include "DocxFactory/opc/ContentTypeNotFoundException.h"

#include "DocxFactory/xml/XmlFunc.h"
#include "DocxFactory/xml/XmlTreeDriller.h"
#include "DocxFactory/xml/Utf8ToXMLCh.h"
#include "DocxFactory/xml/NodeNotFoundException.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include "xercesc/dom/DOM.hpp"

#include "boost/scoped_ptr.hpp"

#include <set>

using namespace DocxFactory;
using namespace std;



DocxCompilerSettingsFile::DocxCompilerSettingsFile( DocxCompilerFile* p_file )
{
	m_file			= p_file;
	m_part			= NULL;

	m_settingsNode	= NULL;
	m_wordMlPrefix	= "";
	m_relMlPrefix	= "";
} // c'tor

DocxCompilerSettingsFile::~DocxCompilerSettingsFile()
{

} // d'tor



void DocxCompilerSettingsFile::load()
{
	map<OpcPart*, string>::const_iterator	l_overrideIterator;
	const map<OpcPart*, string>*			l_overrides = m_file ->getContentTypes() ->getOverridesByPart();

	FOR_EACH( l_overrideIterator, l_overrides )
	{
		if ( l_overrideIterator ->second == "application/vnd.openxmlformats-officedocument.wordprocessingml.settings+xml" )
		{
			m_part = l_overrideIterator ->first;
			break;
		}
	}

	if ( l_overrideIterator == l_overrides ->end() )
		throw ContentTypeNotFoundException( "application/vnd.openxmlformats-officedocument.wordprocessingml.settings+xml", __FILE__, __LINE__ );

	m_part ->loadDoc();

	m_settingsNode	= m_part ->getDoc() ->getDocumentElement();
	m_wordMlPrefix	= XmlFunc::normalizeNsPrefix( m_settingsNode, "w", "http://schemas.openxmlformats.org/wordprocessingml/2006/main" );
	m_relMlPrefix	= XmlFunc::normalizeNsPrefix( m_settingsNode, "r", "http://schemas.openxmlformats.org/officeDocument/2006/relationships" );

	if ( !XmlFunc::XMLChCmp( m_settingsNode ->getLocalName(),		_X( "settings" ) )
	  || !XmlFunc::XMLChCmp( m_settingsNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )

		throw NodeNotFoundException( "settings", __FILE__, __LINE__ );

	normalize();
} // load

void DocxCompilerSettingsFile::normalize()
{
	xercesc::DOMDocument*	l_doc;
	xercesc::DOMNode*		l_node;

	m_part ->setChangeStatus( OpcPart::PART_CHANGED_IN_DOM );

	l_doc	= m_part ->getDoc();
	l_node	= XmlFunc::getChildByTagName(
		m_settingsNode,
		NULL,
		"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
		"rsids" );

	if ( l_node )
		m_settingsNode ->removeChild( l_node );
} // load



void DocxCompilerSettingsFile::serialize( ZipFile* p_zipFile )
{
	p_zipFile ->writePtr( m_file );
	p_zipFile ->writeStr( m_part ->getFullPath() );

	p_zipFile ->writeStr( m_wordMlPrefix );
	p_zipFile ->writeStr( m_relMlPrefix );
} // serialize



DocxCompilerFile* DocxCompilerSettingsFile::getFile() const
{
	return m_file;
} // getFile

OpcPart* DocxCompilerSettingsFile::getPart() const
{
	return m_part;
} // getPart

xercesc::DOMElement* DocxCompilerSettingsFile::getSettingsNode() const
{
	return m_settingsNode;
} // getStylesNode

string DocxCompilerSettingsFile::getWordMlPrefix() const
{
	return m_wordMlPrefix;
} // getWordMlPrefix

string DocxCompilerSettingsFile::getRelMlPrefix() const
{
	return m_relMlPrefix;
} // getRelMlPrefix
