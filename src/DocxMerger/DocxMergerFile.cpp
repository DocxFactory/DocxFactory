
#include "DocxFactory/WordProcessingMerger/CompileVersionMismatchException.h"
#include "DocxFactory/WordProcessingMerger/InvalidMergeDataException.h"

#include "DocxFactory/DocxMerger/DocxMergerFile.h"
#include "DocxFactory/DocxMerger/DocxMergerSettingsFile.h"
#include "DocxFactory/DocxMerger/DocxMergerItemFile.h"
#include "DocxFactory/DocxMerger/DocxMergerItemGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerItemLayout.h"
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerTextField.h"
#include "DocxFactory/DocxMerger/DocxMergerNumberField.h"
#include "DocxFactory/DocxMerger/DocxMergerDateTimeField.h"
#include "DocxFactory/DocxMerger/DocxMergerBooleanField.h"
#include "DocxFactory/DocxMerger/DocxMergerAltChunkField.h"
#include "DocxFactory/DocxMerger/DocxMergerPicField.h"
#include "DocxFactory/DocxMerger/DocxMergerBarcodeField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartFieldSeries.h"
#include "DocxFactory/DocxMerger/DocxMergerChartFieldSingleSeries.h"
#include "DocxFactory/DocxMerger/DocxMergerChartFieldFixedSeries.h"
#include "DocxFactory/DocxMerger/DocxMergerChartFieldXY.h"
#include "DocxFactory/DocxMerger/DocxMergerChartFieldXYSize.h"
#include "DocxFactory/DocxMerger/DocxMergerXmlString.h"
#include "DocxFactory/DocxMerger/DocxMergerId.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteItemGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteItem.h"

#include "DocxFactory/opc/OpcPart.h"

#include "DocxFactory/xml/XmlFunc.h"
#include "DocxFactory/xml/XmlInitializer.h"

#include "DocxFactory/json/JsonFunc.h"
#include "DocxFactory/json/ParseJsonException.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/UnzipFile.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/str/StrFunc.h"

#include "DocxFactory/util/DocxFactoryDefs.h"

#include "xercesc/util/XercesDefs.hpp"
#include "xercesc/dom/DOM.hpp"

#include "boost/scoped_ptr.hpp"

using namespace DocxFactory;
using namespace std;



DocxMergerFile::DocxMergerFile()
{
	XmlInitializer::getInstance();

	m_altChunkSeq	= 0;
	m_chartSeq		= 0;
	m_idSeq			= 10000;
} // c'tor

DocxMergerFile::~DocxMergerFile()
{
	list<DocxMergerItemFile*>::iterator			l_itemFileIterator;
	list<DocxMergerPasteFieldGroup*>::iterator	l_pasteFieldGroupIterator;
	list<DocxMergerId*>::iterator				l_idIterator;

	if ( m_settingsFile )
		delete m_settingsFile;

	FOR_EACH( l_itemFileIterator, &m_itemFiles )
	{
		delete *l_itemFileIterator;
	}

	FOR_EACH( l_pasteFieldGroupIterator, &m_pasteFieldGroups )
	{
		delete *l_pasteFieldGroupIterator;
	}

	FOR_EACH( l_idIterator, &m_ids )
	{
		delete *l_idIterator;
	}
} // d'tor



void DocxMergerFile::load( const string& p_fileName )
{
	UnzipFile* l_unzipFile = getUnzipFile();

	l_unzipFile ->open( p_fileName );
	OpcPackage::load( p_fileName );

	deserialize				( l_unzipFile );
	deserializeSettingsFile	( l_unzipFile );
	deserializeItemFiles	( l_unzipFile );
	deserializeItemGroups	( l_unzipFile );
	deserializeItems		( l_unzipFile );
	deserializeFields		( l_unzipFile );
	deserializeIds			( l_unzipFile );
	deserializeXmlStrings	( l_unzipFile );

	link					( l_unzipFile );
	linkSettingsFile		( l_unzipFile );
	linkItemFiles			( l_unzipFile );
	linkItemGroups			( l_unzipFile );
	linkItems				( l_unzipFile );
	linkFields				( l_unzipFile );
	linkIds					( l_unzipFile );
	linkXmlStrings			( l_unzipFile );
} // load

void DocxMergerFile::save( const string& p_fileName, bool p_updateToc )
{
	list<DocxMergerItemFile*>::iterator	l_itemFileIterator;

	ZipFile*	l_zipFile	= getZipFile();
	UnzipFile*	l_unzipFile	= getUnzipFile();

	l_zipFile ->open( p_fileName, ZipFile::ZIP_CREATE );

	FOR_EACH( l_itemFileIterator, &m_itemFiles )
	{
		( *l_itemFileIterator ) ->save( l_zipFile );
	}

	if( m_hasToc && p_updateToc )
		m_settingsFile ->setUpdateFields( true );

	OpcPackage::save( p_fileName );

	l_zipFile ->close();
	l_unzipFile ->close();
} // save



void DocxMergerFile::merge( const string& p_data )
{
	if ( StrFunc::begins( p_data, "<" ) )
		mergeByXml( p_data );

	else
	if ( StrFunc::begins( p_data, "{" ) )
		mergeByJson( p_data );

	else
		throw InvalidMergeDataException( __FILE__, __LINE__ );;
} // merge

