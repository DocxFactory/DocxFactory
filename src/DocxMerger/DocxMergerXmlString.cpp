
#include "DocxFactory/DocxMerger/DocxMergerItemFile.h"
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerTextField.h"
#include "DocxFactory/DocxMerger/DocxMergerNumberField.h"
#include "DocxFactory/DocxMerger/DocxMergerDateTimeField.h"
#include "DocxFactory/DocxMerger/DocxMergerBooleanField.h"
#include "DocxFactory/DocxMerger/DocxMergerAltChunkField.h"
#include "DocxFactory/DocxMerger/DocxMergerPicField.h"
#include "DocxFactory/DocxMerger/DocxMergerBarcodeField.h"
#include "DocxFactory/DocxMerger/DocxMergerXmlString.h"
#include "DocxFactory/DocxMerger/DocxMergerId.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteFieldGroup.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/UnzipFile.h"

using namespace DocxFactory;
using namespace std;



DocxMergerXmlString::DocxMergerXmlString()
{

} // c'tor

DocxMergerXmlString::~DocxMergerXmlString()
{

} // d'tor



void DocxMergerXmlString::save( DocxMergerPasteFieldGroup* p_pasteFieldGroup )
{
	ZipFile*				l_zipFile		= m_itemFile ->getZipFile();
	string					l_wordMlPrefix	= m_itemFile ->getWordMlPrefix();
	DocxMergerXmlString*	l_sect;

	switch ( m_type )
	{
	/*
	case DocxMergerXmlString::TYPE_TEXT_FIELD:

		( ( DocxMergerTextField* ) m_ptr ) ->save(
			p_pasteFieldGroup,
			p_zipFile,
			p_wordMlPrefix,
			p_relMlPrefix );

		break;
				
	case DocxMergerXmlString::TYPE_NUMBER_FIELD:

		( ( DocxMergerNumberField* ) m_ptr ) ->save(
			p_pasteFieldGroup,
			p_zipFile,
			p_wordMlPrefix,
			p_relMlPrefix );

		break;

	case DocxMergerXmlString::TYPE_DATE_TIME_FIELD:

		( ( DocxMergerDateTimeField* ) m_ptr ) ->save(
			p_pasteFieldGroup,
			p_zipFile,
			p_wordMlPrefix,
			p_relMlPrefix );

		break;

	case DocxMergerXmlString::TYPE_BOOLEAN_FIELD:

		( ( DocxMergerBooleanField* ) m_ptr ) ->save(
			p_pasteFieldGroup,
			p_zipFile,
			p_wordMlPrefix,
			p_relMlPrefix );

		break;

	case DocxMergerXmlString::TYPE_ALT_CHUNK_FIELD:

		if( p_openTc ->size() > 0 )
			p_openTc ->back() = true; // if in tc, then either there's a p after it or it will add a p

		( ( DocxMergerAltChunkField* ) m_ptr ) ->save(
			p_pasteFieldGroup,
			p_zipFile,
			p_wordMlPrefix,
			p_relMlPrefix );

		break;

	case DocxMergerXmlString::TYPE_PIC_FIELD:

		( ( DocxMergerPicField* ) m_ptr ) ->save(
			p_pasteFieldGroup,
			p_zipFile,
			p_wordMlPrefix,
			p_relMlPrefix );

		break;

	case DocxMergerXmlString::TYPE_BARCODE_FIELD:

		( ( DocxMergerBarcodeField* ) m_ptr ) ->save(
			p_pasteFieldGroup,
			p_zipFile,
			p_wordMlPrefix,
			p_relMlPrefix );

		break;
	*/

	case DocxMergerXmlString::TYPE_ALT_CHUNK_FIELD:

		m_itemFile ->setOpenTcBack( true ); // if in tc, then either there's a p after it or it will add a p

	case DocxMergerXmlString::TYPE_TEXT_FIELD:
	case DocxMergerXmlString::TYPE_NUMBER_FIELD:
	case DocxMergerXmlString::TYPE_DATE_TIME_FIELD:
	case DocxMergerXmlString::TYPE_BOOLEAN_FIELD:
	case DocxMergerXmlString::TYPE_PIC_FIELD:
	case DocxMergerXmlString::TYPE_BARCODE_FIELD:
	case DocxMergerXmlString::TYPE_CHART_FIELD:

		( ( DocxMergerField* ) m_ptr ) ->save( p_pasteFieldGroup );

		break;

	case DocxMergerXmlString::TYPE_PAGE_BREAK:

		if ( !( m_itemFile ->getFirstPage() ) )
			( *l_zipFile ) << m_str;

		m_itemFile ->setFirstPage( false );

		if ( m_itemFile ->getType() == DocxMergerItemFile::DOCUMENT )
		{
			l_sect = ( DocxMergerXmlString* ) m_ptr;
			if ( l_sect != m_itemFile ->getSect() )
			{
				if ( m_itemFile ->getSect() )
					( *l_zipFile ) <<
						  "<"	+ l_wordMlPrefix + ":p>"
						+ "<"	+ l_wordMlPrefix + ":pPr>"
						+ *m_itemFile ->getSect() ->getStr()
						+ "</"	+ l_wordMlPrefix + ":pPr>"
						+ "</"	+ l_wordMlPrefix + ":p>";
			
				m_itemFile ->setSect( l_sect );
			}
		}

		break;

	case DocxMergerXmlString::TYPE_ID:

		( *l_zipFile ) << ( ( DocxMergerId* ) m_ptr ) ->getNextId();

		break;

	case DocxMergerXmlString::TYPE_ID_REF:

		( *l_zipFile ) << ( ( DocxMergerId* ) m_ptr ) ->getCurrId();

		break;

	case DocxMergerXmlString::TYPE_SHADE:

		( *l_zipFile ) << ( m_itemFile ->getShadeBack() );

		break;

	case DocxMergerXmlString::TYPE_TC_START:

		m_itemFile ->pushBackOpenTc( false );

		break;

	case DocxMergerXmlString::TYPE_TC_END:

		if ( !( m_itemFile ->getOpenTcBack() ) )
		{
			( *l_zipFile ) <<
				  "<"	+ l_wordMlPrefix + ":p>"
				+ "<"	+ l_wordMlPrefix + ":pPr>"
				+ "<"	+ l_wordMlPrefix + ":spacing "
						+ l_wordMlPrefix + ":before=\"0\" "
						+ l_wordMlPrefix + ":after=\"0\" "
						+ l_wordMlPrefix + ":line=\"2\" "
						+ l_wordMlPrefix + ":lineRule=\"exact\"/>"

				+ "<"	+ l_wordMlPrefix + ":rPr>"
				+ "<"	+ l_wordMlPrefix + ":sz "
						+ l_wordMlPrefix + ":val=\"2\"/>"

				+ "<"	+ l_wordMlPrefix + ":szCs "
						+ l_wordMlPrefix + ":val=\"2\"/>"

				+ "</"	+ l_wordMlPrefix + ":rPr>"
				+ "</"	+ l_wordMlPrefix + ":pPr>"
				+ "</"	+ l_wordMlPrefix + ":p>";
		}

		m_itemFile ->popBackOpenTc();

		break;

	case DocxMergerXmlString::TYPE_P_START:

		m_itemFile ->setOpenTcBack( true );

		( *l_zipFile ) << m_str;

		break;

	case DocxMergerXmlString::TYPE_XML:

		( *l_zipFile ) << m_str;

		break;
	}
} // save



