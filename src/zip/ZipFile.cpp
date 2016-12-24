
#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/ZipFileNotOpenException.h"
#include "DocxFactory/zip/ZipStreamNotOpenException.h"
#include "DocxFactory/zip/ZipStreamOpenException.h"
#include "DocxFactory/zip/MinizipException.h"
#include "DocxFactory/zip/FileInfo.h"

#include "DocxFactory/os/OsFunc.h"
#include "DocxFactory/os/FileNotFoundException.h"
#include "DocxFactory/os/FileExistsException.h"

#include "boost/scoped_array.hpp"

#include <ctime>
#include <cstring>

using namespace DocxFactory;
using namespace std;



ZipFile::ZipFile()
{
	m_zipFile		= NULL;
	m_fileName		= "";

	m_fileOpen		= false;
	m_streamOpen	= false;
} // c'tor

ZipFile::~ZipFile()
{
	close(); // close only works if zip file is open. no need to check twice if zip file is open.
} // d'tor

void ZipFile::open( const string& p_fileName, ZipWriteMode p_writeMode )
{
	close();

	if ( p_writeMode == ZIP_CREATE && OsFunc::isFileExists( p_fileName ) )
		throw FileExistsException( p_fileName, __FILE__, __LINE__ );

	if ( p_writeMode == ZIP_APPEND && !OsFunc::isFileExists( p_fileName ) )
		throw FileNotFoundException( p_fileName, __FILE__, __LINE__ );

	#ifdef USEWIN32IOAPI

		zlib_filefunc_def ffunc;
		fill_win32_filefuncA( &ffunc );

		m_zipFile = zipOpen2(
			p_fileName.c_str(),
			p_writeMode,
			NULL,
			&ffunc );

		if ( !m_zipFile )
			throw MinizipException( "zipOpen2", __FILE__, __LINE__ );

	#else

		m_zipFile = zipOpen(
			p_fileName.c_str(),
			p_writeMode );

		if ( !m_zipFile )
			throw MinizipException( "zipOpen", __FILE__, __LINE__ );

	#endif

	m_fileName	= p_fileName;
	m_fileOpen	= true;

	m_ptrSeq = 0;
	ptrToSeq( NULL ); // map NULL to sequence 0
} // open

void ZipFile::close()
{
	if ( !m_fileOpen )
		return;

	closeStream(); // closeStream only works if stream is open. no need to check twice if stream is open.

	int l_err = zipClose( m_zipFile, NULL );

	if ( l_err != ZIP_OK )
		throw MinizipException( "zipClose", l_err, __FILE__, __LINE__ );

	m_zipFile		= NULL;
	m_fileName		= "";

	m_fileOpen		= false;
	m_streamOpen	= false;

	m_seqsByPtr.clear();
} // close



void ZipFile::addEntryFromBuf(
	const				string& p_path,
	const byte* const	p_buf,
	size_t				p_bufSize,
	int					p_method,	// = Z_DEFLATED
	int					p_level )	// = Z_DEFAULT_COMPRESSION
{
	zip_fileinfo	l_fileInfo;
	int				l_err;

	if ( !m_fileOpen )
		throw ZipFileNotOpenException( __FILE__, __LINE__ );

	if ( m_streamOpen )
		throw ZipStreamOpenException( __FILE__, __LINE__ );

	l_fileInfo.internal_fa	= 0;
	l_fileInfo.external_fa	= 0;
	l_fileInfo.dosDate		= 0;
	l_fileInfo.tmz_date		= ZipFunc::getLocalTime( time( NULL ) );

	l_err = zipOpenNewFileInZip(
		m_zipFile,
		p_path.c_str(),
		&l_fileInfo,
		NULL, 0,
		NULL, 0,
		NULL,
		p_method,
		p_level );

	if ( l_err != ZIP_OK )
		throw MinizipException( "zipOpenNewFileInZip", l_err, __FILE__, __LINE__ );

	l_err = zipWriteInFileInZip( m_zipFile, p_buf, p_bufSize );

	if ( l_err != ZIP_OK )
		throw MinizipException( "zipWriteInFileInZip", l_err, __FILE__, __LINE__ );

	l_err = zipCloseFileInZip( m_zipFile );

	if ( l_err != ZIP_OK )
		throw MinizipException( "zipCloseFileInZip", l_err, __FILE__, __LINE__ );
} // addEntryFromBuf