void DocxMergerFile::mergeByXml( const string& p_xml )
{
	xercesc::DOMLSParser*	l_parser	= getDomParser();
	xercesc::DOMDocument*	l_doc		= XmlFunc::parseBufToDoc( l_parser, ( byte* ) p_xml.c_str(), p_xml.length() );

	LocaleFunc::setCodePage( "UTF-8" );
	mergeItemByXml( l_doc ->getDocumentElement() );
} // mergeByXml

void DocxMergerFile::mergeItemByXml( xercesc::DOMElement* p_node )
{
	typedef multimap<string, DocxMergerField*>::const_iterator FieldIterator_T;

	list<DocxMergerItemFile*>::iterator			l_itemFileIterator;
	DocxMergerItemFile*							l_itemFile		= NULL;
	DocxMergerItemFile::ItemFileType			l_itemFileType;

	map<string, DocxMergerItem*>::iterator		l_itemIterator;
	DocxMergerItem*								l_item			= NULL;
	string										l_itemName		= StrFunc::lc( XmlFunc::XMLChToUtf8( p_node ->getNodeName() ) );

	const multimap<string, DocxMergerField*>*	l_fields;
	pair<FieldIterator_T, FieldIterator_T>		l_fieldRange;
	FieldIterator_T								l_fieldIterator;
	DocxMergerField*							l_field;

	list<xercesc::DOMElement*>					l_elementList;
	list<xercesc::DOMElement*>::iterator		l_elementIterator;

	xercesc::DOMNodeList*						l_childNodeList	= p_node ->getChildNodes();
	xercesc::DOMNode*							l_childNode;
	xercesc::DOMElement*						l_childElement;
	string										l_childName;
	XMLSize_t									l_childLen		= l_childNodeList ->getLength();
	XMLSize_t									l_childPos;

	if ( l_itemName == "_header"
	  || l_itemName == "_footer" )
	{
		FOR_EACH( l_itemFileIterator, &m_itemFiles )
		{
			l_itemFile		= *l_itemFileIterator;
			l_itemFileType	= l_itemFile ->getType();

			if( l_itemFileType == DocxMergerItemFile::HEADER
			 || l_itemFileType == DocxMergerItemFile::FOOTER )
			{
				l_item		= l_itemFile ->getRootItemGroup() ->getChildItemsByName() ->begin() ->second;
				l_fields	= getHeaderFieldsByName();
				break;
			}
		}
	}

	else
	{
		l_itemIterator = m_itemsByName.find( l_itemName );
		if( l_itemIterator != m_itemsByName.end() )
		{
			l_item			= l_itemIterator ->second;
			l_itemFile		= l_item ->getItemFile();
			l_itemFileType	= l_itemFile ->getType();
			l_fields		= l_item ->getFieldsByName();
		}
	}

	if ( l_item )
	{
		l_elementList.clear();

		for ( l_childPos = 0; l_childPos < l_childLen; ++l_childPos )
		{
			l_childNode = l_childNodeList ->item( l_childPos );
			if ( l_childNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE )
			{
				l_childElement	= ( xercesc::DOMElement* ) l_childNode;
				l_childName		= StrFunc::lc( XmlFunc::XMLChToUtf8( l_childElement ->getNodeName() ) );

				l_fieldRange = l_fields ->equal_range( l_childName );
				if ( l_fieldRange.first != l_fieldRange.second )
				{
					for ( l_fieldIterator = l_fieldRange.first; l_fieldIterator != l_fieldRange.second; ++l_fieldIterator )
					{
						l_field = l_fieldIterator ->second;
						if( l_field ->getType() != DocxMergerField::TYPE_CHART )
							mergeFieldByXml( l_childElement, l_field );
						else
							mergeChartFieldByXml( l_childElement, ( DocxMergerChartField* ) l_field );
					}
				}

				else
					l_elementList.push_back( l_childElement );
			}
		}

		if( l_itemFileType != DocxMergerItemFile::HEADER
		 && l_itemFileType != DocxMergerItemFile::FOOTER )
			l_itemFile ->paste( l_item );

		FOR_EACH( l_elementIterator, &l_elementList )
		{
			mergeItemByXml( *l_elementIterator );
		}
	}

	else
	{
		for ( l_childPos = 0; l_childPos < l_childLen; ++l_childPos )
		{
			l_childNode = l_childNodeList ->item( l_childPos );
			if ( l_childNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE )

				mergeItemByXml( ( xercesc::DOMElement* ) l_childNode );
		}
	}
} // mergeItemByXml

void DocxMergerFile::mergeFieldByXml( xercesc::DOMElement* p_node, DocxMergerField* p_field )
{
	if( p_node ->getChildNodes() ->getLength()		== 1
	 && p_node ->getFirstChild() ->getNodeType()	== xercesc::DOMNode::TEXT_NODE )
		p_field ->setClipboardValue( XmlFunc::XMLChToUtf8( p_node ->getFirstChild() ->getNodeValue() ) );

	else
	{
		xercesc::DOMNode*	l_cursorNode	= p_node;
		XmlFunc::TagPos		l_cursorPos		= XmlFunc::TAG_START;
		string				l_stringValue;
		string				l_value;

		while ( XmlFunc::getNextTag( p_node, l_cursorNode, l_cursorPos, l_stringValue ) )
		{
			if ( l_cursorNode == p_node )
				break;

			if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::CDATA_SECTION_NODE )
				l_value += XmlFunc::XMLChToUtf8( ( ( xercesc::DOMCharacterData* ) l_cursorNode ) ->getData() );
			else
				l_value += l_stringValue;
		}

		p_field ->setClipboardValue( l_value );
	}
} // mergeFieldByXml

