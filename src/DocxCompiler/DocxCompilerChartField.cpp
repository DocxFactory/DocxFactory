
#include "DocxFactory/DocxCompiler/DocxCompilerItemFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItem.h"
#include "DocxFactory/DocxCompiler/DocxCompilerField.h"
#include "DocxFactory/DocxCompiler/DocxCompilerChartField.h"

#include "DocxFactory/opc/OpcPart.h"
#include "DocxFactory/opc/OpcRelationships.h"
#include "DocxFactory/opc/OpcRelationship.h"

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



DocxCompilerChartField::ChartType DocxCompilerChartField::getChartType( OpcPart* p_part )
{
	boost::scoped_ptr<XmlTreeDriller>	l_treeDriller;
	xercesc::DOMNode*					l_node;
	set<string>							l_types;

	l_treeDriller.reset( new XmlTreeDriller(
		p_part ->getDoc() ->getDocumentElement(),
		"", "",
		"http://schemas.openxmlformats.org/drawingml/2006/chart", "areaChart,area3DChart,lineChart,line3DChart,stockChart,radarChart,scatterChart,pieChart,pie3DChart,doughnutChart,barChart,bar3DChart,ofPieChart,surfaceChart,surface3DChart,bubbleChart",
		"", "", "" ) );

	while ( l_node = l_treeDriller ->nextNode() )
	{
		l_types.insert( XmlFunc::XMLChToUtf8( l_node ->getLocalName() ) );
	}

	// if ( l_types.find( "stockChart" )		!= l_types.end() )
	// new office versions have combo charts that have multi chart types but fixed series
	// and not just stockChart's have multi charts with fixed series
	if ( l_types.size() > 1 )
		return CHART_FIXED_SERIES;

	else
	if ( l_types.find( "pieChart" )			!= l_types.end()
	  || l_types.find( "pie3DChart" )		!= l_types.end()
	//|| l_types.find( "doughnutChart" )	!= l_types.end() *** doughnut charts can have outer series charts
		)
		return CHART_SINGLE_SERIES;

	else
	if ( l_types.find( "scatterChart" )		!= l_types.end() )
		return CHART_X_Y;

	else
	if ( l_types.find( "bubbleChart" )		!= l_types.end() )
		return CHART_X_Y_SIZE;

	else
		return CHART_SERIES;
} // getChartType
	


