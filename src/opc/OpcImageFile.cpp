
#include "DocxFactory/opc/OpcPackage.h"
#include "DocxFactory/opc/OpcPart.h"
#include "DocxFactory/opc/OpcImageCache.h"
#include "DocxFactory/opc/OpcImageFile.h"
#include "DocxFactory/opc/OpcFunc.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/os/OsFunc.h"

#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include "zlib/zip.h"

#include "Magick++.h"
#include "magick/image.h"

#include <cstring>

using namespace DocxFactory;
using namespace std;



OpcImageFile::OpcImageFile(
	OpcImageCache*	p_imageCache,
	const string&	p_sourceFullPath,
	const string&	p_fileExt )
{
	string	l_ext;
	bool	l_convert = false;

		 if ( p_fileExt == ".jpeg" )	  l_ext = ".jpeg";
	else if	( p_fileExt == ".jpg" )		  l_ext = ".jpeg";
	else if	( p_fileExt == ".jpe" )		  l_ext = ".jpeg";
	else if	( p_fileExt == ".jfif" )	  l_ext = ".jpeg";
	else if ( p_fileExt == ".png" )		  l_ext = ".png";
	else if ( p_fileExt == ".gif" )		  l_ext = ".gif";
	else if ( p_fileExt == ".tiff" )	  l_ext = ".tiff";
	else if ( p_fileExt == ".tif" )		  l_ext = ".tiff";
	else if ( p_fileExt == ".bmp" )		{ l_ext = ".png";  l_convert = true; }
	else if ( p_fileExt == ".dib" )		{ l_ext = ".png";  l_convert = true; }
	else								{ l_ext = ".jpeg"; l_convert = true; }

	m_package			= p_imageCache ->getPackage();
	m_imageCache		= p_imageCache;
	m_sourceFullPath	= p_sourceFullPath;
	m_ext				= l_ext;
	m_converted			= false;

	m_imageSource		= SOURCE_EXTERNAL_FILE;
	m_imageBuf			= NULL;
	m_imageBufSize		= 0;

	m_emuWidth			= 0;
	m_emuHeight			= 0;

	if ( l_convert )
		loadImage( true, l_ext );
} // c'tor

OpcImageFile::~OpcImageFile()
{
	if ( m_imageBuf )
		delete[] m_imageBuf;
} // d'tor



void OpcImageFile::loadImage( bool p_convert, const string& p_convertTo /* = "" */ )
{
	Magick::Image*	l_image		= NULL;
	Magick::Blob*	l_imageBlob	= NULL;

	string	l_ext;
	double	l_width;
	double	l_height;

	l_ext = StrFunc::caps( p_convertTo );

	if ( l_ext.length() > 0
	  && l_ext[0] == '.' )
		 l_ext = l_ext.substr( 1 );

	try
	{
		// if we try to load an image of unsupported format ErrorCorruptImage will be thrown
		l_image		= new Magick::Image( m_sourceFullPath );
		l_imageBlob	= new Magick::Blob();

		// resolutionUnits() gets undefined value after reading the image (possibly a bug in ImageMagick)
		// so we set it with Image::constImage() ->units which gets the correct value
		l_image ->resolutionUnits( l_image ->constImage() ->units );

		if ( p_convert )
		{
			try
			{
				l_image ->magick( l_ext );
				l_image ->write( l_imageBlob );

				m_converted = true;
			}

			catch ( ... )
			{
				if ( l_image )		delete l_image;
				if ( l_imageBlob )	delete l_imageBlob;

				l_image		= new Magick::Image( m_sourceFullPath );
				l_imageBlob	= new Magick::Blob();

				l_image ->resolutionUnits( l_image ->constImage() ->units );
				l_image ->write( l_imageBlob );
			}
		}

		else
			l_image ->write( l_imageBlob );

		if ( m_imageBuf )
			delete[] m_imageBuf; // in case loadImage() is called more than once

		m_imageBufSize	= l_imageBlob ->length();
		m_imageBuf		= new byte[ m_imageBufSize ];

		memcpy( m_imageBuf, l_imageBlob ->data(), m_imageBufSize );
		m_imageSource = SOURCE_IMAGE_OBJECT; // mark image as loaded

		if ( l_image ->xResolution() == 0
		  || l_image ->yResolution() == 0 )
		{
			m_emuWidth	= ( unsigned int ) ( l_image ->columns()	* m_emuPerPixel );
			m_emuHeight	= ( unsigned int ) ( l_image ->rows()		* m_emuPerPixel );
		}

		else
		{
			// calculate print size (the units are l_image ->resolutionUnits())
			l_width		= ( double ) l_image ->columns()	/ l_image ->xResolution();
			l_height	= ( double ) l_image ->rows()		/ l_image ->yResolution();
		
			// multiply by the emu conversion scalar
			// according to the resolution units to get emu size
			// if resolutionUnits() is undefined we treat it as centimeters
			// like image magick does when converting

			if ( l_image ->resolutionUnits() == Magick::PixelsPerInchResolution )
			{
				m_emuWidth	= ( unsigned int ) ( l_width	* m_emuPerInch );
				m_emuHeight	= ( unsigned int ) ( l_height	* m_emuPerInch );
			}

			else
			{
				m_emuWidth	= ( unsigned int ) ( l_width	* m_emuPerCm );
				m_emuHeight	= ( unsigned int ) ( l_height	* m_emuPerCm );
			}
		}

		delete l_image;
		delete l_imageBlob;
	}

	catch ( ... )
	{
		if ( l_image )		delete		l_image;
		if ( l_imageBlob )	delete		l_imageBlob;
		if ( m_imageBuf )	delete[]	m_imageBuf;

		throw;
	}
} // loadImage



void OpcImageFile::save( const string& p_partFullPath )
{
	if ( m_imageSource == SOURCE_EXTERNAL_FILE )
		m_package ->getZipFile() ->addEntryFromFile( m_sourceFullPath,
			OpcFunc::opcToZipPath( p_partFullPath ) );

	else
		m_package ->getZipFile() ->addEntryFromBuf(
			OpcFunc::opcToZipPath( p_partFullPath ), m_imageBuf, m_imageBufSize );
} // save



OpcPackage* OpcImageFile::getPackage() const
{
	return m_package;
} // getPackage

OpcImageCache* OpcImageFile::getImageCache() const
{
	return m_imageCache;
} // getImageCache

string OpcImageFile::getSourceFullPath() const
{
	return m_sourceFullPath;
} // getSourceFullPath

string OpcImageFile::getExt() const
{
	return m_ext;
} // getExt

bool OpcImageFile::isConverted() const
{
	return m_converted;
} // isConverted

unsigned int OpcImageFile::getEmuWidth() const
{
	// if the image is not yet loaded into the object, the first
	// call to this function will have to load it using loadImage()
	// which is a non-const member function. we want to keep getEmuWidth()
	// a const function because the change in the state of the object
	// doesn't concern the user of the object, so we cast the const away to call loadImage().
	// the same applies for getEmuHeight().

	if ( m_imageSource == SOURCE_EXTERNAL_FILE )
		( ( OpcImageFile* ) this ) ->loadImage( false );
	
	return m_emuWidth;
} // getPixelWidth

unsigned int OpcImageFile::getEmuHeight() const
{
	if ( m_imageSource == SOURCE_EXTERNAL_FILE )
		( ( OpcImageFile* ) this ) ->loadImage( false );
	
	return m_emuHeight;
} // getPixelHeight
