
#include "DocxFactory/DocxCompiler/DocxCompilerItemFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItem.h"
#include "DocxFactory/DocxCompiler/DocxCompilerField.h"
#include "DocxFactory/DocxCompiler/DocxCompilerBarcodeField.h"

#include "DocxFactory/barcode/BarcodeFormat.h"

#include "DocxFactory/xml/XmlFunc.h"
#include "DocxFactory/xml/XmlTreeDriller.h"
#include "DocxFactory/xml/Utf8ToXMLCh.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include "zint/zint.h"

#include "xercesc/dom/DOM.hpp"

#include "boost/scoped_ptr.hpp"

using namespace DocxFactory;
using namespace std;



DocxCompilerBarcodeField::DocxCompilerBarcodeField(
	DocxCompilerItem*				p_item,
	const string&					p_name,
	const string&					p_format,
	const map<FieldParam, string>*	p_valueByParam,
	xercesc::DOMElement*			p_placeHolderNode )

	: DocxCompilerField (
		p_item,
		TYPE_BARCODE,
		p_name,
		p_format,
		p_valueByParam,
		p_placeHolderNode )
{
	map<FieldParam, string>::const_iterator	l_valueByParamIterator;
	FieldParam								l_param;
	string									l_value;

	int						l_checkDigit;
	int						l_cols;
	int						l_channels;
	int						l_size;
	int						l_mode;
	int						l_security;
	bool					l_square;

	xercesc::DOMElement*	l_cursorNode;
	unsigned short			l_sz;
	unsigned short			l_szCs;
	string					l_fgcolor;
	string					l_bgcolor;

	string					l_wordMlPrefix = getItem() ->getItemFile() ->getWordMlPrefix();
	xercesc::DOMElement*	l_tcNode;
	xercesc::DOMElement*	l_tcPrNode;
	xercesc::DOMElement*	l_pNode;
	xercesc::DOMElement*	l_pPrNode;
	xercesc::DOMElement*	l_rNode;
	xercesc::DOMElement*	l_rPrNode;
	xercesc::DOMElement*	l_tNode;
	xercesc::DOMElement*	l_tTempNode;

	xercesc::DOMNode*		l_node;
	string					l_nodeName;
	xercesc::DOMAttr*		l_attrNode;
	string					l_attrValue;

	l_tNode		= ( xercesc::DOMElement* ) p_placeHolderNode ->getParentNode();
	l_rNode		= ( xercesc::DOMElement* ) l_tNode ->getParentNode();
	l_rPrNode	= ( xercesc::DOMElement* ) l_rNode ->getFirstChild();

	if ( l_tNode ->getChildNodes() ->getLength() == 1 )
	{
		l_tNode ->removeChild( p_placeHolderNode );
		l_rNode	->insertBefore( p_placeHolderNode, l_tNode );
		l_rNode ->removeChild( l_tNode );
	}

	else
	if ( p_placeHolderNode == l_tNode ->getFirstChild() )
	{
		l_tNode ->removeChild( p_placeHolderNode );
		l_rNode ->insertBefore( p_placeHolderNode, l_tNode );
	}

	else
	if ( p_placeHolderNode == l_tNode ->getLastChild() )
	{
		l_tNode ->removeChild( p_placeHolderNode );

		if ( l_tNode == l_rNode ->getLastChild() )
			l_rNode ->appendChild( p_placeHolderNode );

		else
		{
			l_node = l_tNode ->getNextSibling();
			l_rNode ->insertBefore( p_placeHolderNode, l_node );
		}
	}

	else
	{
		l_tTempNode = ( xercesc::DOMElement* ) l_tNode ->cloneNode( false );
		l_rNode ->insertBefore( l_tTempNode, l_tNode );
		
		while ( p_placeHolderNode != l_tNode ->getFirstChild() )
		{
			l_node = p_placeHolderNode ->getPreviousSibling();
			l_tNode ->removeChild( l_node );

			if( l_tTempNode ->getFirstChild() )
				l_tTempNode ->insertBefore( l_node, l_tTempNode ->getFirstChild() );
			else
				l_tTempNode ->appendChild( l_node );
		}

		l_tNode ->removeChild( p_placeHolderNode );
		l_rNode ->insertBefore( p_placeHolderNode, l_tNode );
	}



	m_barcodeFormat	= new BarcodeFormat;
	m_barcodeFormat ->m_symbology			= BARCODE_CODE128;
	m_barcodeFormat ->m_symbologyName		= "";
	m_barcodeFormat ->m_option1				= -1;
	m_barcodeFormat ->m_option2				= -1;
	m_barcodeFormat ->m_option3				= -1;
	m_barcodeFormat ->m_inputMode			= UNICODE_MODE;

	m_barcodeFormat ->m_scale				= 0.0f;
	m_barcodeFormat ->m_height				= 0;
	m_barcodeFormat ->m_showHrt				= 0;
	m_barcodeFormat ->m_angle				= 0;
	m_barcodeFormat ->m_fontSize			= 11;
	m_barcodeFormat ->m_font				= "Calibri";
	m_barcodeFormat ->m_fgcolor				= "";
	m_barcodeFormat ->m_bgcolor				= "";
	m_barcodeFormat ->m_whiteSpaceWidth		= 0;
	m_barcodeFormat ->m_borderWidth			= 0;
	m_barcodeFormat ->m_outputOptions		= -1;

	l_checkDigit	= -1;
	l_cols			= -1;
	l_channels		= -1;
	l_size			= -1;
	l_mode			= -1;
	l_security		= -1;
	l_square		= false;

	FOR_EACH( l_valueByParamIterator, p_valueByParam )
	{
		l_param = l_valueByParamIterator ->first;
		l_value = l_valueByParamIterator ->second;

		switch ( l_param )
		{
		case PARAM_CODE11:				m_barcodeFormat ->m_symbology = BARCODE_CODE11;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CODE25:				m_barcodeFormat ->m_symbology = BARCODE_C25MATRIX;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CODE25_IATA:			m_barcodeFormat ->m_symbology = BARCODE_C25IATA;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CODE25_INTER:		m_barcodeFormat ->m_symbology = BARCODE_C25INTER;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CODE25_LOGIC:		m_barcodeFormat ->m_symbology = BARCODE_C25LOGIC;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CODE25_IND:			m_barcodeFormat ->m_symbology = BARCODE_C25IND;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_ITF14:				m_barcodeFormat ->m_symbology = BARCODE_ITF14;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CODE39:				m_barcodeFormat ->m_symbology = BARCODE_CODE39;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CODE39_EXT:			m_barcodeFormat ->m_symbology = BARCODE_EXCODE39;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CODE93:				m_barcodeFormat ->m_symbology = BARCODE_CODE93;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_PZN:					m_barcodeFormat ->m_symbology = BARCODE_PZN;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_LOGMARS:				m_barcodeFormat ->m_symbology = BARCODE_LOGMARS;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CODE32:				m_barcodeFormat ->m_symbology = BARCODE_CODE32;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CODE128:				m_barcodeFormat ->m_symbology = BARCODE_CODE128;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CODE128B:			m_barcodeFormat ->m_symbology = BARCODE_CODE128B;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_EAN128:				m_barcodeFormat ->m_symbology = BARCODE_EAN128;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_EAN14:				m_barcodeFormat ->m_symbology = BARCODE_EAN14;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_NVE18:				m_barcodeFormat ->m_symbology = BARCODE_NVE18;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_UPCA:				m_barcodeFormat ->m_symbology = BARCODE_UPCA;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_UPCE:				m_barcodeFormat ->m_symbology = BARCODE_UPCE;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_EAN:					m_barcodeFormat ->m_symbology = BARCODE_EANX;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_ISBN:				m_barcodeFormat ->m_symbology = BARCODE_ISBNX;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CODABAR:				m_barcodeFormat ->m_symbology = BARCODE_CODABAR;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_PHARMA:				m_barcodeFormat ->m_symbology = BARCODE_PHARMA;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_PHARMA_2TRACK:		m_barcodeFormat ->m_symbology = BARCODE_PHARMA_TWO;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_PLESSEY:				m_barcodeFormat ->m_symbology = BARCODE_PLESSEY;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_MSI_PLESSEY:			m_barcodeFormat ->m_symbology = BARCODE_MSI_PLESSEY;		m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_TELEPEN:				m_barcodeFormat ->m_symbology = BARCODE_TELEPEN;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_TELEPEN_NUM:			m_barcodeFormat ->m_symbology = BARCODE_TELEPEN_NUM;		m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_RSS14:				m_barcodeFormat ->m_symbology = BARCODE_RSS14;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_RSS14_STACK:			m_barcodeFormat ->m_symbology = BARCODE_RSS14STACK;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_RSS14_STACKOMNI:		m_barcodeFormat ->m_symbology = BARCODE_RSS14STACK_OMNI;	m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_RSS_LTD:				m_barcodeFormat ->m_symbology = BARCODE_RSS_LTD;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_RSS_EXP:				m_barcodeFormat ->m_symbology = BARCODE_RSS_EXP;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_RSS_EXPSTACK:		m_barcodeFormat ->m_symbology = BARCODE_RSS_EXPSTACK;		m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CHANNEL:				m_barcodeFormat ->m_symbology = BARCODE_CHANNEL;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_FIM:					m_barcodeFormat ->m_symbology = BARCODE_FIM;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_FLAT:				m_barcodeFormat ->m_symbology = BARCODE_FLAT;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_DAFT:				m_barcodeFormat ->m_symbology = BARCODE_DAFT;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_POSTNET:				m_barcodeFormat ->m_symbology = BARCODE_POSTNET;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_PLANET:				m_barcodeFormat ->m_symbology = BARCODE_PLANET;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_ONECODE:				m_barcodeFormat ->m_symbology = BARCODE_ONECODE;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_RM4SCC:				m_barcodeFormat ->m_symbology = BARCODE_RM4SCC;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_DPLEIT:				m_barcodeFormat ->m_symbology = BARCODE_DPLEIT;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_DPIDENT:				m_barcodeFormat ->m_symbology = BARCODE_DPIDENT;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_AUSPOST:				m_barcodeFormat ->m_symbology = BARCODE_AUSPOST;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_AUSREPLY:			m_barcodeFormat ->m_symbology = BARCODE_AUSREPLY;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_AUSROUTE:			m_barcodeFormat ->m_symbology = BARCODE_AUSROUTE;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_AUSREDIRECT:			m_barcodeFormat ->m_symbology = BARCODE_AUSREDIRECT;		m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_KIX:					m_barcodeFormat ->m_symbology = BARCODE_KIX;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_JAPANPOST:			m_barcodeFormat ->m_symbology = BARCODE_JAPANPOST;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_KOREAPOST:			m_barcodeFormat ->m_symbology = BARCODE_KOREAPOST;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CODE16K:				m_barcodeFormat ->m_symbology = BARCODE_CODE16K;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CODE49:				m_barcodeFormat ->m_symbology = BARCODE_CODE49;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_PDF417:				m_barcodeFormat ->m_symbology = BARCODE_PDF417;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_PDF417_TRUNC:		m_barcodeFormat ->m_symbology = BARCODE_PDF417TRUNC;		m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_MICROPDF:			m_barcodeFormat ->m_symbology = BARCODE_MICROPDF417;		m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_DATAMATRIX:			m_barcodeFormat ->m_symbology = BARCODE_DATAMATRIX;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_QRCODE:				m_barcodeFormat ->m_symbology = BARCODE_QRCODE;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_MICROQR:				m_barcodeFormat ->m_symbology = BARCODE_MICROQR;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_MAXICODE:			m_barcodeFormat ->m_symbology = BARCODE_MAXICODE;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_AZTEC:				m_barcodeFormat ->m_symbology = BARCODE_AZTEC;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_AZRUNE:				m_barcodeFormat ->m_symbology = BARCODE_AZRUNE;				m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CODEONE:				m_barcodeFormat ->m_symbology = BARCODE_CODEONE;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_GRIDMATRIX:			m_barcodeFormat ->m_symbology = BARCODE_GRIDMATRIX;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_HIBC_CODE39:			m_barcodeFormat ->m_symbology = BARCODE_HIBC_39;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_HIBC_CODE128:		m_barcodeFormat ->m_symbology = BARCODE_HIBC_128;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_HIBC_PDF417:			m_barcodeFormat ->m_symbology = BARCODE_HIBC_PDF;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_HIBC_MICROPDF:		m_barcodeFormat ->m_symbology = BARCODE_HIBC_MICPDF;		m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_HIBC_DATAMATRIX:		m_barcodeFormat ->m_symbology = BARCODE_HIBC_DM;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_HIBC_QRCODE:			m_barcodeFormat ->m_symbology = BARCODE_HIBC_QR;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_HIBC_AZTEC:			m_barcodeFormat ->m_symbology = BARCODE_HIBC_AZTEC;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CS_EAN:				m_barcodeFormat ->m_symbology = BARCODE_EANX_CC;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CS_EAN128:			m_barcodeFormat ->m_symbology = BARCODE_EAN128_CC;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CS_UPCA:				m_barcodeFormat ->m_symbology = BARCODE_UPCA_CC;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CS_UPCE:				m_barcodeFormat ->m_symbology = BARCODE_UPCE_CC;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CS_RSS14:			m_barcodeFormat ->m_symbology = BARCODE_RSS14_CC;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CS_RSS14_STACK:		m_barcodeFormat ->m_symbology = BARCODE_RSS14STACK_CC;		m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CS_RSS14_STACKOMNI:	m_barcodeFormat ->m_symbology = BARCODE_RSS14_OMNI_CC;		m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CS_RSS_LTD:			m_barcodeFormat ->m_symbology = BARCODE_RSS_LTD_CC;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CS_RSS_EXP:			m_barcodeFormat ->m_symbology = BARCODE_RSS_EXP_CC;			m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;
		case PARAM_CS_RSS_EXPSTACK:		m_barcodeFormat ->m_symbology = BARCODE_RSS_EXPSTACK_CC;	m_barcodeFormat ->m_symbologyName = DocxCompilerField::paramToStr( l_param ); break;

		case PARAM_CHECK_DIGIT:

			l_checkDigit = StrFunc::strToInt<int>( l_value );
			break;

		case PARAM_COLS:

			l_cols = StrFunc::strToInt<int>( l_value );
			break;

		case PARAM_CHANNELS:

			l_channels = StrFunc::strToInt<int>( l_value );
			break;

		case PARAM_SIZE:

			l_size = StrFunc::strToInt<int>( l_value );
			break;

		case PARAM_MODE:

			l_mode = StrFunc::strToInt<int>( l_value );
			break;

		case PARAM_SECURITY:

			l_security = StrFunc::strToInt<int>( l_value );
			break;

		case PARAM_SQUARE:

			l_square = true;
			break;

		case PARAM_GS1:

			m_barcodeFormat ->m_inputMode = GS1_MODE;
			break;

		case PARAM_SCALE:

			m_barcodeFormat ->m_scale = StrFunc::strToInt<float>( l_value );
			break;

		case PARAM_HEIGHT:

			m_barcodeFormat ->m_height = StrFunc::strToInt<int>( l_value );
			break;

		case PARAM_SHOW_VALUE:

			m_barcodeFormat ->m_showHrt = 1;
			break;

		case PARAM_ANGLE:

			m_barcodeFormat ->m_angle = StrFunc::strToInt<int>( l_value );
			break;

		case PARAM_FONT_SIZE:

			m_barcodeFormat ->m_fontSize = StrFunc::strToInt<int>( l_value );
			break;

		case PARAM_FONT:

			m_barcodeFormat ->m_font = l_value;
			break;

		case PARAM_FGCOLOR:

			m_barcodeFormat ->m_fgcolor = l_value;
			break;

		case PARAM_BGCOLOR:

			m_barcodeFormat ->m_bgcolor = l_value;
			break;

		case PARAM_WHITESPACE_WIDTH:

			m_barcodeFormat ->m_whiteSpaceWidth = StrFunc::strToInt<int>( l_value );
			break;

		case PARAM_BORDER_WIDTH:

			m_barcodeFormat ->m_borderWidth = StrFunc::strToInt<int>( l_value );
			break;

		case PARAM_BIND:

			m_barcodeFormat ->m_outputOptions = BARCODE_BIND;
			break;

		case PARAM_BOX:

			m_barcodeFormat ->m_outputOptions = BARCODE_BOX;
			break;
		}
	}



	l_rNode		= ( xercesc::DOMElement* ) p_placeHolderNode ->getParentNode();
	l_rPrNode	= ( xercesc::DOMElement* ) l_rNode ->getFirstChild();

	l_sz		= 0;
	l_szCs		= 0;
	l_fgcolor	= "";
	l_bgcolor	= "";

	boost::scoped_ptr<XmlTreeDriller> l_treeDriller( new XmlTreeDriller(
		l_rPrNode,
		"http://schemas.openxmlformats.org/wordprocessingml/2006/main", "rPr",
		"http://schemas.openxmlformats.org/wordprocessingml/2006/main",	"sz,szCs,color,highlight",
		"http://schemas.openxmlformats.org/wordprocessingml/2006/main", "val", "" ) );

	while ( l_cursorNode = ( xercesc::DOMElement* ) l_treeDriller ->nextNode() )
	{
		l_nodeName	= XmlFunc::XMLChToUtf8( l_cursorNode ->getLocalName() );
		l_attrNode	= l_cursorNode ->getAttributeNodeNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X ( "val") );
		l_attrValue	= XmlFunc::XMLChToUtf8( l_attrNode ->getValue() );

			 if ( l_nodeName == "sz" )			l_sz		= StrFunc::strToInt<unsigned short>( l_attrValue );
		else if ( l_nodeName == "szCs" )		l_szCs		= StrFunc::strToInt<unsigned short>( l_attrValue );
		else if ( l_nodeName == "color" )		l_fgcolor	= StrFunc::normalizeColor( l_attrValue );
		else if ( l_nodeName == "highlight" )	l_bgcolor	= StrFunc::normalizeColor( l_attrValue );
	}

	if ( l_sz == 0 || l_szCs == 0 || l_fgcolor == "" )
	{
		l_pNode		= ( xercesc::DOMElement* ) l_rNode ->getParentNode();
		l_pPrNode	= ( xercesc::DOMElement* ) l_pNode ->getFirstChild();

		l_treeDriller.reset( new XmlTreeDriller(
			l_pPrNode,
			"http://schemas.openxmlformats.org/wordprocessingml/2006/main", "rPr",
			"http://schemas.openxmlformats.org/wordprocessingml/2006/main",	"sz,szCs,color,highlight",
			"http://schemas.openxmlformats.org/wordprocessingml/2006/main", "val", "" ) );

		while ( l_cursorNode = ( xercesc::DOMElement* ) l_treeDriller ->nextNode() )
		{
			l_nodeName	= XmlFunc::XMLChToUtf8( l_cursorNode ->getLocalName() );
			l_attrNode	= l_cursorNode ->getAttributeNodeNS( _X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ), _X ( "val") );
			l_attrValue	= XmlFunc::XMLChToUtf8( l_attrNode ->getValue() );

				 if ( l_nodeName == "sz" )			{ if ( l_sz == 0 )			l_sz		= StrFunc::strToInt<unsigned short>( l_attrValue ); }
			else if ( l_nodeName == "szCs" )		{ if ( l_szCs == 0 )		l_szCs		= StrFunc::strToInt<unsigned short>( l_attrValue ); }
			else if ( l_nodeName == "color" )		{ if ( l_fgcolor.empty() )	l_fgcolor	= StrFunc::normalizeColor( l_attrValue ); }
			else if ( l_nodeName == "highlight" )	{ if ( l_bgcolor.empty() )	l_bgcolor	= StrFunc::normalizeColor( l_attrValue ); }
		}
	}

	if ( l_bgcolor.empty() )
	{
		l_pNode		= ( xercesc::DOMElement* ) l_rNode ->getParentNode();
		l_tcNode	= ( xercesc::DOMElement* ) l_pNode ->getParentNode();

		if ( l_tcNode ->getNodeType() == xercesc::DOMNode::ELEMENT_NODE 
		  && XmlFunc::XMLChCmp( l_tcNode ->getLocalName(),		_X( "tc" ) )
		  && XmlFunc::XMLChCmp( l_tcNode ->getNamespaceURI(),	_X( "http://schemas.openxmlformats.org/wordprocessingml/2006/main" ) ) )
		{
			l_tcPrNode = ( xercesc::DOMElement* ) l_tcNode ->getFirstChild();

			l_cursorNode = XmlFunc::getChildByTagName(
				l_tcPrNode,
				NULL,
				"http://schemas.openxmlformats.org/wordprocessingml/2006/main",
				"shd" );

			if( l_cursorNode && l_cursorNode ->hasAttribute( Utf8ToXMLCh( l_wordMlPrefix + ":fill" ) ) )
				l_bgcolor = StrFunc::normalizeColor( XmlFunc::XMLChToUtf8( l_cursorNode ->getAttribute( Utf8ToXMLCh( l_wordMlPrefix + ":fill" ) ) ) );
		}
	}

	if ( l_sz == 0 && l_szCs == 0 )
		 l_sz = 22;

	if ( m_barcodeFormat ->m_scale == 0.0f )
	{
			 if ( l_sz != 0 )	m_barcodeFormat ->m_scale = ( float ) l_sz / 60.0f;
		else if ( l_szCs != 0 )	m_barcodeFormat ->m_scale = ( float ) l_szCs / 60.0f;
	}

	if ( m_barcodeFormat ->m_fgcolor.empty() )
	{
		if ( !l_fgcolor.empty() )
			m_barcodeFormat ->m_fgcolor = l_fgcolor;
	}

	if ( m_barcodeFormat ->m_bgcolor.empty()

	  /* if the bgcolor is the same as the background then it is only needed for maxicode which must have a bgcolor
	     so if the bgcolor is taken from the document then it is only needed for maxicode */

	  && m_barcodeFormat ->m_symbology == BARCODE_MAXICODE )
	{
		if ( !l_bgcolor.empty() )
			m_barcodeFormat ->m_bgcolor = l_bgcolor;
	}



	switch ( m_barcodeFormat ->m_symbology )
	{
	case BARCODE_CODE39:
	case BARCODE_EXCODE39:

		if ( l_checkDigit != -1 )
			m_barcodeFormat ->m_option2 = 1;
		
		else
			m_barcodeFormat ->m_option2 = 0;

		break;

	case BARCODE_MSI_PLESSEY:

		if ( l_checkDigit >= 0 && l_checkDigit <= 4 )
			m_barcodeFormat ->m_option2 = l_checkDigit;

		break;

	case BARCODE_RSS_EXPSTACK:
	case BARCODE_RSS_EXPSTACK_CC:

		if ( l_cols >= 1 && l_cols <= 9 )
			m_barcodeFormat ->m_option2 = l_cols;

		else
			m_barcodeFormat ->m_option2 = 0;

		break;

	case BARCODE_CHANNEL:

		if ( l_channels >= 3 && l_channels <= 8 )
			m_barcodeFormat ->m_option2 = l_channels;

		else
			m_barcodeFormat ->m_option2 = 0;

		break;

	case BARCODE_PDF417:
	case BARCODE_PDF417TRUNC:
	case BARCODE_HIBC_PDF:

		if ( l_cols >= 1 && l_cols <= 20 )
			m_barcodeFormat ->m_option2 = l_cols;

		else
			m_barcodeFormat ->m_option2 = 0;

		if ( l_security >= 0 && l_security <= 8 )
			m_barcodeFormat ->m_option1 = l_security;

		break;

	case BARCODE_MICROPDF417:
	case BARCODE_HIBC_MICPDF:

		if ( l_cols >= 1 && l_cols <= 4 )
			m_barcodeFormat ->m_option2 = l_cols;

		break;

	case BARCODE_MAXICODE:

		if ( l_mode >= 2 && l_mode <= 6 )
			m_barcodeFormat ->m_option1 = l_mode;

		else
			m_barcodeFormat ->m_option1 = 4;

		break;

	case BARCODE_QRCODE:
	case BARCODE_HIBC_QR:

		if ( l_size >= 1 && l_size <= 20 )
			m_barcodeFormat ->m_option2 = l_size;
		
		else
			m_barcodeFormat ->m_option2 = 0;

		if ( l_security >= 1 && l_security <= 4 )
			m_barcodeFormat ->m_option1 = l_security;

		else
			m_barcodeFormat ->m_option1 = 1;

		break;

	case BARCODE_MICROQR:

		if ( l_size >= 1 && l_size <= 4 )
			m_barcodeFormat ->m_option2 = l_size;
		
		else
			m_barcodeFormat ->m_option2 = 0;

		if ( l_security >= 1 && l_security <= 3 )
			m_barcodeFormat ->m_option1 = l_security;

		else
			m_barcodeFormat ->m_option1 = 1;

		break;

	case BARCODE_DATAMATRIX:
	case BARCODE_HIBC_DM:

		if ( l_size >= 1 && l_size <= 15 )
			m_barcodeFormat ->m_option2 = l_size;
		
		else
			m_barcodeFormat ->m_option2 = 0;

		if ( l_square )
			m_barcodeFormat ->m_option3 = DM_SQUARE;

		break;

	case BARCODE_AZTEC:
	case BARCODE_HIBC_AZTEC:

		if ( l_size >= 1 && l_size <= 15 )
			m_barcodeFormat ->m_option2 = l_size;

		else
			m_barcodeFormat ->m_option2 = 0;

		if ( l_security >= 1 && l_security <= 4 )
			m_barcodeFormat ->m_option1 = l_security;

		else
			m_barcodeFormat ->m_option1 = 1;

		break;

	case BARCODE_CODEONE:

		if ( l_size >= 1 && l_size <= 10 )
			m_barcodeFormat ->m_option2 = l_size;
		
		else
			m_barcodeFormat ->m_option2 = 0;

		break;

	case BARCODE_GRIDMATRIX:

		if ( l_size >= 1 && l_size <= 13 )
			m_barcodeFormat ->m_option2 = l_size;

		else
			m_barcodeFormat ->m_option2 = 0;

		if ( l_security >= 1 && l_security <= 5 )
			m_barcodeFormat ->m_option1 = l_security;

		else
			m_barcodeFormat ->m_option1 = 1;

		break;
	}

	switch ( m_barcodeFormat ->m_symbology )
	{
	case BARCODE_EANX_CC:
	case BARCODE_EAN128_CC:
	case BARCODE_UPCA_CC:
	case BARCODE_UPCE_CC:
	case BARCODE_RSS14_CC:
	case BARCODE_RSS14STACK_CC:
	case BARCODE_RSS14_OMNI_CC:
	case BARCODE_RSS_LTD_CC:
	case BARCODE_RSS_EXP_CC:
	case BARCODE_RSS_EXPSTACK_CC:

		if ( l_mode >= 1 && l_mode <= 3 )
			m_barcodeFormat ->m_option1 = l_mode;

		else
			m_barcodeFormat ->m_option1 = 1;

		break;
	}
} // C'tor