void DocxMergerFile::mergeChartFieldByXml( xercesc::DOMElement* p_node, DocxMergerChartField* p_chartField )
{
	xercesc::DOMNodeList*	l_seriesNodeList	= p_node ->getChildNodes();
	xercesc::DOMNode*		l_seriesNode;
	XMLSize_t				l_seriesLen			= l_seriesNodeList ->getLength();
	XMLSize_t				l_seriesPos;

	xercesc::DOMNodeList*	l_childNodeList;
	xercesc::DOMNode*		l_childNode;
	xercesc::DOMElement*	l_childElement;
	string					l_childName;
	XMLSize_t				l_childLen;
	XMLSize_t				l_childPos;

	string					l_series;
	string					l_category;
	double					l_value;
	string					l_str;

	for ( l_seriesPos = 0; l_seriesPos < l_seriesLen; ++l_seriesPos )
	{
		l_seriesNode = l_seriesNodeList ->item( l_seriesPos );

		if ( l_seriesNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
		  && StrFunc::lc( XmlFunc::XMLChToUtf8( l_seriesNode ->getNodeName() ) ) == "point" ) 
		{
			l_series	= "";
			l_category	= "";
			l_value		= 0.0f;

			l_childNodeList	= l_seriesNode ->getChildNodes();
			l_childLen		= l_childNodeList ->getLength();

			for ( l_childPos = 0; l_childPos < l_childLen; ++l_childPos )
			{
				l_childNode = l_childNodeList ->item( l_childPos );
				if ( l_childNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE )
				{
					l_childElement = ( xercesc::DOMElement* ) l_childNode;

					if ( l_childElement ->getChildNodes() ->getLength()		== 1
					  && l_childElement ->getFirstChild() ->getNodeType()	== xercesc::DOMNode::TEXT_NODE )
					{
						l_childName	= StrFunc::lc( XmlFunc::XMLChToUtf8( l_childElement ->getNodeName() ) );
						l_str		= XmlFunc::XMLChToUtf8( l_childElement ->getFirstChild() ->getNodeValue() );

							 if ( l_childName == "series" )		l_series	= l_str;
						else if ( l_childName == "category" )	l_category	= l_str;
						else if ( l_childName == "value" )		l_value		= LocaleFunc::strToNum( l_str );
						else

						switch ( p_chartField ->getChartType() )
						{
						case DocxMergerChartField::CHART_X_Y:
						case DocxMergerChartField::CHART_X_Y_SIZE:

							 if ( l_childName == "x" )			l_series	= l_str;
						else if ( l_childName == "y" )			l_category	= l_str;
						else if ( l_childName == "size" )		l_value		= LocaleFunc::strToNum( l_str );
						break;
						}
					}
				}
			}

			p_chartField ->setChartValue( l_series, l_category, l_value );
		}
	}
} // mergeChartByXml



void DocxMergerFile::mergeByJson( const string& p_json )
{
	rapidjson::Document	l_doc;
	l_doc.Parse<0>( p_json.c_str() );

	if ( l_doc.HasParseError() )
		throw ParseJsonException( __FILE__, __LINE__ );;

	mergeItemByJson( &l_doc, NULL );
} // mergeByJson