DocxCompilerChartField::DocxCompilerChartField(
	DocxCompilerItem*		p_item,
	const string&			p_name,
	ChartType				p_chartType,
	xercesc::DOMElement*	p_placeHolderNode,
	xercesc::DOMElement*	p_drawingNode,
	OpcPart*				p_chartPart )

	: DocxCompilerField(
		p_item,
		TYPE_CHART,
		p_name,
		"",
		NULL,
		p_placeHolderNode )
{
	OpcRelationships*								l_itemFileRelationships		= p_item ->getItemFile() ->getPart() ->getRelationships();

	map<string, OpcRelationship*>::const_iterator	l_chartRelationshipIterator;
	const map<string, OpcRelationship*>*			l_chartRelationships		= p_chartPart ->getRelationships() ->getRelationshipsById();
	OpcRelationship*								l_chartRelationship;

	xercesc::DOMElement*		l_rootNode			= p_chartPart ->getDoc() ->getDocumentElement();
	xercesc::DOMNode*			l_cursorNode		= p_drawingNode;
	XmlFunc::TagPos				l_cursorPos			= XmlFunc::TAG_BEFORE;
	string						l_str;
	string						l_drawingStrings	= "";

	xercesc::DOMNamedNodeMap*	l_attributes;
	xercesc::DOMAttr*			l_attrNode;
	XMLSize_t					l_pos;
	XMLSize_t					l_len;

	m_chartType		= p_chartType;
	m_chartMlPrefix	= XmlFunc::normalizeNsPrefix( l_rootNode, "c", "http://schemas.openxmlformats.org/drawingml/2006/chart" );
	m_relMlPrefix	= XmlFunc::normalizeNsPrefix( l_rootNode, "r", "http://schemas.openxmlformats.org/officeDocument/2006/relationships" );

	while ( XmlFunc::getNextTag( p_drawingNode, l_cursorNode, l_cursorPos, l_str ) )
	{
		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
		  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "chart" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/drawingml/2006/chart" ) )
		  && ( l_cursorPos == XmlFunc::TAG_EMPTY || l_cursorPos == XmlFunc::TAG_START ) )
		{
			l_drawingStrings += "<" + XmlFunc::XMLChToUtf8( l_cursorNode ->getNodeName() );

			l_attributes	= l_cursorNode ->getAttributes();
			l_len			= l_attributes ->getLength();

			for ( l_pos = 0; l_pos < l_len; l_pos++ )
			{
				l_attrNode = ( xercesc::DOMAttr* ) l_attributes ->item( l_pos );

				if ( XmlFunc::XMLChCmp( l_attrNode ->getLocalName(),	_X( "id" ) )
				  && XmlFunc::XMLChCmp( l_attrNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/officeDocument/2006/relationships" ) ) )
				{
					l_drawingStrings	+= string( " " + XmlFunc::XMLChToUtf8( l_attrNode ->getNodeName() ) + "=\"" );
					m_drawingStrings.push_back( make_pair( l_drawingStrings, 'r' ) );
					l_drawingStrings	 = "\"";
				}

				else
					l_drawingStrings += " " 
						+ XmlFunc::XMLChToUtf8( l_attrNode ->getNodeName() ) + "=\"" 
						+ XmlFunc::XMLChToUtf8( l_attrNode ->getNodeValue() ) + "\"";
			}

			l_drawingStrings += ( l_cursorPos == XmlFunc::TAG_START ? ">" : "/>" );
		}

		else
		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
		  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "docPr" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing" ) )
		  && ( l_cursorPos == XmlFunc::TAG_EMPTY || l_cursorPos == XmlFunc::TAG_START ) )
		{
			l_drawingStrings += "<" + XmlFunc::XMLChToUtf8( l_cursorNode ->getNodeName() );

			l_attributes	= l_cursorNode ->getAttributes();
			l_len			= l_attributes ->getLength();

			for ( l_pos = 0; l_pos < l_len; l_pos++ )
			{
				l_attrNode = ( xercesc::DOMAttr* ) l_attributes ->item( l_pos );

				if ( XmlFunc::XMLChCmp( l_attrNode ->getLocalName(), _X( "id" ) ) )
				{
					l_drawingStrings	+= string( " " + XmlFunc::XMLChToUtf8( l_attrNode ->getNodeName() ) + "=\"" );
					m_drawingStrings.push_back( make_pair( l_drawingStrings, 'i' ) );
					l_drawingStrings	 = "\"";
				}

				else
					l_drawingStrings += " "
						+ XmlFunc::XMLChToUtf8( l_attrNode ->getNodeName() ) + "=\""
						+ XmlFunc::XMLChToUtf8( l_attrNode ->getNodeValue() ) + "\"";
			}

			l_drawingStrings += ( l_cursorPos == XmlFunc::TAG_START ? ">" : "/>" );
		}

		else
			l_drawingStrings += l_str;
	}

	if ( !l_drawingStrings.empty() )
		  m_drawingStrings.push_back( make_pair( l_drawingStrings, NULL ) );



	FOR_EACH( l_chartRelationshipIterator, l_chartRelationships )
	{
		l_chartRelationship = l_chartRelationshipIterator ->second;
		if ( l_chartRelationship ->getType() != "http://schemas.openxmlformats.org/officeDocument/2006/relationships/package" )
		{
			if( l_chartRelationship ->getTargetMode() == OpcRelationship::INTERNAL_MODE )
				l_itemFileRelationships ->insertRelationship(
					l_chartRelationship ->getType(),
					l_chartRelationship ->getTargetPart() );

			m_chartRelationships.push_back( boost::make_tuple(
				l_chartRelationship ->getId(),
				l_chartRelationship ->getType(),
				l_chartRelationship ->getTargetMode() == OpcRelationship::INTERNAL_MODE ? l_chartRelationship ->getTargetPart() ->getFullPath() : l_chartRelationship ->getExternalPath(),
				l_chartRelationship ->getTargetMode() == OpcRelationship::INTERNAL_MODE ) );
		}
	}
} // c'tor

DocxCompilerChartField::~DocxCompilerChartField()
{

} // d'tor



void DocxCompilerChartField::serialize( ZipFile* p_zipFile )
{
	list<pair<string, char>>::iterator							l_stringIterator;
	list<boost::tuple<string, string, string, bool>>::iterator	l_relationshipIterator;

	DocxCompilerField::serialize( p_zipFile );

	// chart type is saved in the DocxCompilerItemFile::serializeFields() function
	p_zipFile ->writeStr( m_chartMlPrefix );
	p_zipFile ->writeStr( m_relMlPrefix );

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_drawingStrings.size() );
	FOR_EACH( l_stringIterator, &m_drawingStrings )
	{
		p_zipFile ->writeStr		( l_stringIterator ->first );
		p_zipFile ->writeNum<uint8>	( l_stringIterator ->second );
	}

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_chartRelationships.size() );
	FOR_EACH( l_relationshipIterator, &m_chartRelationships )
	{
		p_zipFile ->writeStr		( get<0>( *l_relationshipIterator ) );
		p_zipFile ->writeStr		( get<1>( *l_relationshipIterator ) );
		p_zipFile ->writeStr		( get<2>( *l_relationshipIterator ) );
		p_zipFile ->writeNum<uint8>	( get<3>( *l_relationshipIterator ) );
	}
} // serialize



DocxCompilerChartField::ChartType DocxCompilerChartField::getChartType() const
{
	return m_chartType;
} // getChartType

string DocxCompilerChartField::getRelMlPrefix() const
{
	return m_relMlPrefix;
} // getRelMlPrefix

string DocxCompilerChartField::getChartMlPrefix() const
{
	return m_chartMlPrefix;
} // getChartMlPrefix

const list<pair<string, char>>*	DocxCompilerChartField::getDrawingStrings() const
{
	return &m_drawingStrings;
} // getDrawingStrings

const list<boost::tuple<string, string, string, bool>>*	DocxCompilerChartField::getChartRelationships() const
{
	return &m_chartRelationships;
} // getChartRelationships