DocxCompilerBarcodeField::~DocxCompilerBarcodeField()
{
	delete m_barcodeFormat;
} // d'tor



void DocxCompilerBarcodeField::serialize( ZipFile* p_zipFile )
{
	DocxCompilerField::serialize( p_zipFile );

	p_zipFile ->writeNum<int32>	( m_barcodeFormat ->m_symbology );
	p_zipFile ->writeStr		( m_barcodeFormat ->m_symbologyName );
	p_zipFile ->writeNum<int32>	( m_barcodeFormat ->m_option1 );
	p_zipFile ->writeNum<int32>	( m_barcodeFormat ->m_option2 );
	p_zipFile ->writeNum<int32>	( m_barcodeFormat ->m_option3 );
	p_zipFile ->writeNum<int32>	( m_barcodeFormat ->m_inputMode );

	p_zipFile ->writeNum<float>	( m_barcodeFormat ->m_scale );
	p_zipFile ->writeNum<int32>	( m_barcodeFormat ->m_height );
	p_zipFile ->writeNum<int32>	( m_barcodeFormat ->m_showHrt );
	p_zipFile ->writeNum<int32>	( m_barcodeFormat ->m_angle );
	p_zipFile ->writeNum<int32>	( m_barcodeFormat ->m_fontSize );
	p_zipFile ->writeStr		( m_barcodeFormat ->m_font );
	p_zipFile ->writeStr		( m_barcodeFormat ->m_fgcolor );
	p_zipFile ->writeStr		( m_barcodeFormat ->m_bgcolor );
	p_zipFile ->writeNum<int32>	( m_barcodeFormat ->m_whiteSpaceWidth );
	p_zipFile ->writeNum<int32>	( m_barcodeFormat ->m_borderWidth );
	p_zipFile ->writeNum<int32>	( m_barcodeFormat ->m_outputOptions );
} // serialize