void DocxMergerFile::mergeItemByJson( rapidjson::Value* p_value, DocxMergerItem* p_item )
{
	typedef multimap<string, DocxMergerField*>::const_iterator FieldIterator_T;

	list<DocxMergerItemFile*>::iterator							l_itemFileIterator;
	DocxMergerItemFile*											l_itemFile;
	DocxMergerItemFile::ItemFileType							l_itemFileType;

	map<string, DocxMergerItem*>::iterator						l_itemIterator;
	DocxMergerItem*												l_item;

	const multimap<string, DocxMergerField*>*					l_fields;
	pair<FieldIterator_T, FieldIterator_T>						l_fieldRange;
	FieldIterator_T												l_fieldIterator;
	DocxMergerField*											l_field;

	list<pair<rapidjson::Value*, DocxMergerItem*>>				l_valueList;
	list<pair<rapidjson::Value*, DocxMergerItem*>>::iterator	l_valueIterator;

	rapidjson::Value::Member*									l_childMember;
	rapidjson::Value*											l_childValue;
	string														l_childName;

	if ( p_item )
	{
		if ( p_value ->IsObject() )
		{
			l_itemFile		= p_item ->getItemFile();
			l_itemFileType	= l_itemFile ->getType();

			if( l_itemFileType == DocxMergerItemFile::HEADER
			 || l_itemFileType == DocxMergerItemFile::FOOTER )
				l_fields = getHeaderFieldsByName();
			else
				l_fields = p_item ->getFieldsByName();

			l_valueList.clear();

			for ( l_childMember = p_value ->MemberBegin(); l_childMember != p_value ->MemberEnd(); ++l_childMember )
			{
				l_childName		= StrFunc::lc( l_childMember ->name.GetString() );
				l_childValue	= &l_childMember ->value;

				l_fieldRange = l_fields ->equal_range( l_childName );
				if ( l_fieldRange.first != l_fieldRange.second )
				{
					for ( l_fieldIterator = l_fieldRange.first; l_fieldIterator != l_fieldRange.second; ++l_fieldIterator )
					{
						l_field = l_fieldIterator ->second;
						if( l_field ->getType() != DocxMergerField::TYPE_CHART )
							mergeFieldByJson( l_childValue, l_field );
						else
							mergeChartFieldByJson( l_childValue, ( DocxMergerChartField* ) l_field );
					}
				}

				else
				if ( l_childValue ->IsObject() || l_childValue ->IsArray() )
				{
					l_item = NULL;

					if ( l_childName == "_header"
					  || l_childName == "_footer" )
					{
						FOR_EACH( l_itemFileIterator, &m_itemFiles )
						{
							l_itemFile		= *l_itemFileIterator;
							l_itemFileType	= l_itemFile ->getType();

							if( l_itemFileType == DocxMergerItemFile::HEADER
							 || l_itemFileType == DocxMergerItemFile::FOOTER )
							{
								l_item = l_itemFile ->getRootItemGroup() ->getChildItemsByName() ->begin() ->second;
								break;
							}
						}
					}

					else
					{
						l_itemIterator = m_itemsByName.find( l_childName );
						if( l_itemIterator != m_itemsByName.end() )
							l_item = l_itemIterator ->second;

						/***
						if (!l_item
						  && l_childName.length() > 1
						  && l_childName.substr( l_childName.length() - 1, 1 ) == "s" )
						{
							l_itemIterator = m_itemsByName.find( l_childName.substr( 0, l_childName.length() - 1 ) );
							if( l_itemIterator != m_itemsByName.end() )
								l_item = l_itemIterator ->second;
						}
						***/
					}

					l_valueList.push_back( make_pair( l_childValue, l_item ) );
				}
			}

			l_itemFile		= p_item ->getItemFile();
			l_itemFileType	= l_itemFile ->getType();

			if( l_itemFileType != DocxMergerItemFile::HEADER
			 && l_itemFileType != DocxMergerItemFile::FOOTER )
				l_itemFile ->paste( p_item );

			FOR_EACH( l_valueIterator, &l_valueList )
			{
				mergeItemByJson( l_valueIterator ->first, l_valueIterator ->second );
			}
		}

		else
		{
			for ( l_childValue = p_value ->Begin(); l_childValue != p_value ->End(); ++l_childValue )
			{
				if ( l_childValue ->IsObject() || l_childValue ->IsArray() )
					mergeItemByJson( l_childValue, p_item );
			}
		}
	}

	else
	{
		if ( p_value ->IsObject() )
		{
			for ( l_childMember = p_value ->MemberBegin(); l_childMember != p_value ->MemberEnd(); ++l_childMember )
			{
				l_childName		= StrFunc::lc( l_childMember ->name.GetString() );
				l_childValue	= &l_childMember ->value;

				if ( l_childValue ->IsObject() || l_childValue ->IsArray() )
				{
					l_item = NULL;

					if ( l_childName == "_header"
					  || l_childName == "_footer" )
					{
						FOR_EACH( l_itemFileIterator, &m_itemFiles )
						{
							l_itemFile		= *l_itemFileIterator;
							l_itemFileType	= l_itemFile ->getType();

							if( l_itemFileType == DocxMergerItemFile::HEADER
							 || l_itemFileType == DocxMergerItemFile::FOOTER )
							{
								l_item = l_itemFile ->getRootItemGroup() ->getChildItemsByName() ->begin() ->second;
								break;
							}
						}
					}

					else
					{
						l_itemIterator = m_itemsByName.find( l_childName );
						if( l_itemIterator != m_itemsByName.end() )
							l_item = l_itemIterator ->second;

						/***
						if (!l_item
						  && l_childName.length() > 1
						  && l_childName.substr( l_childName.length() - 1, 1 ) == "s" )
						{
							l_itemIterator = m_itemsByName.find( l_childName.substr( 0, l_childName.length() - 1 ) );
							if( l_itemIterator != m_itemsByName.end() )
								l_item = l_itemIterator ->second;
						}
						***/
					}

					mergeItemByJson( l_childValue, l_item );
				}
			}
		}

		else
		{
			for ( l_childValue = p_value ->Begin(); l_childValue != p_value ->End(); ++l_childValue )
			{
				if ( l_childValue ->IsObject() || l_childValue ->IsArray() )
					mergeItemByJson( l_childValue, p_item );
			}
		}
	}
} // mergeItemByJson

void DocxMergerFile::mergeFieldByJson( rapidjson::Value* p_value, DocxMergerField* p_field )
{
	if ( p_value ->IsString() )
		return p_field ->setClipboardValue( p_value ->GetString() );

	else
	if ( p_value ->IsNumber() )
		return p_field ->setClipboardValue( p_value ->GetDouble() );

	else
	if ( p_value ->IsBool() )
		return p_field ->setClipboardValue( ( p_value ->GetBool() ? "true" : "false" ) );
} // mergeFieldByJson

