
#ifndef __DOCXFACTORY_DOCX_COMPILER_FIELD_H__
#define __DOCXFACTORY_DOCX_COMPILER_FIELD_H__

#include "xercesc/util/XercesDefs.hpp"

#include <map>
#include <list>
#include <string>



XERCES_CPP_NAMESPACE_BEGIN

class DOMElement;

XERCES_CPP_NAMESPACE_END

namespace DocxFactory
{
	using namespace std;

	class ZipFile;

	class DocxCompilerItem;
	class DocxCompilerField
	{
	public:
		enum FieldType
		{
			TYPE_UNDEFINED,
			TYPE_TEXT,
			TYPE_NUMBER,
			TYPE_DATE_TIME,
			TYPE_BOOLEAN,
			TYPE_ALT_CHUNK,
			TYPE_PIC,
			TYPE_BARCODE,
			TYPE_CHART
		};

		enum FieldParam
		{
			PARAM_UNDEFINED,
			PARAM_TEXT,
			PARAM_NUMBER,
			PARAM_DATE_TIME,
			PARAM_UPPER_CASE,
			PARAM_LOWER_CASE,
			PARAM_TITLE_CASE,
			PARAM_FIRST_CAPITAL,
			PARAM_TRIM,
			PARAM_HTML,
			PARAM_RTF,
			PARAM_USE_IMAGE_FILE_SIZE,
			PARAM_USE_PLACE_HOLDER_WIDTH,
			PARAM_USE_PLACE_HOLDER_HEIGHT,
			PARAM_USE_PLACE_HOLDER_SIZE,
			PARAM_CODE11,
			PARAM_CODE25,
			PARAM_CODE25_IATA,
			PARAM_CODE25_INTER,
			PARAM_CODE25_LOGIC,
			PARAM_CODE25_IND,
			PARAM_ITF14,
			PARAM_CODE39,
			PARAM_CODE39_EXT,
			PARAM_CODE93,
			PARAM_PZN,
			PARAM_LOGMARS,
			PARAM_CODE32,
			PARAM_CODE128,
			PARAM_CODE128B,
			PARAM_EAN128,
			PARAM_EAN14,
			PARAM_NVE18,
			PARAM_UPCA,
			PARAM_UPCE,
			PARAM_EAN,
			PARAM_ISBN,
			PARAM_CODABAR,
			PARAM_PHARMA,
			PARAM_PHARMA_2TRACK,
			PARAM_PLESSEY,
			PARAM_MSI_PLESSEY,
			PARAM_TELEPEN,
			PARAM_TELEPEN_NUM,
			PARAM_RSS14,
			PARAM_RSS14_STACK,
			PARAM_RSS14_STACKOMNI,
			PARAM_RSS_LTD,
			PARAM_RSS_EXP,
			PARAM_RSS_EXPSTACK,
			PARAM_CHANNEL,
			PARAM_FIM,
			PARAM_FLAT,
			PARAM_DAFT,
			PARAM_POSTNET,
			PARAM_PLANET,
			PARAM_ONECODE,
			PARAM_RM4SCC,
			PARAM_DPLEIT,
			PARAM_DPIDENT,
			PARAM_AUSPOST,
			PARAM_AUSREPLY,
			PARAM_AUSROUTE,
			PARAM_AUSREDIRECT,
			PARAM_KIX,
			PARAM_JAPANPOST,
			PARAM_KOREAPOST,
			PARAM_CODE16K,
			PARAM_CODE49,
			PARAM_PDF417,
			PARAM_PDF417_TRUNC,
			PARAM_MICROPDF,
			PARAM_DATAMATRIX,
			PARAM_QRCODE,
			PARAM_MICROQR,
			PARAM_MAXICODE,
			PARAM_AZTEC,
			PARAM_AZRUNE,
			PARAM_CODEONE,
			PARAM_GRIDMATRIX,
			PARAM_HIBC_CODE39,
			PARAM_HIBC_CODE128,
			PARAM_HIBC_PDF417,
			PARAM_HIBC_MICROPDF,
			PARAM_HIBC_DATAMATRIX,
			PARAM_HIBC_QRCODE,
			PARAM_HIBC_AZTEC,
			PARAM_CS_EAN,
			PARAM_CS_EAN128,
			PARAM_CS_UPCA,
			PARAM_CS_UPCE,
			PARAM_CS_RSS14,
			PARAM_CS_RSS14_STACK,
			PARAM_CS_RSS14_STACKOMNI,
			PARAM_CS_RSS_LTD,
			PARAM_CS_RSS_EXP,
			PARAM_CS_RSS_EXPSTACK,
			PARAM_CHECK_DIGIT,
			PARAM_COLS,
			PARAM_CHANNELS,
			PARAM_SIZE,
			PARAM_MODE,
			PARAM_SECURITY,
			PARAM_SQUARE,
			PARAM_GS1,
			PARAM_SCALE,
			PARAM_HEIGHT,
			PARAM_SHOW_VALUE,
			PARAM_ANGLE,
			PARAM_FONT_SIZE,
			PARAM_FONT,
			PARAM_FGCOLOR,
			PARAM_BGCOLOR,
			PARAM_WHITESPACE_WIDTH,
			PARAM_BORDER_WIDTH,
			PARAM_BIND,
			PARAM_BOX
		};

		static void getTypeAndFormat( 
			string&						p_format,
			FieldType&					p_type,
			map<FieldParam, string>*	p_valueByParam );

		static FieldParam	strToParam( const string& p_str );
		static string		paramToStr( FieldParam p_param );



		DocxCompilerField(
			DocxCompilerItem*				p_item,
			FieldType						p_type,
			const string&					p_name,
			const string&					p_format,
			const map<FieldParam, string>*	p_valueByParam,
			xercesc::DOMElement*			p_placeHolderNode );

		virtual ~DocxCompilerField();

		virtual void serialize( ZipFile* p_zipFile );

		void setItem( DocxCompilerItem* p_item );

		DocxCompilerItem*				getItem() const;
		FieldType						getType() const;
		string							getName() const;
		string							getFormat() const;
		const map<FieldParam, string>*	getValueByParam() const;
		xercesc::DOMElement*			getPlaceHolderNode() const;

	protected:
		DocxCompilerItem*		m_item;
		FieldType				m_type;
		string					m_name;
		string					m_format;
		map<FieldParam, string>	m_valueByParam;
		xercesc::DOMElement*	m_placeHolderNode;

	private:
		DocxCompilerField( const DocxCompilerField& p_other );
		DocxCompilerField& operator = ( const DocxCompilerField& p_other );

		static void initialize();

		static bool						m_initialized;
		static map<string, FieldParam>*	m_paramByStr;
		static map<FieldParam, string>*	m_strByParam;

	};
};

#endif
