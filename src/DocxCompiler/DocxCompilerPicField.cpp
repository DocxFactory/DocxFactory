
#include "DocxFactory/DocxCompiler/DocxCompilerItem.h"
#include "DocxFactory/DocxCompiler/DocxCompilerField.h"
#include "DocxFactory/DocxCompiler/DocxCompilerPicField.h"

#include "DocxFactory/parser/ParserFunc.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/xml/XmlFunc.h"

#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include "xercesc/dom/DOM.hpp"

#include <vector>

using namespace DocxFactory;
using namespace std;



void DocxCompilerPicField::getFormat( 
	string&						p_format,
	map<FieldParam, string>*	p_valueByParam )
{
 	map<FieldParam, string>::iterator l_valueByParamIterator;

	vector<string>	l_wordList;
	FieldParam		l_param;
	string			l_value;

	size_t			l_pos;
	size_t			l_len;
	FieldParam		l_useSize;

	string			l_str;
	size_t			i;

	p_valueByParam ->clear();

	if ( p_format.empty() )
		return;

	// backward compatibility with progress

	p_format = StrFunc::replace( StrFunc::replace( StrFunc::replace( StrFunc::replace( p_format,
		"use-image-size",	"UseImageFileSize" ),
		"keep-size-width",	"UsePlaceHolderWidth" ),
		"keep-size-height",	"UsePlaceHolderHeight" ),
		"keep-size",		"UsePlaceHolderSize" );

	try
	{	
		l_wordList  = ParserFunc::parseWordList( p_format, ParserFunc::SPACE_KEEP, false );
		p_format	= "";

		l_pos       = 0;
		l_len       = l_wordList.size();  

		_word:
		{
			while ( l_pos < l_len )
			{
				_param:
				{
					i = l_pos;

					if ( i != 0 && StrFunc::trim( l_wordList[i] ).empty() )
						i++;

					if ( i >= l_len )
						goto _paramEnd;

					l_param	= strToParam( l_wordList[i] );

					i++;



					_value:
					{
						l_value  = "";
						l_pos    = i;

						if ( i >= l_len )
							goto _valueEnd;

						if ( StrFunc::trim( l_wordList[i] ).empty() )
							i++;

						if ( i >= l_len )
							goto _valueEnd;

						if ( l_wordList[i] != ":" )
							goto _valueEnd;

						i++;

						if ( i >= l_len )
							goto _valueEnd;

						if ( StrFunc::trim( l_wordList[i] ).empty() )
							i++;

						if ( i >= l_len )
							goto _valueEnd;

						l_str = l_wordList[i];

						if ( l_str.length() > 0 && ( l_str[0] == '"' || l_str[0] == '\'' ) )
							 l_str = ParserFunc::unquote( l_str );

						i++;

						l_value  = l_str;
						l_pos    = i;
					}

					_valueEnd:

					p_valueByParam ->insert( make_pair( l_param, l_value ) );

					goto _word;
				}

				_paramEnd:

				while ( l_pos < i )
				{
					p_format += l_wordList[ l_pos ];
					l_pos++;
				}
			}	
		}

		p_format = StrFunc::trim( p_format );
	}

	catch ( ... )
	{

	}



	l_useSize = PARAM_UNDEFINED;

	FOR_EACH( l_valueByParamIterator, p_valueByParam )
	{
		l_param = l_valueByParamIterator ->first;

		switch ( l_param )
		{
		case PARAM_USE_IMAGE_FILE_SIZE		:
		case PARAM_USE_PLACE_HOLDER_WIDTH	:
		case PARAM_USE_PLACE_HOLDER_HEIGHT	:
		case PARAM_USE_PLACE_HOLDER_SIZE	:

			if ( l_useSize == PARAM_UNDEFINED )
				 l_useSize = l_param;

			break;
		}

		if ( l_useSize == PARAM_UNDEFINED )
			 l_useSize =  PARAM_USE_IMAGE_FILE_SIZE;
	}

	p_format = "";
	p_valueByParam ->clear();

	if ( l_useSize!= PARAM_UNDEFINED )
		p_valueByParam ->insert( make_pair( l_useSize, "" ) );
} // getFormat