void DocxMergerFile::mergeChartFieldByJson( rapidjson::Value* p_value, DocxMergerChartField* p_chartField )
{
	rapidjson::Value*			l_seriesValue;
	rapidjson::Value::Member*	l_childMember;
	rapidjson::Value*			l_childValue;
	string						l_childName;

	string						l_series;
	string						l_category;
	double						l_value;

	if ( p_value ->IsArray() )
	{
		for ( l_seriesValue = p_value ->Begin(); l_seriesValue != p_value ->End(); ++l_seriesValue )
		{
			if ( l_seriesValue ->IsObject() )
			{
				l_series	= "";
				l_category	= "";
				l_value		= 0.0f;

				for ( l_childMember = l_seriesValue ->MemberBegin(); l_childMember != l_seriesValue ->MemberEnd(); ++l_childMember )
				{
					l_childName		= StrFunc::lc( l_childMember ->name.GetString() );
					l_childValue	= &l_childMember ->value;

						 if ( l_childName == "series" )		l_series	= JsonFunc::getStr( l_childValue );
					else if ( l_childName == "category" )	l_category	= JsonFunc::getStr( l_childValue );
					else if ( l_childName == "value" )		l_value		= JsonFunc::getNum( l_childValue );
					else

					switch ( p_chartField ->getChartType() )
					{
					case DocxMergerChartField::CHART_X_Y:
					case DocxMergerChartField::CHART_X_Y_SIZE:

					 	 if ( l_childName == "x" )			l_series	= JsonFunc::getStr( l_childValue );
					else if ( l_childName == "y" )			l_category	= JsonFunc::getStr( l_childValue );
					else if ( l_childName == "size" )		l_value		= JsonFunc::getNum( l_childValue );
					break;
					}
				}

				p_chartField ->setChartValue( l_series, l_category, l_value );
			}
		}
	}
} // mergerChartFieldByJson



void DocxMergerFile::setChartValue( const string& p_itemName, const string& p_fieldName, const string&	p_series, const string&	p_category, double p_value )
{
	setChartValueImpl( p_itemName, p_fieldName, p_series, p_category, p_value );
} // setChartValue

void DocxMergerFile::setChartValue( const string& p_itemName, const string& p_fieldName, const string&	p_series, double p_category, double p_value )
{
	setChartValueImpl( p_itemName, p_fieldName, p_series, p_category, p_value );
} // setChartValue

void DocxMergerFile::setChartValue( const string& p_itemName, const string& p_fieldName, double p_series, double p_category, double p_value )
{
	setChartValueImpl( p_itemName, p_fieldName, p_series, p_category, p_value );
} // setChartValue

template<class Series_T, class Category_T>
void DocxMergerFile::setChartValueImpl( const string& p_itemName, const string& p_fieldName, Series_T p_series, Category_T p_category, double p_value )
{
	typedef multimap<string, DocxMergerField*>::const_iterator FieldIterator_T;

	list<DocxMergerItemFile*>::iterator		l_itemFileIterator;
	DocxMergerItemFile*						l_itemFile;
	DocxMergerItemFile::ItemFileType		l_itemFileType;

	map<string, DocxMergerItem*>::iterator	l_itemIterator;
	DocxMergerItem*							l_item;

	pair<FieldIterator_T, FieldIterator_T>	l_fieldRange;
	FieldIterator_T							l_fieldIterator;
	DocxMergerField*						l_field;

	if ( p_itemName == "_header"
	  || p_itemName == "_footer" )
	{
		FOR_EACH( l_itemFileIterator, &m_itemFiles )
		{
			l_itemFile		= *l_itemFileIterator;
			l_itemFileType	= l_itemFile ->getType();

			if ( l_itemFileType == DocxMergerItemFile::HEADER
			  || l_itemFileType == DocxMergerItemFile::FOOTER )
			{
				// get the first and only item from the item file's root group
				l_item = l_itemFile ->getRootItemGroup() ->getChildItemsByName() ->begin() ->second;

				l_fieldRange = l_item ->getFieldsByName() ->equal_range( p_fieldName );
				for ( l_fieldIterator = l_fieldRange.first; l_fieldIterator != l_fieldRange.second; ++l_fieldIterator )
				{
					l_field = l_fieldIterator ->second;
					if( l_field ->getType() == DocxMergerField::TYPE_CHART )
						( ( DocxMergerChartField* ) l_field ) ->setChartValue( p_series, p_category, p_value );
				}
			}
		}
	}

	else
	if ( !p_itemName.empty() )
	{
		l_itemIterator = m_itemsByName.find( p_itemName );
		if ( l_itemIterator != m_itemsByName.end() )
		{
			l_item = l_itemIterator ->second;

			l_fieldRange = l_item ->getFieldsByName() ->equal_range( p_fieldName );
			for ( l_fieldIterator = l_fieldRange.first; l_fieldIterator != l_fieldRange.second; ++l_fieldIterator )
			{
				l_field = l_fieldIterator ->second;
				if ( l_field ->getType() == DocxMergerField::TYPE_CHART )
					( ( DocxMergerChartField* ) l_field ) ->setChartValue( p_series, p_category, p_value );
			}
		}
	}

	else
	{
		l_fieldRange = m_fieldsByName.equal_range( p_fieldName );
		for ( l_fieldIterator = l_fieldRange.first; l_fieldIterator != l_fieldRange.second; ++l_fieldIterator )
		{
			l_field = l_fieldIterator ->second;
			if ( l_field ->getType() == DocxMergerField::TYPE_CHART )
				( ( DocxMergerChartField* ) l_field ) ->setChartValue( p_series, p_category, p_value );
		}
	}
} // setChartValueImpl



void DocxMergerFile::setClipboardValue( const string& p_itemName, const string& p_fieldName, const string& p_value )
{
	setClipboardValueImpl( p_itemName, p_fieldName, p_value );
} // setClipboardValue( string )

void DocxMergerFile::setClipboardValue( const string& p_itemName, const string& p_fieldName, double p_value )
{
	setClipboardValueImpl( p_itemName, p_fieldName, p_value );
} // setClipboardValue( double )

