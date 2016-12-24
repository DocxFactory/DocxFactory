
#include "DocxFactory/DocxCompiler/DocxCompilerItemFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItem.h"
#include "DocxFactory/DocxCompiler/DocxCompilerField.h"
#include "DocxFactory/DocxCompiler/DocxCompilerChartField.h"
#include "DocxFactory/DocxCompiler/DocxCompilerChartFieldSeries.h"

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



DocxCompilerChartFieldSeries::DocxCompilerChartFieldSeries(
	DocxCompilerItem*		p_item,
	const string&			p_name,
	xercesc::DOMElement*	p_placeHolderNode,
	xercesc::DOMElement*	p_drawingNode,
	OpcPart*				p_chartPart )

	: DocxCompilerChartField(
		p_item,
		p_name,
		CHART_SERIES,
		p_placeHolderNode,
		p_drawingNode,
		p_chartPart )
{
	xercesc::DOMDocument*	l_doc			= p_chartPart ->getDoc();
	xercesc::DOMElement*	l_rootNode		= l_doc ->getDocumentElement();
	xercesc::DOMNode*		l_cursorNode	= l_rootNode;
	XmlFunc::TagPos			l_cursorPos		= XmlFunc::TAG_BEFORE;
	string					l_str;

	string					l_chartStrings	= "";
	char					l_seriesIdx		= -1;

	while ( XmlFunc::getNextTag( l_rootNode, l_cursorNode, l_cursorPos, l_str ) )
	{
		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
		  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "ser" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/drawingml/2006/chart" ) )
		  && ( l_cursorPos == XmlFunc::TAG_EMPTY || l_cursorPos == XmlFunc::TAG_START ) )
		{
			l_seriesIdx++;

			if ( l_seriesIdx == 0 )
			{
				m_chartStrings.push_back( make_pair( l_chartStrings, 's' ) );
				l_chartStrings = "";
			}

			m_seriesStrings.push_back( list<pair<string, char>>() );

			loadSeries(
				( xercesc::DOMElement* ) l_cursorNode,
				l_seriesIdx );

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

DocxCompilerChartFieldSeries::~DocxCompilerChartFieldSeries()
{

} // d'tor



void DocxCompilerChartFieldSeries::serialize( ZipFile* p_zipFile )
{
	list<pair<string, char>>::iterator			l_stringIterator;
	vector<list<pair<string, char>>>::iterator	l_seriesIterator;

	DocxCompilerChartField::serialize( p_zipFile );

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_chartStrings.size() );
	FOR_EACH( l_stringIterator, &m_chartStrings )
	{
		p_zipFile ->writeStr		( l_stringIterator ->first );
		p_zipFile ->writeNum<uint8>	( l_stringIterator ->second );
	}

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_seriesStrings.size() );
	FOR_EACH( l_seriesIterator, &m_seriesStrings )
	{
		p_zipFile ->writeNum<uint32>( ( uint32 ) l_seriesIterator ->size() );
		FOR_EACH( l_stringIterator, l_seriesIterator )
		{
			p_zipFile ->writeStr		( l_stringIterator ->first );
			p_zipFile ->writeNum<uint8>	( l_stringIterator ->second );
		}
	}

	p_zipFile ->writeNum<int16>	( m_catType );
} // serialize



