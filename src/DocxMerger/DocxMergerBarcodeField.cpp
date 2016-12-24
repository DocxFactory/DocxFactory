
#include "DocxFactory/DocxMerger/DocxMergerItemFile.h"
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerBarcodeField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteFieldGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteBarcodeField.h"

#include "DocxFactory/barcode/BarcodeFunc.h"
#include "DocxFactory/barcode/BarcodeFormat.h"

#include "DocxFactory/xml/XmlFunc.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/UnzipFile.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/locale/NumFormat.h"
#include "DocxFactory/str/StrFunc.h"

using namespace DocxFactory;
using namespace std;



NumFormat*	DocxMergerBarcodeField::m_numFormat	= NULL;



DocxMergerBarcodeField::DocxMergerBarcodeField() : DocxMergerField( DocxMergerField::TYPE_BARCODE )
{
	if ( !m_numFormat )
	{
		m_numFormat = new NumFormat();

		m_numFormat ->m_posPrefix		= "";
		m_numFormat ->m_posLeftDigCnt	= 0;
		m_numFormat ->m_posRightDigCnt	= 0;
		m_numFormat ->m_posRightExtCnt	= 10;
		m_numFormat ->m_posSuffix		= "";
		m_numFormat ->m_posSep			= false;
		m_numFormat ->m_posPercent		= false;

		m_numFormat ->m_negPrefix		= "-";
		m_numFormat ->m_negLeftDigCnt	= 0;
		m_numFormat ->m_negRightDigCnt	= 0;
		m_numFormat ->m_negRightExtCnt	= 10;
		m_numFormat ->m_negSuffix		= "";
		m_numFormat ->m_negSep			= false;
		m_numFormat ->m_negPercent		= false;

		m_numFormat ->m_zeroPrefix		= "";
		m_numFormat ->m_zeroLeftDigCnt	= 0;
		m_numFormat ->m_zeroRightDigCnt	= 0;
		m_numFormat ->m_zeroSuffix		= "";
	}
} // c'tor

DocxMergerBarcodeField::~DocxMergerBarcodeField()
{
	delete m_barcodeFormat;
} // d'tor



void DocxMergerBarcodeField::save( DocxMergerPasteFieldGroup* p_pasteFieldGroup )
{
	if ( !p_pasteFieldGroup )
		return;

	ZipFile*	l_zipFile		= m_itemFile ->getZipFile();
	string		l_wordMlPrefix	= m_itemFile ->getWordMlPrefix();

	map<DocxMergerField*, DocxMergerPasteField*>::const_iterator	l_pasteFieldIterator;
	const map<DocxMergerField*, DocxMergerPasteField*>*				l_pasteFields = p_pasteFieldGroup ->getPasteFieldsByField();
	DocxMergerPasteBarcodeField*									l_pasteBarcodeField;

	string	l_err;
	string	l_str;
	bool	l_ok;

	l_pasteFieldIterator = l_pasteFields ->find( this );
	if ( l_pasteFieldIterator != l_pasteFields ->end() )
	{
		l_pasteBarcodeField	= ( DocxMergerPasteBarcodeField* ) l_pasteFieldIterator ->second;

		l_ok = BarcodeFunc::encodeAndPrint(
			m_barcodeFormat,
			*( l_pasteBarcodeField ->getValue() ),
			BarcodeFunc::PRINT_VML,
			l_err,
			l_str );

		if ( l_ok )
		{
			( *l_zipFile ) << "<"  + l_wordMlPrefix + ":pict>";
			( *l_zipFile ) << l_str;
			( *l_zipFile ) << "</" + l_wordMlPrefix + ":pict>";
		}	

		else
		{
			( *l_zipFile ) << "<"  + l_wordMlPrefix + ":rPr>";
			( *l_zipFile ) << "<"  + l_wordMlPrefix + ":highlight "	+ l_wordMlPrefix + ":val=\"red\"/>";
			( *l_zipFile ) << "<"  + l_wordMlPrefix + ":color "		+ l_wordMlPrefix + ":val=\"white\"/>";
			( *l_zipFile ) << "</" + l_wordMlPrefix + ":rPr>";
			( *l_zipFile ) << "<"  + l_wordMlPrefix + ":t xml:space=\"preserve\">";
			( *l_zipFile ) << XmlFunc::encodeXml( l_err );
			( *l_zipFile ) << "</" + l_wordMlPrefix + ":t>";
		}
	}
} // save



void DocxMergerBarcodeField::setClipboardValue( const string& p_value )
{
	createPasteField<const string&, DocxMergerPasteBarcodeField>( p_value );
} // setClipboardValue( string )

void DocxMergerBarcodeField::setClipboardValue( double p_value )
{
	string l_value = LocaleFunc::numToStr( p_value, m_numFormat );

	createPasteField<const string&, DocxMergerPasteBarcodeField>( l_value );
} // setClipboardValue( double )



void DocxMergerBarcodeField::deserialize( UnzipFile* p_unzipFile )
{
	DocxMergerField::deserialize( p_unzipFile );

	m_barcodeFormat	= new BarcodeFormat;
	m_barcodeFormat ->m_symbology			= p_unzipFile ->readNum<int32>();
	m_barcodeFormat ->m_symbologyName		= p_unzipFile ->readStr();
	m_barcodeFormat ->m_option1				= p_unzipFile ->readNum<int32>();
	m_barcodeFormat ->m_option2				= p_unzipFile ->readNum<int32>();
	m_barcodeFormat ->m_option3				= p_unzipFile ->readNum<int32>();
	m_barcodeFormat ->m_inputMode			= p_unzipFile ->readNum<int32>();

	m_barcodeFormat ->m_scale				= p_unzipFile ->readNum<float>();
	m_barcodeFormat ->m_height				= p_unzipFile ->readNum<int32>();
	m_barcodeFormat ->m_showHrt				= p_unzipFile ->readNum<int32>();
	m_barcodeFormat ->m_angle				= p_unzipFile ->readNum<int32>();
	m_barcodeFormat ->m_fontSize			= p_unzipFile ->readNum<int32>();
	m_barcodeFormat ->m_font				= p_unzipFile ->readStr();
	m_barcodeFormat ->m_fgcolor				= p_unzipFile ->readStr();
	m_barcodeFormat ->m_bgcolor				= p_unzipFile ->readStr();
	m_barcodeFormat ->m_whiteSpaceWidth		= p_unzipFile ->readNum<int32>();
	m_barcodeFormat ->m_borderWidth			= p_unzipFile ->readNum<int32>();
	m_barcodeFormat ->m_outputOptions		= p_unzipFile ->readNum<int32>();
} // deserialize