template<class T>
void DocxMergerFile::setClipboardValueImpl( const string& p_itemName, const string& p_fieldName, T p_value )
{
	typedef multimap<string, DocxMergerField*>::const_iterator FieldIterator_T;

	list<DocxMergerItemFile*>::iterator		l_itemFileIterator;
	DocxMergerItemFile*						l_itemFile;
	DocxMergerItemFile::ItemFileType		l_itemFileType;

	map<string, DocxMergerItem*>::iterator	l_itemIterator;
	DocxMergerItem*							l_item;

	pair<FieldIterator_T, FieldIterator_T>	l_fieldRange;
	FieldIterator_T							l_fieldIterator;
	DocxMergerField*						l_field;

	if ( p_itemName == "_header"
	  || p_itemName == "_footer" )
	{
		FOR_EACH( l_itemFileIterator, &m_itemFiles )
		{
			l_itemFile		= *l_itemFileIterator;
			l_itemFileType	= l_itemFile ->getType();

			if ( l_itemFileType == DocxMergerItemFile::HEADER
			  || l_itemFileType == DocxMergerItemFile::FOOTER )
			{
				// get the first and only item from the item file's root group
				l_item = l_itemFile ->getRootItemGroup() ->getChildItemsByName() ->begin() ->second;

				l_fieldRange = l_item ->getFieldsByName() ->equal_range( p_fieldName );
				for ( l_fieldIterator = l_fieldRange.first; l_fieldIterator != l_fieldRange.second; ++l_fieldIterator )
				{
					l_field = l_fieldIterator ->second;
					if( l_field ->getType() != DocxMergerField::TYPE_CHART )
						l_field ->setClipboardValue( p_value );
				}
			}
		}
	}

	else
	if ( !p_itemName.empty() )
	{
		l_itemIterator = m_itemsByName.find( p_itemName );
		if ( l_itemIterator != m_itemsByName.end() )
		{
			l_item = l_itemIterator ->second;

			l_fieldRange = l_item ->getFieldsByName() ->equal_range( p_fieldName );
			for ( l_fieldIterator = l_fieldRange.first; l_fieldIterator != l_fieldRange.second; ++l_fieldIterator )
			{
				l_field = l_fieldIterator ->second;
				if( l_field ->getType() != DocxMergerField::TYPE_CHART )
					l_field ->setClipboardValue( p_value );
			}
		}
	}

	else
	{
		l_fieldRange = m_fieldsByName.equal_range( p_fieldName );
		for ( l_fieldIterator = l_fieldRange.first; l_fieldIterator != l_fieldRange.second; ++l_fieldIterator )
		{
			l_field = l_fieldIterator ->second;
			if( l_field ->getType() != DocxMergerField::TYPE_CHART )
				l_field ->setClipboardValue( p_value );
		}
	}
} // setClipboardValueImpl

void DocxMergerFile::paste( const string& p_itemName )
{
	map<string, DocxMergerItem*>::iterator	l_itemIterator;
	DocxMergerItem*							l_item;
	DocxMergerItemFile*						l_itemFile;

	l_itemIterator = m_itemsByName.find( p_itemName );
	if ( l_itemIterator != m_itemsByName.end() )
	{
		l_item		= l_itemIterator ->second;
		l_itemFile	= l_item ->getItemFile();
		l_itemFile ->paste( l_item );
	}
} // paste



void DocxMergerFile::deserialize( UnzipFile* p_unzipFile )
{
	string	l_str;
	uint32	l_ptrSeq;
	uint32	l_size;
	uint32	i;

	p_unzipFile ->openStream( "#docx.bin" );

	l_str = p_unzipFile ->readStr();
	if ( l_str != DOCX_TEMPLATE_VERSION )
		throw CompileVersionMismatchException( __FILE__, __LINE__ );

	p_unzipFile ->insertPtrBySeq( p_unzipFile ->readNum<uint32>(), this );

	m_altChunkDir	= p_unzipFile ->readStr();
	m_chartDir		= p_unzipFile ->readStr();
	m_hasToc		= p_unzipFile ->readNum<uint8>() != 0;

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_ptrSeq	= p_unzipFile ->readNum<uint32>();
		m_itemFiles.push_back( ( DocxMergerItemFile* ) l_ptrSeq );
	}

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_ptrSeq	= p_unzipFile ->readNum<uint32>();
		m_itemGroups.push_back( ( DocxMergerItemGroup* ) l_ptrSeq );
	}

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_str		= p_unzipFile ->readStr();
		l_ptrSeq	= p_unzipFile ->readNum<uint32>();
		m_itemsByName.insert( make_pair( l_str, ( DocxMergerItem* ) l_ptrSeq ) );
	}

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_str		= p_unzipFile ->readStr();
		l_ptrSeq	= p_unzipFile ->readNum<uint32>();
		m_fieldsByName.insert( make_pair( l_str, ( DocxMergerField* ) l_ptrSeq ) );
	}

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_str		= p_unzipFile ->readStr();
		l_ptrSeq	= p_unzipFile ->readNum<uint32>();
		m_headerFieldsByName.insert( make_pair( l_str, ( DocxMergerField* ) l_ptrSeq ) );
	}

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_ptrSeq	= p_unzipFile ->readNum<uint32>();
		m_xmlStrings.push_back( ( DocxMergerXmlString* ) l_ptrSeq );
	}

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_ptrSeq	= p_unzipFile ->readNum<uint32>();
		m_ids.push_back( ( DocxMergerId* ) l_ptrSeq );
	}

	p_unzipFile ->closeStream();
} // deserialize