DocxCompilerPicField::DocxCompilerPicField(
	DocxCompilerItem*				p_item,
	const string&					p_name,
	const string&					p_format,
	const map<FieldParam, string>*	p_valueByParam,
	xercesc::DOMElement*			p_placeHolderNode,
	xercesc::DOMElement*			p_drawingNode )

	: DocxCompilerField(
		p_item,
		TYPE_PIC,
		p_name,
		p_format,
		p_valueByParam,
		p_placeHolderNode )
{
	map<FieldParam, string>::const_iterator	l_valueByParamIterator;
	FieldParam								l_param;

	xercesc::DOMNamedNodeMap*	l_attributes;
	xercesc::DOMAttr*			l_attrNode;
	XMLSize_t					l_pos;
	XMLSize_t					l_len;

	xercesc::DOMNode*			l_cursorNode	= p_drawingNode;
	XmlFunc::TagPos				l_cursorPos		= XmlFunc::TAG_BEFORE;
	string						l_str;
	string						l_picString		= "";

	m_picId		= 0;
	m_emuWidth	= 0;
	m_emuHeight	= 0;
	m_fileRId	= "";

	while ( XmlFunc::getNextTag( p_drawingNode, l_cursorNode, l_cursorPos, l_str ) )
	{
		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
		  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "blip" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/drawingml/2006/main" ) )
		  && ( l_cursorPos == XmlFunc::TAG_EMPTY || l_cursorPos == XmlFunc::TAG_START ) )
		{
			l_picString += "<" + XmlFunc::XMLChToUtf8( l_cursorNode ->getNodeName() );

			l_attributes	= l_cursorNode ->getAttributes();
			l_len			= l_attributes ->getLength();

			for ( l_pos = 0; l_pos < l_len; l_pos++ )
			{
				l_attrNode = ( xercesc::DOMAttr* ) l_attributes ->item( l_pos );

				if ( ( XmlFunc::XMLChCmp( l_attrNode ->getLocalName(),		_X( "embed" ) )
					|| XmlFunc::XMLChCmp( l_attrNode ->getLocalName(),		_X( "link" ) ) )
					&& XmlFunc::XMLChCmp( l_attrNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/officeDocument/2006/relationships" ) ) )
				{
					m_fileRId	 = XmlFunc::XMLChToUtf8( l_attrNode ->getNodeValue() );
					l_picString += " ";

					m_picStrings.push_back( make_pair( l_picString, 'r' ) );
					l_picString	 = "";
				}

				else
					l_picString += " " 
						+ XmlFunc::XMLChToUtf8( l_attrNode ->getNodeName() ) + "=\"" 
						+ XmlFunc::XMLChToUtf8( l_attrNode ->getNodeValue() ) + "\"";
			}

			l_picString += ( l_cursorPos == XmlFunc::TAG_START ? ">" : "/>" );
		}

		else
		if ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
		  && XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),		_X( "docPr" ) )
		  && XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing" ) )
		  && ( l_cursorPos == XmlFunc::TAG_EMPTY || l_cursorPos == XmlFunc::TAG_START ) )
		{
			l_picString += "<" + XmlFunc::XMLChToUtf8( l_cursorNode ->getNodeName() );

			l_attributes	= l_cursorNode ->getAttributes();
			l_len			= l_attributes ->getLength();

			for ( l_pos = 0; l_pos < l_len; l_pos++ )
			{
				l_attrNode = ( xercesc::DOMAttr* ) l_attributes ->item( l_pos );

				if ( XmlFunc::XMLChCmp( l_attrNode ->getLocalName(), _X( "id" ) ) )
				{
					m_picId		 = StrFunc::strToInt<unsigned short>( XmlFunc::XMLChToUtf8( l_attrNode ->getNodeValue() ) );
					l_picString	+= string( " " + XmlFunc::XMLChToUtf8( l_attrNode ->getNodeName() ) + "=\"" );

					m_picStrings.push_back( make_pair( l_picString, 'i' ) );
					l_picString	 = "\"";
				}

				else
					l_picString	+= " "
						+ XmlFunc::XMLChToUtf8( l_attrNode ->getNodeName() ) + "=\""
						+ XmlFunc::XMLChToUtf8( l_attrNode ->getNodeValue() ) + "\"";
			}

			l_picString += ( l_cursorPos == XmlFunc::TAG_START ? ">" : "/>" );
		}

		else
		if ( ( l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
			&& XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),	_X( "extent" ) )
			&& XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing" ) )

			|| l_cursorNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE
			&& XmlFunc::XMLChCmp( l_cursorNode ->getLocalName(),	_X( "ext" ) )
			&& XmlFunc::XMLChCmp( l_cursorNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/drawingml/2006/main" ) ) )

			&& ( l_cursorPos == XmlFunc::TAG_EMPTY || l_cursorPos == XmlFunc::TAG_START ) )
		{
			l_picString += "<" + XmlFunc::XMLChToUtf8( l_cursorNode ->getNodeName() );

			l_attributes	= l_cursorNode ->getAttributes();
			l_len			= l_attributes ->getLength();

			for ( l_pos = 0; l_pos < l_len; l_pos++ )
			{
				l_attrNode = ( xercesc::DOMAttr* ) l_attributes ->item( l_pos );

				if ( XmlFunc::XMLChCmp( l_attrNode ->getLocalName(), _X( "cx" ) ) )
				{
					m_emuWidth	 = StrFunc::strToInt<unsigned long>( XmlFunc::XMLChToUtf8( l_attrNode ->getNodeValue() ) );
					l_picString	+= " " + XmlFunc::XMLChToUtf8( l_attrNode ->getNodeName() ) + "=\"";
					
					m_picStrings.push_back( make_pair( l_picString, 'x' ) );
					l_picString	 = "\"";
				}

				else
				if ( XmlFunc::XMLChCmp( l_attrNode ->getLocalName(), _X( "cy" ) ) )
				{
					m_emuHeight = StrFunc::strToInt<unsigned long>( XmlFunc::XMLChToUtf8( l_attrNode ->getNodeValue() ) );
					l_picString	+= " " + XmlFunc::XMLChToUtf8( l_attrNode ->getNodeName() ) + "=\"";
					
					m_picStrings.push_back( make_pair( l_picString, 'y' ) );
					l_picString	 = "\"";
				}

				else
					l_picString += " "
						+ XmlFunc::XMLChToUtf8( l_attrNode ->getNodeName() ) + "=\""
						+ XmlFunc::XMLChToUtf8( l_attrNode ->getNodeValue() ) + "\"";
			}

			l_picString += ( l_cursorPos == XmlFunc::TAG_START ? ">" : "/>" );
		}

		else
			l_picString += l_str;
	}

	if ( !l_picString.empty() )
		  m_picStrings.push_back( make_pair( l_picString, NULL ) );

	FOR_EACH( l_valueByParamIterator, p_valueByParam )
	{
		l_param = l_valueByParamIterator ->first;

		switch ( l_param )
		{
		case PARAM_USE_IMAGE_FILE_SIZE		: m_useSize = USE_IMAGE_FILE_SIZE;		break;
		case PARAM_USE_PLACE_HOLDER_WIDTH	: m_useSize = USE_PLACE_HOLDER_WIDTH;	break;
		case PARAM_USE_PLACE_HOLDER_HEIGHT	: m_useSize = USE_PLACE_HOLDER_HEIGHT;	break;
		case PARAM_USE_PLACE_HOLDER_SIZE	: m_useSize = USE_PLACE_HOLDER_SIZE;	break;
		}
	}
} // c'tor

DocxCompilerPicField::~DocxCompilerPicField()
{

} // d'tor



void DocxCompilerPicField::serialize( ZipFile* p_zipFile )
{
	list<pair<string, char>>::iterator l_picStringIterator;

	DocxCompilerField::serialize( p_zipFile );

	p_zipFile ->writeNum<uint32>( m_picId );
	p_zipFile ->writeNum<uint32>( m_emuWidth );
	p_zipFile ->writeNum<uint32>( m_emuHeight );
	p_zipFile ->writeNum<int16>	( m_useSize );

	p_zipFile ->writeNum<uint32>( ( uint32 ) m_picStrings.size() );
	FOR_EACH( l_picStringIterator, &m_picStrings )
	{
		p_zipFile ->writeStr		( ( *l_picStringIterator ).first );
		p_zipFile ->writeNum<uint8>	( ( *l_picStringIterator ).second );
	}
} // serialize



string DocxCompilerPicField::getFileRId() const
{
	return m_fileRId;
} // getFileRId