void ZipFile::addEntryFromRaw(
	const string&	p_path,
	const byte*		p_buf,
	size_t			p_bufSize,
	int				p_method,
	int				p_level,
	FileInfo*		p_fileInfo )
{
	if ( !m_fileOpen )
		throw ZipFileNotOpenException( __FILE__, __LINE__ );

	if ( m_streamOpen )
		throw ZipStreamOpenException( __FILE__, __LINE__ );

	int l_err = zipOpenNewFileInZip2(
		m_zipFile,
		p_path.c_str(),
		p_fileInfo ->getZipFileInfo(),
		NULL, 0,
		NULL, 0,
		NULL,
		p_method,
		p_level,
		1 );

	if ( l_err != ZIP_OK )
		throw MinizipException( "zipOpenNewFileInZip2", l_err, __FILE__, __LINE__ );

	l_err = zipWriteInFileInZip( m_zipFile, p_buf, p_bufSize );

	if ( l_err != ZIP_OK )
		throw MinizipException( "zipWriteInFileInZip", l_err, __FILE__, __LINE__ );

	l_err = zipCloseFileInZipRaw(
		m_zipFile,
		( p_fileInfo ->getUnzFileInfo() ) ->uncompressed_size,
		( p_fileInfo ->getUnzFileInfo() ) ->crc );

	if ( l_err != ZIP_OK )
		throw MinizipException( "zipCloseFileInZipRaw", l_err, __FILE__, __LINE__ );
} // addEntryFromRaw

void ZipFile::addEntryFromFile(
	const string&	p_fileName,
	const string&	p_path,
	int				p_method,	// = Z_DEFLATED
	int				p_level	)	// = Z_DEFAULT_COMPRESSION
{
	size_t	l_bufSize;

	boost::scoped_array<byte>l_buf( OsFunc::readFile( p_fileName, l_bufSize ) );

	addEntryFromBuf( p_path, l_buf.get(), l_bufSize );
} // addEntryFromFile



void ZipFile::openStream(
	const string&	p_path,
	int				p_method,	// = Z_DEFLATED
	int				p_level )	// = Z_DEFAULT_COMPRESSION
{
	zip_fileinfo	l_fileInfo;
	int				l_err;

	if ( !m_fileOpen )
		throw ZipFileNotOpenException( __FILE__, __LINE__ );

	closeStream(); // closeStream only works if stream is open. no need to check twice if stream is open.

	l_fileInfo.internal_fa	= 0;
	l_fileInfo.external_fa	= 0;
	l_fileInfo.dosDate		= 0;
	l_fileInfo.tmz_date		= ZipFunc::getLocalTime( time( NULL ) );

	l_err = zipOpenNewFileInZip(
		m_zipFile,
		p_path.c_str(),
		&l_fileInfo,
		NULL, 0,
		NULL, 0,
		NULL,
		p_method,
		p_level );

	if ( l_err != ZIP_OK )
		throw MinizipException( "zipOpenNewFileInZip", l_err, __FILE__, __LINE__ );

	m_streamOpen	= true;
	m_streamPos		= 0;
} // openStream

void ZipFile::closeStream()
{
	if ( !m_streamOpen ) // stream cannot be open if zip file is not open so there's no need to check zip file is open.
		return;

	int l_err = zipCloseFileInZip( m_zipFile );

	if ( l_err != ZIP_OK )
		throw MinizipException( "zipCloseFileInZip", l_err, __FILE__, __LINE__ );

	m_streamOpen = false;
} // closeStream

void ZipFile::write( const char* p_buf, size_t p_bufSize )
{
	if ( !m_fileOpen )
		throw ZipFileNotOpenException( __FILE__, __LINE__ );

	if ( !m_streamOpen )
		throw ZipStreamNotOpenException( __FILE__, __LINE__ );

	int l_err = zipWriteInFileInZip( m_zipFile, ( const byte* ) p_buf, p_bufSize );

	if ( l_err != ZIP_OK )
		throw MinizipException( "zipWriteInFileInZip", l_err, __FILE__, __LINE__ );

	m_streamPos += p_bufSize;
} // write

void ZipFile::writePtr( void* p_ptr )
{
	writeNum( ( uint32 ) ptrToSeq( p_ptr ) );
} // writePtr

void ZipFile::writeStr( const string& p_str )
{
	size_t l_len = p_str.length();

	writeNum( ( uint32 ) l_len );
	write( p_str.c_str(), l_len );
} // writeStr

void ZipFile::writeStr( const char* p_str )
{
	size_t l_len = strlen( p_str );

	writeNum( ( uint32 ) l_len );
	write( p_str, l_len );
} // writeStr



ZipFile& ZipFile::operator << ( const char* p_str )
{
	write( p_str, strlen( p_str ) );

	return *this;
} // operator <<

ZipFile& ZipFile::operator << ( const string& p_str )
{
	write( p_str.c_str(), p_str.length() );

	return *this;
} // operator <<



bool ZipFile::isFileOpen() const
{
	return m_fileOpen;
} // isFileOpen

size_t ZipFile::getStreamPos() const
{
	return m_streamPos;
} // getStreamPos

string ZipFile::getFileName() const
{
	return m_fileName;
} // getFileName



uint32 ZipFile::ptrToSeq( void* p_ptr )
{
	map<void*, uint32>::iterator l_seqIterator = m_seqsByPtr.find( p_ptr );
	uint32 l_ptrSeq;

	if ( l_seqIterator == m_seqsByPtr.end() )
	{
		l_ptrSeq = m_ptrSeq++;
		m_seqsByPtr.insert( make_pair( p_ptr, l_ptrSeq ) );
	}

	else
		l_ptrSeq = l_seqIterator ->second;

	return l_ptrSeq;
} // ptrToSeq
