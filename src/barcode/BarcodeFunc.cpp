
#include "DocxFactory/barcode/BarcodeFunc.h"
#include "DocxFactory/barcode/BarcodeFormat.h"

#include "DocxFactory/str/StrFunc.h"

#include "zint/zint.h"

#include <climits>
#include <cstring>

using namespace DocxFactory;
using namespace std;



bool BarcodeFunc::encodeAndPrint(
	BarcodeFormat*	p_barcodeFormat,
	const string&	p_value,
	BarcodePrinter	p_printer,
	string&			p_errTxt,
	string&			p_retVal )
{
	string				l_primary	= "";
	string				l_value		= p_value;
	
	struct zint_symbol*	l_symbol;
	int					l_errVal;
	const char*			l_retVal;

	p_retVal = "";

	l_symbol = ZBarcode_Create();
													l_symbol ->symbology		= p_barcodeFormat ->m_symbology;
	if ( p_barcodeFormat ->m_option1 != -1 )		l_symbol ->option_1			= p_barcodeFormat ->m_option1;
	if ( p_barcodeFormat ->m_option2 != -1 )		l_symbol ->option_2			= p_barcodeFormat ->m_option2;
	if ( p_barcodeFormat ->m_option3 != -1 )		l_symbol ->option_3			= p_barcodeFormat ->m_option3;
	if ( p_barcodeFormat ->m_inputMode != -1 )		l_symbol ->input_mode		= p_barcodeFormat ->m_inputMode;

	if ( p_barcodeFormat ->m_scale > 0 )			l_symbol ->scale			= p_barcodeFormat ->m_scale;
	if ( p_barcodeFormat ->m_height > 0 )			l_symbol ->height			= p_barcodeFormat ->m_height;
	if ( p_barcodeFormat ->m_showHrt != -1 )		l_symbol ->show_hrt			= p_barcodeFormat ->m_showHrt;
	if ( p_barcodeFormat ->m_whiteSpaceWidth > 0 )	l_symbol ->whitespace_width	= p_barcodeFormat ->m_whiteSpaceWidth;
	if ( p_barcodeFormat ->m_borderWidth > 0 )		l_symbol ->border_width		= p_barcodeFormat ->m_borderWidth;
	if ( p_barcodeFormat ->m_outputOptions != -1 )	l_symbol ->output_options	= p_barcodeFormat ->m_outputOptions;

	if ( !p_barcodeFormat ->m_fgcolor.empty() )
	strcpy( l_symbol ->fgcolour,  p_barcodeFormat ->m_fgcolor.c_str() );

	/* bgcolor can be empty in which case there is no background except for maxicode which must have a background */
	if ( !p_barcodeFormat ->m_fgcolor.empty() || p_barcodeFormat ->m_symbology != BARCODE_MAXICODE )
	strcpy( l_symbol ->bgcolour,  p_barcodeFormat ->m_bgcolor.c_str() );

	if ( p_barcodeFormat ->m_symbology == BARCODE_MAXICODE
	  && p_barcodeFormat ->m_option1 >= 2 && p_barcodeFormat ->m_option1 <= 3

	  || p_barcodeFormat ->m_symbology == BARCODE_EANX_CC
	  || p_barcodeFormat ->m_symbology == BARCODE_EAN128_CC
	  || p_barcodeFormat ->m_symbology == BARCODE_UPCA_CC
	  || p_barcodeFormat ->m_symbology == BARCODE_UPCE_CC
	  || p_barcodeFormat ->m_symbology == BARCODE_RSS14_CC
	  || p_barcodeFormat ->m_symbology == BARCODE_RSS14STACK_CC
	  || p_barcodeFormat ->m_symbology == BARCODE_RSS14_OMNI_CC
	  || p_barcodeFormat ->m_symbology == BARCODE_RSS_LTD_CC
	  || p_barcodeFormat ->m_symbology == BARCODE_RSS_EXP_CC
	  || p_barcodeFormat ->m_symbology == BARCODE_RSS_EXPSTACK_CC )
	{
		size_t i = p_value.find( '\n' );
		if ( i != string::npos )
		{
			l_primary	= p_value.substr( 0, i );
			l_value		= p_value.substr( i + 1 );
		}
	}

	if ( l_primary.length() > 127 )
		 l_primary.substr( 0, 127 );

	if ( !l_primary.empty() )
		strcpy( l_symbol ->primary, l_primary.c_str() );

	l_errVal = ZBarcode_Encode( l_symbol, ( unsigned char* ) l_value.c_str(), 0);
	if( l_errVal <= WARN_INVALID_OPTION )
	{
		l_errVal = ZBarcode_Export( l_symbol, p_barcodeFormat ->m_angle, p_barcodeFormat ->m_fontSize, ( char* ) p_barcodeFormat ->m_font.c_str(), l_retVal );
		if( l_errVal <= WARN_INVALID_OPTION ) p_retVal = string( l_retVal );
	}

	if( l_errVal > WARN_INVALID_OPTION )
	{
		p_errTxt = StrFunc::trim( string( l_symbol ->errtxt ) );

		if ( StrFunc::begins( StrFunc::lc( p_errTxt ), "error:" ) )
			 p_errTxt = StrFunc::leftTrim( p_errTxt.substr(6) );

		p_errTxt = p_barcodeFormat ->m_symbologyName + ": " 
			+ p_errTxt
			+ " (" + p_value + ").";
	}

	ZBarcode_Delete( l_symbol );

	if( l_errVal > WARN_INVALID_OPTION )
		return false;
	else
		return true;
} // encodeAndPrint