void DocxMergerFile::deserializeSettingsFile( UnzipFile* p_unzipFile )
{
	m_settingsFile = new DocxMergerSettingsFile();

	p_unzipFile ->openStream( "#settingsfile.bin" );
	m_settingsFile ->deserialize( p_unzipFile );
	p_unzipFile ->closeStream();
} // deserializeSettingsFile

void DocxMergerFile::deserializeItemFiles( UnzipFile* p_unzipFile )
{
	DocxMergerItemFile*	l_itemFile;
	uint32				l_size;
	uint32				i;

	p_unzipFile ->openStream( "#itemfiles.bin" );

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_itemFile = new DocxMergerItemFile();
		l_itemFile ->deserialize( p_unzipFile );
	}

	p_unzipFile ->closeStream();
} // deserializeItemFiles

void DocxMergerFile::deserializeItemGroups( UnzipFile* p_unzipFile )
{
	DocxMergerItemGroup*	l_itemGroup;
	uint32					l_size;
	uint32					i;

	p_unzipFile ->openStream( "#itemgroups.bin" );

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_itemGroup = new DocxMergerItemGroup();
		l_itemGroup ->deserialize( p_unzipFile );
	}

	p_unzipFile ->closeStream();
} // deserializeItemGroups

void DocxMergerFile::deserializeItems( UnzipFile* p_unzipFile )
{
	DocxMergerItem*	l_item;
	uint32			l_size;
	uint32			i;

	p_unzipFile ->openStream( "#items.bin" );

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_item = new DocxMergerItem();
		l_item ->deserialize( p_unzipFile );
	}

	p_unzipFile ->closeStream();
} // deserializeItems

void DocxMergerFile::deserializeFields( UnzipFile* p_unzipFile )
{
	DocxMergerField*	l_field;
	int16				l_type;
	int16				l_chartType;

	uint32				l_size;
	uint32				i;

	p_unzipFile ->openStream( "#fields.bin" );

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_type = p_unzipFile ->readNum<int16>();
		switch ( ( DocxMergerField::FieldType ) l_type )
		{
		case DocxMergerField::TYPE_TEXT		: l_field = new DocxMergerTextField();		break;
		case DocxMergerField::TYPE_NUMBER	: l_field = new DocxMergerNumberField();	break;
		case DocxMergerField::TYPE_DATE_TIME: l_field = new DocxMergerDateTimeField();	break;
		case DocxMergerField::TYPE_BOOLEAN	: l_field = new DocxMergerBooleanField();	break;
		case DocxMergerField::TYPE_ALT_CHUNK: l_field = new DocxMergerAltChunkField();	break;
		case DocxMergerField::TYPE_PIC		: l_field = new DocxMergerPicField();		break;
		case DocxMergerField::TYPE_BARCODE	: l_field = new DocxMergerBarcodeField();	break;
		case DocxMergerField::TYPE_CHART	:

			l_chartType = p_unzipFile ->readNum<int16>();
			switch ( ( DocxMergerChartField::ChartType ) l_chartType )
			{
			case DocxMergerChartField::CHART_SERIES:		l_field = new DocxMergerChartFieldSeries();			break;
			case DocxMergerChartField::CHART_SINGLE_SERIES:	l_field = new DocxMergerChartFieldSingleSeries();	break;
			case DocxMergerChartField::CHART_FIXED_SERIES:	l_field = new DocxMergerChartFieldFixedSeries();	break;
			case DocxMergerChartField::CHART_X_Y:			l_field = new DocxMergerChartFieldXY();				break;
			case DocxMergerChartField::CHART_X_Y_SIZE:		l_field = new DocxMergerChartFieldXYSize();			break;
			}

			break;
		}
		
		l_field ->deserialize( p_unzipFile );
	}

	p_unzipFile ->closeStream();
} // deserializeFields

void DocxMergerFile::deserializeXmlStrings( UnzipFile* p_unzipFile )
{
	DocxMergerXmlString*	l_xmlString;
	uint32					l_size;
	uint32					i;

	p_unzipFile ->openStream( "#xmlstrings.bin" );

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_xmlString = new DocxMergerXmlString();
		l_xmlString ->deserialize( p_unzipFile );
	}

	p_unzipFile ->closeStream();
} // deserializeXmlStrings

void DocxMergerFile::deserializeIds( UnzipFile* p_unzipFile )
{
	DocxMergerId*	l_id;
	uint32			l_size;
	uint32			i;

	p_unzipFile ->openStream( "#ids.bin" );

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_id = new DocxMergerId();
		l_id ->deserialize( p_unzipFile );
	}

	p_unzipFile ->closeStream();
} // deserializeIds