void DocxCompilerChartFieldSeries::loadSeries(
	xercesc::DOMElement*	p_seriesNode,
	char					p_seriesIdx )
{
	map<DocxCompilerField::FieldParam, string>	l_valueByParam;
	DocxCompilerField:FieldType					l_fieldType;
	string										l_format;

	boost::scoped_ptr<XmlTreeDriller>	l_treeDriller;

	xercesc::DOMNode*			l_cursorNode	= p_seriesNode;
	XmlFunc::TagPos				l_cursorPos		= XmlFunc::TAG_BEFORE;
	string						l_str;
	string						l_seriesStrings	= "";

	xercesc::DOMNode*			l_node;
	xercesc::DOMNamedNodeMap*	l_attributes;
	xercesc::DOMAttr*			l_attrNode;
	XMLSize_t					l_pos;
	XMLSize_t					l_len;

	while ( XmlFunc::getNextTag( p_seriesNode, l_cursorNode, l_cursorPos, l_str ) )
	{
		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
		  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "idx" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/drawingml/2006/chart" ) )
		  && ( l_cursorPos == XmlFunc::TAG_EMPTY || l_cursorPos == XmlFunc::TAG_START ) )
		{
			l_seriesStrings += "<" + XmlFunc::XMLChToUtf8( l_cursorNode ->getNodeName() );

			l_attributes	= l_cursorNode ->getAttributes();
			l_len			= l_attributes ->getLength();

			for ( l_pos = 0; l_pos < l_len; l_pos++ )
			{
				l_attrNode = ( xercesc::DOMAttr* ) l_attributes ->item( l_pos );

				if ( XmlFunc::XMLChCmp( l_attrNode ->getLocalName(), _X( "val" ) ) )
				{
					l_seriesStrings	+= " " + XmlFunc::XMLChToUtf8( l_attrNode ->getNodeName() ) + "=\"";
					m_seriesStrings[ p_seriesIdx ].push_back( make_pair( l_seriesStrings, 'i' ) );
					l_seriesStrings	 = "\"";
				}

				else
					l_seriesStrings += " " 
						+ XmlFunc::XMLChToUtf8( l_attrNode ->getNodeName() ) + "=\"" 
						+ XmlFunc::XMLChToUtf8( l_attrNode ->getNodeValue() ) + "\"";
			}

			l_seriesStrings += ( l_cursorPos == XmlFunc::TAG_START ? ">" : "/>" );
		}

		else
		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
		  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "order" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/drawingml/2006/chart" ) )
		  && ( l_cursorPos == XmlFunc::TAG_EMPTY || l_cursorPos == XmlFunc::TAG_START ) )
		{
			l_seriesStrings += "<" + XmlFunc::XMLChToUtf8( l_cursorNode ->getNodeName() );

			l_attributes	= l_cursorNode ->getAttributes();
			l_len			= l_attributes ->getLength();

			for ( l_pos = 0; l_pos < l_len; l_pos++ )
			{
				l_attrNode = ( xercesc::DOMAttr* ) l_attributes ->item( l_pos );

				if ( XmlFunc::XMLChCmp( l_attrNode ->getLocalName(), _X( "val" ) ) )
				{
					l_seriesStrings	+= " " + XmlFunc::XMLChToUtf8( l_attrNode ->getNodeName() ) + "=\"";
					m_seriesStrings[ p_seriesIdx ].push_back( make_pair( l_seriesStrings, 'o' ) );
					l_seriesStrings	 = "\"";
				}

				else
					l_seriesStrings += " " 
						+ XmlFunc::XMLChToUtf8( l_attrNode ->getNodeName() ) + "=\"" 
						+ XmlFunc::XMLChToUtf8( l_attrNode ->getNodeValue() ) + "\"";
			}

			l_seriesStrings += ( l_cursorPos == XmlFunc::TAG_START ? ">" : "/>" );
		}

		else
		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
		  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "tx" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/drawingml/2006/chart" ) )
		  && ( l_cursorPos == XmlFunc::TAG_EMPTY || l_cursorPos == XmlFunc::TAG_START ) )
		{
			l_seriesStrings	+=
				  "<"	+ XmlFunc::XMLChToUtf8( l_cursorNode ->getNodeName() ) + ">"
				+ "<"	+ m_chartMlPrefix + ":v>";

			m_seriesStrings[ p_seriesIdx ].push_back( make_pair( l_seriesStrings, 't' ) );
			l_seriesStrings	 =
				  "</"	+ m_chartMlPrefix + ":v>"
				+ "</"	+ XmlFunc::XMLChToUtf8( l_cursorNode ->getNodeName() ) + ">";

			l_cursorPos	= XmlFunc::TAG_END;
		}

		else
		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
		  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "cat" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/drawingml/2006/chart" ) )
		  && ( l_cursorPos == XmlFunc::TAG_EMPTY || l_cursorPos == XmlFunc::TAG_START ) )
		{
			l_seriesStrings	+= "<" + XmlFunc::XMLChToUtf8( l_cursorNode ->getNodeName() ) + ">";

			l_node = XmlFunc::getChildByTagName(
				l_cursorNode,
				NULL,
				"http://schemas.openxmlformats.org/drawingml/2006/chart",
				"numRef,numLit,strRef,strLit" );

			if ( l_node ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
			&& ( XmlFunc::XMLChCmp( l_node ->getLocalName(), _X( "numRef" ) )
			  || XmlFunc::XMLChCmp( l_node ->getLocalName(), _X( "numLit" ) ) ) )
			{
				l_fieldType = DocxCompilerField::TYPE_NUMBER;

				l_seriesStrings	+=
					"<" + m_chartMlPrefix + ":numLit>";

				l_treeDriller.reset( new XmlTreeDriller(
					l_node,
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

						l_seriesStrings	+=
							  "<"	+ m_chartMlPrefix + ":formatCode>"
							+ l_format
							+ "</"	+ m_chartMlPrefix + ":formatCode>";

						DocxCompilerField::getTypeAndFormat( 
							l_format,
							l_fieldType,
							&l_valueByParam );

						if( l_fieldType != DocxCompilerField::TYPE_DATE_TIME
						 && l_fieldType != DocxCompilerField::TYPE_NUMBER )
							l_fieldType	 = DocxCompilerField::TYPE_NUMBER;
					}
				}

				m_seriesStrings[ p_seriesIdx ].push_back( make_pair( l_seriesStrings, 'c' ) );
				l_seriesStrings = "</" + m_chartMlPrefix + ":numLit>";
			}

			else
			{
				l_fieldType = DocxCompilerField::TYPE_TEXT;

				l_seriesStrings	+= "<"	+ m_chartMlPrefix + ":strLit>";
				m_seriesStrings[ p_seriesIdx ].push_back( make_pair( l_seriesStrings, 'c' ) );
				l_seriesStrings	 = "</"	+ m_chartMlPrefix + ":strLit>";
			}

			l_seriesStrings	+= "</" + XmlFunc::XMLChToUtf8( l_cursorNode ->getNodeName() ) + ">";
			m_catType	 = l_fieldType;

			l_cursorPos	= XmlFunc::TAG_END;
		}

		else
		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
		  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "val" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/drawingml/2006/chart" ) )
		  && ( l_cursorPos == XmlFunc::TAG_EMPTY || l_cursorPos == XmlFunc::TAG_START ) )
		{
			l_seriesStrings	+=
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

					l_seriesStrings	+=
						  "<"	+ m_chartMlPrefix + ":formatCode>"
						+ l_format
						+ "</"	+ m_chartMlPrefix + ":formatCode>";
				}
			}

			m_seriesStrings[ p_seriesIdx ].push_back( make_pair( l_seriesStrings, 'v' ) );
			l_seriesStrings =
				  "</" + m_chartMlPrefix + ":numLit>"
				+ "</" + XmlFunc::XMLChToUtf8( l_cursorNode ->getNodeName() ) + ">";

			l_cursorPos	= XmlFunc::TAG_END;
		}

		else
			l_seriesStrings += l_str;
	}

	if ( !l_seriesStrings.empty() )
		  m_seriesStrings[ p_seriesIdx ].push_back( make_pair( l_seriesStrings, NULL ) );
} // loadSeries
