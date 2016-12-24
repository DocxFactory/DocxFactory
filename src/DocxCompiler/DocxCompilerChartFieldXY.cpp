
#include "DocxFactory/DocxCompiler/DocxCompilerItemFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItem.h"
#include "DocxFactory/DocxCompiler/DocxCompilerField.h"
#include "DocxFactory/DocxCompiler/DocxCompilerChartField.h"
#include "DocxFactory/DocxCompiler/DocxCompilerChartFieldXY.h"

#include "DocxFactory/opc/OpcPart.h"

#include "DocxFactory/xml/XmlFunc.h"
#include "DocxFactory/xml/XmlTreeDriller.h"
#include "DocxFactory/xml/Utf8ToXMLCh.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include "xercesc/dom/DOM.hpp"

#include "boost/scoped_ptr.hpp"

using namespace DocxFactory;
using namespace std;



DocxCompilerChartFieldXY::DocxCompilerChartFieldXY(
	DocxCompilerItem*		p_item,
	const string&			p_name,
	xercesc::DOMElement*	p_placeHolderNode,
	xercesc::DOMElement*	p_drawingNode,
	OpcPart*				p_chartPart )

	: DocxCompilerChartField(
		p_item,
		p_name,
		CHART_X_Y,
		p_placeHolderNode,
		p_drawingNode,
		p_chartPart )
{
	boost::scoped_ptr<XmlTreeDriller>	l_treeDriller;

	xercesc::DOMDocument*	l_doc			= p_chartPart ->getDoc();
	xercesc::DOMElement*	l_rootNode		= l_doc ->getDocumentElement();
	xercesc::DOMNode*		l_node;

	xercesc::DOMNode*		l_cursorNode	= l_rootNode;
	XmlFunc::TagPos			l_cursorPos		= XmlFunc::TAG_BEFORE;
	string					l_str;
	string					l_chartStrings	= "";
	string					l_format;

	while ( XmlFunc::getNextTag( l_rootNode, l_cursorNode, l_cursorPos, l_str ) )
	{
		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
		  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "tx" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/drawingml/2006/chart" ) )
		  && ( l_cursorPos == XmlFunc::TAG_EMPTY || l_cursorPos == XmlFunc::TAG_START ) )
		{
			l_chartStrings	+=
				  "<" + XmlFunc::XMLChToUtf8( l_cursorNode ->getNodeName() ) + ">"
				+ "<" + m_chartMlPrefix + ":v>";

			l_treeDriller.reset( new XmlTreeDriller(
				l_cursorNode,
				"", "",
				"http://schemas.openxmlformats.org/drawingml/2006/chart", "v",
				"", "", "" ) );

			l_node = l_treeDriller ->nextNode();
			if( l_node )
			{
				l_node = l_node ->getFirstChild();
				if ( l_node ->getNodeType() == xercesc::DOMNode::TEXT_NODE )
				{
					l_chartStrings += XmlFunc::XMLChToUtf8( l_node ->getNodeValue() );
				}
			}

			l_chartStrings +=
				  "</" + m_chartMlPrefix + ":v>"
				+ "</" + XmlFunc::XMLChToUtf8( l_cursorNode ->getNodeName() ) + ">";

			l_cursorPos	= XmlFunc::TAG_END;
		}

		else
		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
		&& ( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "xVal" ) )
		  || XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "yVal" ) ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/drawingml/2006/chart" ) )
		  && ( l_cursorPos == XmlFunc::TAG_EMPTY || l_cursorPos == XmlFunc::TAG_START ) )
		{
			l_chartStrings +=
				  "<" + XmlFunc::XMLChToUtf8( l_cursorNode ->getNodeName() ) + ">"
				+ "<" + m_chartMlPrefix + ":numLit>";

			l_treeDriller.reset( new XmlTreeDriller(
				l_cursorNode,
				"", "",
				"http://schemas.openxmlformats.org/drawingml/2006/chart", "formatCode",
				"", "", "" ) );

			l_node = l_treeDriller ->nextNode();
			if( l_node )
			{
				l_node = l_node ->getFirstChild();
				if ( l_node ->getNodeType() == xercesc::DOMNode::TEXT_NODE )
				{
					l_format = XmlFunc::XMLChToUtf8( l_node ->getNodeValue() );

					l_chartStrings +=
						  "<"	+ m_chartMlPrefix + ":formatCode>"
						+ l_format
						+ "</"	+ m_chartMlPrefix + ":formatCode>";
				}
			}

			m_chartStrings.push_back( make_pair(
				l_chartStrings, 
				( XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(), _X( "xVal" ) ) ? 'x' : 'y' ) ) );

			l_chartStrings =
				  "</" + m_chartMlPrefix + ":numLit>"
				+ "</" + XmlFunc::XMLChToUtf8( l_cursorNode ->getNodeName() ) + ">";

			l_cursorPos	= XmlFunc::TAG_END;
		}

		else
		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
		  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "externalData" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/drawingml/2006/chart" ) )
		  && ( l_cursorPos == XmlFunc::TAG_EMPTY || l_cursorPos == XmlFunc::TAG_START ) )
		{
			l_cursorPos = XmlFunc::TAG_END;
		}

		else
			l_chartStrings += l_str;
	}

	if ( !l_chartStrings.empty() )
		  m_chartStrings.push_back( make_pair( l_chartStrings, NULL ) );

} // c'tor

DocxCompilerChartFieldXY::~DocxCompilerChartFieldXY()
{

} // d'tor



void DocxCompilerChartFieldXY::serialize( ZipFile* p_zipFile )
{
	list<pair<string, char>>::iterator l_stringIterator;

	DocxCompilerChartField::serialize( p_zipFile );

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_chartStrings.size() );
	FOR_EACH( l_stringIterator, &m_chartStrings )
	{
		p_zipFile ->writeStr		( l_stringIterator ->first );
		p_zipFile ->writeNum<uint8>	( l_stringIterator ->second );
	}
} // serialize