void DocxMergerFile::link( UnzipFile* p_unzipFile )
{
	list<DocxMergerItemGroup*>::iterator			l_itemGroupIterator;
	list<DocxMergerItemFile*>::iterator				l_itemFileIterator;
	map<string, DocxMergerItem*>::iterator			l_itemIterator;
	multimap<string, DocxMergerField*>::iterator	l_fieldIterator;
	list<DocxMergerXmlString*>::iterator			l_xmlStringIterator;
	list<DocxMergerId*>::iterator					l_idIterator;

	const map<string, OpcPart*>*	l_partsByFullPath	= getPartsByFullPath();
	const map<uint32, void*>*		l_ptrsBySeq			= p_unzipFile ->getPtrsBySeq();
	uint32							l_ptrSeq;

	FOR_EACH( l_itemFileIterator, &m_itemFiles )
	{
		l_ptrSeq					= ( uint32 ) *l_itemFileIterator;
		*l_itemFileIterator			= ( DocxMergerItemFile* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;
	}

	FOR_EACH( l_itemGroupIterator, &m_itemGroups )
	{
		l_ptrSeq					= ( uint32 ) *l_itemGroupIterator;
		*l_itemGroupIterator		= ( DocxMergerItemGroup* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;
	}

	FOR_EACH( l_itemIterator, &m_itemsByName )
	{
		l_ptrSeq					= ( uint32 ) l_itemIterator ->second;
		l_itemIterator ->second		= ( DocxMergerItem* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;
	}

	FOR_EACH( l_fieldIterator, &m_fieldsByName )
	{
		l_ptrSeq					= ( uint32 ) l_fieldIterator ->second;
		l_fieldIterator ->second	= ( DocxMergerField* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;
	}

	FOR_EACH( l_fieldIterator, &m_headerFieldsByName )
	{
		l_ptrSeq					= ( uint32 ) l_fieldIterator ->second;
		l_fieldIterator ->second	= ( DocxMergerField* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;
	}

	FOR_EACH( l_xmlStringIterator, &m_xmlStrings )
	{
		l_ptrSeq					= ( uint32 ) *l_xmlStringIterator;
		*l_xmlStringIterator		= ( DocxMergerXmlString* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;
	}

	FOR_EACH( l_idIterator, &m_ids )
	{
		l_ptrSeq					= ( uint32 ) *l_idIterator;
		*l_idIterator				= ( DocxMergerId* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;
	}
} // link

void DocxMergerFile::linkSettingsFile( UnzipFile* p_unzipFile )
{
	m_settingsFile ->link( p_unzipFile );
} // linkSettingsFile

void DocxMergerFile::linkItemFiles( UnzipFile* p_unzipFile )
{
	list<DocxMergerItemFile*>::iterator l_itemFileIterator;
	FOR_EACH( l_itemFileIterator, &m_itemFiles )
	{
		( *l_itemFileIterator ) ->link( p_unzipFile );
	}
} // linkItemFiles

void DocxMergerFile::linkItemGroups( UnzipFile* p_unzipFile )
{
	list<DocxMergerItemGroup*>::iterator l_itemGroupIterator;
	FOR_EACH( l_itemGroupIterator, &m_itemGroups )
	{
		( *l_itemGroupIterator ) ->link( p_unzipFile );
	}
} // linkItemGroups

void DocxMergerFile::linkItems( UnzipFile* p_unzipFile )
{
	map<string, DocxMergerItem*>::iterator l_itemIterator;
	FOR_EACH( l_itemIterator, &m_itemsByName )
	{
		l_itemIterator ->second ->link( p_unzipFile );
	}
} // linkItems

void DocxMergerFile::linkFields( UnzipFile* p_unzipFile )
{
	multimap<string, DocxMergerField*>::iterator l_fieldIterator;
	FOR_EACH( l_fieldIterator, &m_fieldsByName )
	{
		l_fieldIterator ->second ->link( p_unzipFile );
	}
} // linkFields

void DocxMergerFile::linkXmlStrings( UnzipFile* p_unzipFile )
{
	list<DocxMergerXmlString*>::iterator l_xmlStringIterator;
	FOR_EACH( l_xmlStringIterator, &m_xmlStrings )
	{
		( *l_xmlStringIterator ) ->link( p_unzipFile );
	}
} // linkXmlStrings

void DocxMergerFile::linkIds( UnzipFile* p_unzipFile )
{
	list<DocxMergerId*>::iterator l_idIterator;
	FOR_EACH( l_idIterator, &m_ids )
	{
		( *l_idIterator ) ->link( p_unzipFile );
	}
} // linkIds



void DocxMergerFile::insertPasteFieldGroup( DocxMergerPasteFieldGroup* p_pasteFieldGroup )
{
	m_pasteFieldGroups.push_back( p_pasteFieldGroup );
} // insertPasteFieldGroup

unsigned int DocxMergerFile::nextAltChunkSeq()
{
	return ++m_altChunkSeq;
} // nextAltChunkSeq

string DocxMergerFile::getAltChunkDir() const
{
	return m_altChunkDir;
} // getAltChunkDir

unsigned int DocxMergerFile::nextChartSeq()
{
	return ++m_chartSeq;
} // nextChartSeq

string DocxMergerFile::getChartDir() const
{
	return m_chartDir;
} // getChartDir

size_t DocxMergerFile::nextIdSeq()
{
	return ++m_idSeq;
} // nextIdSeq

bool DocxMergerFile::hasToc() const
{
	return m_hasToc;
} // hasToc



const map<string, DocxMergerItem*>* DocxMergerFile::getItemsByName() const
{
	return &m_itemsByName;
} // getItemsByName

const multimap<string, DocxMergerField*>* DocxMergerFile::getFieldsByName() const
{
	return &m_fieldsByName;
} // getFieldsByName

const multimap<string, DocxMergerField*>* DocxMergerFile::getHeaderFieldsByName() const
{
	return &m_headerFieldsByName;
} // getHeaderFieldsByName
