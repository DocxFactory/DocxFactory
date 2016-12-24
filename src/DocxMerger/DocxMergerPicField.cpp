
#include "DocxFactory/DocxMerger/DocxMergerFile.h"
#include "DocxFactory/DocxMerger/DocxMergerItemFile.h"
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerPicField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteFieldGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"
#include "DocxFactory/DocxMerger/DocxMergerPastePicField.h"

#include "DocxFactory/opc/OpcPart.h"
#include "DocxFactory/opc/OpcImageFile.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/UnzipFile.h"
#include "DocxFactory/os/OsFunc.h"

#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/util/ArgTypeMismatchException.h"

using namespace DocxFactory;
using namespace std;



DocxMergerPicField::DocxMergerPicField() : DocxMergerField( DocxMergerField::TYPE_PIC )
{

} // c'tor

DocxMergerPicField::~DocxMergerPicField()
{

} // d'tor



void DocxMergerPicField::save( DocxMergerPasteFieldGroup* p_pasteFieldGroup )
{
	if ( !p_pasteFieldGroup )
		return;

	ZipFile*	l_zipFile		= m_itemFile ->getZipFile();
	string		l_relMlPrefix	= m_itemFile ->getRelMlPrefix();

	map<DocxMergerField*, DocxMergerPasteField*>::const_iterator	l_pasteFieldIterator;
	const map<DocxMergerField*, DocxMergerPasteField*>*				l_pasteFields = p_pasteFieldGroup ->getPasteFieldsByField();
	DocxMergerPastePicField*										l_pastePicField;

	list<pair<string, char>>::const_iterator						l_stringIterator;

	OpcImageFile*	l_imageFile;
	unsigned int	l_emuWidth;
	unsigned int	l_emuHeight;
	string			l_fileRId;

	l_pasteFieldIterator = l_pasteFields ->find( this );
	if ( l_pasteFieldIterator != l_pasteFields ->end() )
	{
		l_pastePicField	= ( DocxMergerPastePicField* ) l_pasteFieldIterator ->second;
		l_imageFile		= m_itemFile ->getPart() ->insertImageFile( 

			l_pastePicField ->getTargetMode(),
			*( l_pastePicField ->getFileName() ),
			*( l_pastePicField ->getFileExt() ),
			*( l_pastePicField ->getFileUrl() ), 
			l_fileRId );

		switch ( m_useSize )
		{
		case DocxMergerPicField::USE_IMAGE_FILE_SIZE:

			l_emuWidth	= l_imageFile ->getEmuWidth();
			l_emuHeight	= l_imageFile ->getEmuHeight();

			break;

		case DocxMergerPicField::USE_PLACE_HOLDER_WIDTH:

			l_emuWidth	= l_imageFile ->getEmuWidth();
			l_emuHeight	= l_imageFile ->getEmuHeight();

			l_emuHeight	= ( unsigned int ) ( ( ( double ) l_emuHeight / l_emuWidth ) * m_emuWidth );
			l_emuWidth	= m_emuWidth;

			break;

		case DocxMergerPicField::USE_PLACE_HOLDER_HEIGHT:

			l_emuWidth	= l_imageFile ->getEmuWidth();
			l_emuHeight	= l_imageFile ->getEmuHeight();

			l_emuWidth	= ( unsigned int ) ( ( ( double ) l_emuWidth / l_emuHeight ) * m_emuHeight );
			l_emuHeight	= m_emuHeight;
								
			break;

		case DocxMergerPicField::USE_PLACE_HOLDER_SIZE:

			l_emuWidth	= m_emuWidth;
			l_emuHeight	= m_emuHeight;

			break;
		}

		FOR_EACH( l_stringIterator, &m_picStrings )
		{
			( *l_zipFile ) << l_stringIterator ->first;

			switch ( l_stringIterator ->second )
			{
			case 'r': 

				( *l_zipFile ) <<
					( l_pastePicField ->getTargetMode() == OpcImageFile::EXTERNAL_MODE ? 
						l_relMlPrefix + ":link=\"" + l_fileRId + "\"" : 
						l_relMlPrefix + ":embed=\"" + l_fileRId + "\"" );

				break;

			case 'i': ( *l_zipFile ) << StrFunc::intToStr( m_file ->nextIdSeq() ); break;
			case 'x': ( *l_zipFile ) << StrFunc::intToStr( l_emuWidth ); break;
			case 'y': ( *l_zipFile ) << StrFunc::intToStr( l_emuHeight ); break;
			}
		}
	}
} // save



void DocxMergerPicField::setClipboardValue( const string& p_value )
{
	OpcImageFile::TargetMode	l_targetMode;
	string						l_fileName;
	string						l_fileExt;
	string						l_fileUrl;
	
	if ( p_value.empty() )
		return;

	if ( StrFunc::begins( StrFunc::lc( p_value ), "file://" ) )
	{
		l_targetMode	= OpcImageFile::EXTERNAL_MODE;
		l_fileUrl		= OsFunc::normalizeFileUrl( p_value );
		l_fileName		= OsFunc::fileUrlToPath( l_fileUrl );
	}

	else
	{
		l_targetMode	= OpcImageFile::INTERNAL_MODE;
		l_fileName		= OsFunc::normalizePath( p_value );
		l_fileUrl		= "";
	}

	l_fileName	= OsFunc::getFullPath( l_fileName );
	l_fileExt	= OsFunc::getSubPath( l_fileName, OsFunc::SUBPATH_EXT, OsFunc::SUBPATH_EXT );

	if ( !OsFunc::isFileExists( l_fileName ) )
		return;

	createPasteField( l_targetMode, l_fileName, l_fileExt, l_fileUrl );
} // setClipboardValue( string )

void DocxMergerPicField::setClipboardValue( double p_value )
{
	throw ArgTypeMismatchException( "string", "number", __FILE__, __LINE__ );
} // setClipboardValue( double )

void DocxMergerPicField::createPasteField(
	OpcImageFile::TargetMode p_targetMode,
	const string& p_fileName,
	const string& p_fileExt,
	const string& p_fileUrl )
{
	const map<DocxMergerField*, DocxMergerPasteField*>*	l_pasteFieldsByField;
	DocxMergerPasteFieldGroup*							l_pasteFieldGroup;
	
	l_pasteFieldGroup		= getItem() ->getItemFile() ->getPasteFieldGroup();
	l_pasteFieldsByField	= l_pasteFieldGroup ->getPasteFieldsByField();

	if ( l_pasteFieldsByField ->find( this ) == l_pasteFieldsByField ->end() )
		 l_pasteFieldGroup ->insertPasteField( this, new DocxMergerPastePicField(
			this,
			p_targetMode,
			p_fileName,
			p_fileExt,
			p_fileUrl ) );
} // createPasteField



void DocxMergerPicField::deserialize( UnzipFile* p_unzipFile )
{
	string	l_str;
	char	l_ch;

	uint32	l_size;
	uint32	i;

	DocxMergerField::deserialize( p_unzipFile );

	m_picId		=				p_unzipFile ->readNum<uint32>();
	m_emuWidth	=				p_unzipFile ->readNum<uint32>();
	m_emuHeight	=				p_unzipFile ->readNum<uint32>();
	m_useSize	= ( UseSize )	p_unzipFile ->readNum<int16>();

	l_size		= p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size ; ++i )
	{
		l_str	=			p_unzipFile ->readStr();
		l_ch	= ( char )	p_unzipFile ->readNum<uint8>();

		m_picStrings.push_back( make_pair( l_str, l_ch ) );
	}
} // deserialize