void DocxMergerXmlString::deserialize( UnzipFile* p_unzipFile )
{
	p_unzipFile ->insertPtrBySeq( p_unzipFile ->readNum<uint32>(), this );

	m_itemFile	= ( DocxMergerItemFile* )	p_unzipFile ->readNum<uint32>();
	m_item		= ( DocxMergerItem* )		p_unzipFile ->readNum<uint32>();
	m_type		= ( XmlStringType )			p_unzipFile ->readNum<int16>();
	m_str		=							p_unzipFile ->readStr();
	m_ptr		= ( void* )					p_unzipFile ->readNum<uint32>();
} // deserialize

void DocxMergerXmlString::link( UnzipFile* p_unzipFile )
{
	const map<uint32, void*>*	l_ptrsBySeq = p_unzipFile ->getPtrsBySeq();
	uint32						l_ptrSeq;

	l_ptrSeq	= ( uint32 ) m_itemFile;
	m_itemFile	= ( DocxMergerItemFile* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;

	l_ptrSeq	= ( uint32 ) m_item;
	m_item		= ( DocxMergerItem* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;

	l_ptrSeq	= ( uint32 ) m_ptr;
	m_ptr		= ( void* ) l_ptrsBySeq ->find( l_ptrSeq ) ->second;
} // link



DocxMergerXmlString::XmlStringType DocxMergerXmlString::getType() const
{
	return m_type;
} // getType

const string* DocxMergerXmlString::getStr() const
{
	return &m_str;
} // getStr

void* DocxMergerXmlString::getPtr() const
{
	return m_ptr;
} // getPtr
