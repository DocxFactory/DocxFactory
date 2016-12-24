
#include "DocxFactory/DocxCompiler/DocxCompilerItemFile.h"
#include "DocxFactory/DocxCompiler/DocxCompilerItem.h"
#include "DocxFactory/DocxCompiler/DocxCompilerNumberField.h"

#include "DocxFactory/zip/ZipFile.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/locale/NumFormat.h"

#include "DocxFactory/util/DocxFactoryDefs.h"

using namespace DocxFactory;
using namespace std;



DocxCompilerNumberField::DocxCompilerNumberField(
	DocxCompilerItem*				p_item,
	const string&					p_name,
	const string&					p_format,
	const map<FieldParam, string>*	p_valueByParam,
	xercesc::DOMElement*			p_placeHolderNode )

	: DocxCompilerField( 
		p_item,
		TYPE_NUMBER,
		p_name,
		p_format,
		p_valueByParam,
		p_placeHolderNode )
{
	string l_wordMlPrefix = p_item ->getItemFile() ->getWordMlPrefix();

	m_numFormat = new NumFormat;
	LocaleFunc::getNumFormat( p_format, m_numFormat );

	if ( !m_numFormat ->m_posColor.empty() )
	{
		m_numFormat ->m_posPrefix = 
			
			  "</"	+ l_wordMlPrefix + ":t>"
			+ "<"	+ l_wordMlPrefix + ":rPr>"
			+ "<"	+ l_wordMlPrefix + ":color " + l_wordMlPrefix + ":val=\"" + m_numFormat ->m_posColor + "\"/>"
			+ "</"	+ l_wordMlPrefix + ":rPr>"
			+ "<"	+ l_wordMlPrefix + ":t xml:space=\"preserve\">"
			
			+ m_numFormat ->m_posPrefix;

		m_numFormat ->m_posSuffix = m_numFormat ->m_posSuffix

			+ "</"	+ l_wordMlPrefix + ":t>"
			+ "<"	+ l_wordMlPrefix + ":rPr>"
			+ "<"	+ l_wordMlPrefix + ":color " + l_wordMlPrefix + ":val=\"auto\"/>"
			+ "</"	+ l_wordMlPrefix + ":rPr>"
			+ "<"	+ l_wordMlPrefix + ":t xml:space=\"preserve\">";
	}

	if ( !m_numFormat ->m_negColor.empty() )
	{
		m_numFormat ->m_negPrefix = 
			
			  "</"	+ l_wordMlPrefix + ":t>"
			+ "<"	+ l_wordMlPrefix + ":rPr>"
			+ "<"	+ l_wordMlPrefix + ":color " + l_wordMlPrefix + ":val=\"" + m_numFormat ->m_negColor + "\"/>"
			+ "</"	+ l_wordMlPrefix + ":rPr>"
			+ "<"	+ l_wordMlPrefix + ":t xml:space=\"preserve\">"
			
			+ m_numFormat ->m_negPrefix;

		m_numFormat ->m_negSuffix = m_numFormat ->m_negSuffix

			+ "</"	+ l_wordMlPrefix + ":t>"
			+ "<"	+ l_wordMlPrefix + ":rPr>"
			+ "<"	+ l_wordMlPrefix + ":color " + l_wordMlPrefix + ":val=\"auto\"/>"
			+ "</"	+ l_wordMlPrefix + ":rPr>"
			+ "<"	+ l_wordMlPrefix + ":t xml:space=\"preserve\">";
	}

	if ( !m_numFormat ->m_zeroColor.empty() )
	{
		m_numFormat ->m_zeroPrefix = 
			
			  "</"	+ l_wordMlPrefix + ":t>"
			+ "<"	+ l_wordMlPrefix + ":rPr>"
			+ "<"	+ l_wordMlPrefix + ":color " + l_wordMlPrefix + ":val=\"" + m_numFormat ->m_zeroColor + "\"/>"
			+ "</"	+ l_wordMlPrefix + ":rPr>"
			+ "<"	+ l_wordMlPrefix + ":t xml:space=\"preserve\">"
			
			+ m_numFormat ->m_zeroPrefix;

		m_numFormat ->m_zeroSuffix = m_numFormat ->m_zeroSuffix

			+ "</"	+ l_wordMlPrefix + ":t>"
			+ "<"	+ l_wordMlPrefix + ":rPr>"
			+ "<"	+ l_wordMlPrefix + ":color " + l_wordMlPrefix + ":val=\"auto\"/>"
			+ "</"	+ l_wordMlPrefix + ":rPr>"
			+ "<"	+ l_wordMlPrefix + ":t xml:space=\"preserve\">";
	}
} // c'tor

DocxCompilerNumberField::~DocxCompilerNumberField()
{
	delete m_numFormat;
} // d'tor



void DocxCompilerNumberField::serialize( ZipFile* p_zipFile )
{
	DocxCompilerField::serialize( p_zipFile );

	p_zipFile ->writeStr		( m_numFormat ->m_posColor );
	p_zipFile ->writeStr		( m_numFormat ->m_posPrefix );
	p_zipFile ->writeNum<uint16>( m_numFormat ->m_posLeftDigCnt );
	p_zipFile ->writeNum<uint16>( m_numFormat ->m_posRightDigCnt );
	p_zipFile ->writeNum<uint16>( m_numFormat ->m_posRightExtCnt );
	p_zipFile ->writeStr		( m_numFormat ->m_posSuffix );
	p_zipFile ->writeNum<uint8>	( m_numFormat ->m_posSep );
	p_zipFile ->writeNum<uint8>	( m_numFormat ->m_posPercent );

	p_zipFile ->writeStr		( m_numFormat ->m_negColor );
	p_zipFile ->writeStr		( m_numFormat ->m_negPrefix );
	p_zipFile ->writeNum<uint16>( m_numFormat ->m_negLeftDigCnt );
	p_zipFile ->writeNum<uint16>( m_numFormat ->m_negRightDigCnt );
	p_zipFile ->writeNum<uint16>( m_numFormat ->m_negRightExtCnt );
	p_zipFile ->writeStr		( m_numFormat ->m_negSuffix );
	p_zipFile ->writeNum<uint8>	( m_numFormat ->m_negSep );
	p_zipFile ->writeNum<uint8>	( m_numFormat ->m_negPercent );

	p_zipFile ->writeStr		( m_numFormat ->m_zeroColor );
	p_zipFile ->writeStr		( m_numFormat ->m_zeroPrefix );
	p_zipFile ->writeNum<uint16>( m_numFormat ->m_zeroLeftDigCnt );
	p_zipFile ->writeNum<uint16>( m_numFormat ->m_zeroRightDigCnt );
	p_zipFile ->writeStr		( m_numFormat ->m_zeroSuffix );
	p_zipFile ->writeNum<uint8>	( m_numFormat ->m_zeroSep );
} // serialize

