
#include "DocxFactory/zip/UnzipFile.h"
#include "DocxFactory/zip/ZipFileNotOpenException.h"
#include "DocxFactory/zip/ZipStreamNotOpenException.h"
#include "DocxFactory/zip/ZipStreamOpenException.h"
#include "DocxFactory/zip/MinizipException.h"
#include "DocxFactory/zip/FileInfo.h"

#include "DocxFactory/os/OsFunc.h"
#include "DocxFactory/os/FileNotFoundException.h"
#include "DocxFactory/os/FileExistsException.h"

#include "boost/scoped_array.hpp"

using namespace DocxFactory;
using namespace std;



UnzipFile::UnzipFile()
{
	m_unzipFile		= NULL;
	m_fileName		= "";

	m_fileOpen		= false;
	m_streamOpen	= false;
} // c'tor

UnzipFile::~UnzipFile()
{
	close(); // close only works if zip file is open. no need to check twice if zip file is open.
} // d'tor

void UnzipFile::open( const string& p_fileName )
{
	close(); // close only works if zip file is open. no need to check twice if zip file is open.

	if ( !OsFunc::isFileExists( p_fileName ) )
		throw FileNotFoundException( p_fileName, __FILE__, __LINE__ );

	m_unzipFile	= unzOpen( p_fileName.c_str() );

	if ( !m_unzipFile )
		throw MinizipException( "unzOpen", __FILE__, __LINE__ );

	fillEntryList();

	m_fileName	= p_fileName;
	m_fileOpen	= true;

	m_ptrsBySeq.clear();
	insertPtrBySeq( 0, NULL );
} // open

void UnzipFile::close()
{
	if ( !m_fileOpen )
		return;

	closeStream(); // closeStream only works if stream is open. no need to check twice if stream is open.
	
	int l_err = unzClose( m_unzipFile );

	if ( l_err != ZIP_OK )
		throw MinizipException( "unzClose", l_err, __FILE__, __LINE__ );

	m_fileName	= "";
	m_fileOpen	= false;

	clearEntryList();
} // close



byte* UnzipFile::extractEntryToBuf(
	const string&	p_path,
	size_t&			p_bufSize ) const
{
	FileInfo*	l_fileInfo;
	byte*		l_buf = NULL;
	int			l_err;

	if ( !m_fileOpen )
		throw ZipFileNotOpenException( __FILE__, __LINE__ );

	if ( m_streamOpen )
		throw ZipStreamOpenException( __FILE__, __LINE__ );

	try
	{
		l_fileInfo = m_entryList.at( p_path ); // at throws out_of_range if entry doesn't exist

		l_err = unzLocateFile( m_unzipFile, p_path.c_str(), 0 );

		if ( l_err != UNZ_OK )
			throw MinizipException( "unzLocateFile", l_err, __FILE__, __LINE__ );

		l_err = unzOpenCurrentFile( m_unzipFile );

		if ( l_err != UNZ_OK )
			throw MinizipException( "unzOpenCurrentFile", l_err, __FILE__, __LINE__ );

		p_bufSize	= ( size_t ) ( ( l_fileInfo ->getUnzFileInfo() ) ->uncompressed_size );
		l_buf		= new byte[ p_bufSize ]; // will throw bad_alloc if allocation fails

		l_err = unzReadCurrentFile( m_unzipFile, l_buf, p_bufSize );

		if ( l_err < 0 )
			throw MinizipException( "unzReadCurrentFile", l_err, __FILE__, __LINE__ );

		l_err = unzCloseCurrentFile( m_unzipFile );

		if ( l_err != UNZ_OK )
			throw MinizipException( "unzCloseCurrentFile", l_err, __FILE__, __LINE__ );
	}

	catch ( ... )
	{
		if ( l_buf )
			delete[] l_buf;

		throw;
	}

	return l_buf;
} // extract

byte* UnzipFile::extractEntryToRaw(
	const string&	p_path,
	int&			p_method,
	int&			p_level,
	FileInfo*&		p_fileInfo,
	size_t&			p_bufSize ) const
{
	byte*	l_buf = NULL;
	int		l_err;

	if ( !m_fileOpen )
		throw ZipFileNotOpenException( __FILE__, __LINE__ );

	if ( m_streamOpen )
		throw ZipStreamOpenException( __FILE__, __LINE__ );

	try
	{
		p_fileInfo = m_entryList.at( p_path ); // at throws out_of_range if entry does not exist

		l_err = unzLocateFile( m_unzipFile, p_path.c_str(), 0 );

		if ( l_err != UNZ_OK )
			throw MinizipException( "unzLocateFile", l_err, __FILE__, __LINE__ );

		l_err = unzOpenCurrentFile2( m_unzipFile, &p_method, &p_level, 1 );

		if ( l_err != UNZ_OK )
			throw MinizipException( "unzOpenCurrentFile2", l_err, __FILE__, __LINE__ );

		p_bufSize	= ( size_t ) ( ( p_fileInfo ->getUnzFileInfo() ) ->compressed_size );
		l_buf		= new byte[ p_bufSize ]; // will throw bad_alloc if allocation fail

		l_err = unzReadCurrentFile( m_unzipFile, l_buf, p_bufSize );

		if ( l_err < 0 )
			throw MinizipException( "unzReadCurrentFile", l_err, __FILE__, __LINE__ );

		l_err = unzCloseCurrentFile( m_unzipFile );

		if ( l_err != UNZ_OK )
			throw MinizipException( "unzCloseCurrentFile", l_err, __FILE__, __LINE__ );
	}

	catch ( ... )
	{
		if ( l_buf )
			delete[] l_buf;

		throw;
	}

	return l_buf;
} // extractRaw

void UnzipFile::extractEntryToFile(
	const string&	p_fileName,
	const string&	p_path ) const
{
	size_t l_bufSize;

	boost::scoped_array<byte> l_buf( extractEntryToBuf( p_path, l_bufSize ) );

	OsFunc::writeFile( p_fileName, l_buf.get(), l_bufSize );
} // extractEntryToFile



void UnzipFile::openStream( const string& p_path )
{
	FileInfo*	l_fileInfo;
	int			l_err;
	
	if ( !m_fileOpen )
		throw ZipFileNotOpenException( __FILE__, __LINE__ );

	if ( m_streamOpen )
		throw ZipStreamOpenException( __FILE__, __LINE__ );

	l_fileInfo = m_entryList.at( p_path ); // at throws out_of_range if entry doesn't exist

	l_err = unzLocateFile( m_unzipFile, p_path.c_str(), 0 );

	if ( l_err != UNZ_OK )
		throw MinizipException( "unzLocateFile", l_err, __FILE__, __LINE__ );

	l_err = unzOpenCurrentFile( m_unzipFile );

	if ( l_err != UNZ_OK )
		throw MinizipException( "unzOpenCurrentFile", l_err, __FILE__, __LINE__ );

	m_streamOpen	= true;
	m_streamPos		= 0;
} // openStream

void UnzipFile::closeStream()
{
	if ( !m_streamOpen ) // stream cannot be open if zip file is not open so there's no need to check zip file is open.
		return;

	int l_err = unzCloseCurrentFile( m_unzipFile );
	
	if ( l_err != UNZ_OK )
		throw MinizipException( "unzCloseCurrentFile", l_err, __FILE__, __LINE__ );

	m_streamOpen = false;
} // closeStream



void UnzipFile::read( char* p_buf, size_t p_bufSize )
{
	if ( !m_fileOpen )
		throw ZipFileNotOpenException( __FILE__, __LINE__ );

	if ( !m_streamOpen )
		throw ZipStreamNotOpenException( __FILE__, __LINE__ );

	int l_err = unzReadCurrentFile( m_unzipFile, p_buf, p_bufSize );
	
	if ( l_err < 0 )
		throw MinizipException( "unzReadCurrentFile", l_err, __FILE__, __LINE__ );

	m_streamPos += p_bufSize;
} // read

string UnzipFile::readStr()
{
	uint32						l_bufSize	= readNum<uint32>();
	boost::scoped_array<char>	l_buf( new char[ l_bufSize + 1 ] );

	read( l_buf.get(), l_bufSize );
	l_buf[ l_bufSize ] = NULL;

	return l_buf.get();
} // readStr



void UnzipFile::fillEntryList()
{
	unz_global_info	l_globalInfo;
	unz_file_info	l_unzFileInfo;

	char*			l_fileName = NULL;
	int				l_err;
	unsigned int	i;

	try
	{
		clearEntryList();

		l_err = unzGetGlobalInfo( m_unzipFile, &l_globalInfo );

		if ( l_err != UNZ_OK )
			throw MinizipException( "unzGetGlobalInfo", l_err, __FILE__, __LINE__ );

		for ( i = 0; i < l_globalInfo.number_entry; ++i )
		{
			if ( i == 0 )
			{
				l_err = unzGoToFirstFile( m_unzipFile );

				if ( l_err != UNZ_OK )
					throw MinizipException( "unzGoToFirstFile", l_err, __FILE__, __LINE__ );
			}

			else
			{
				l_err = unzGoToNextFile( m_unzipFile );

				if ( l_err != UNZ_OK )
					throw MinizipException( "unzGoToNextFile", l_err, __FILE__, __LINE__ );
			}

			l_err = unzGetCurrentFileInfo(
				m_unzipFile,
				&l_unzFileInfo,
				NULL, 0,
				NULL, 0,
				NULL, 0 );

			if ( l_err != UNZ_OK )
				throw MinizipException( "unzGetCurrentFileInfo", l_err, __FILE__, __LINE__ );

			l_fileName = new char[ l_unzFileInfo.size_filename + 1 ]; // will throw bad_alloc if allocation fail

			l_err = unzGetCurrentFileInfo(
				m_unzipFile,
				&l_unzFileInfo,
				l_fileName, l_unzFileInfo.size_filename,
				NULL, 0,
				NULL, 0 );

			if ( l_err != UNZ_OK )
				throw MinizipException( "unzGetCurrentFileInfo", l_err, __FILE__, __LINE__ );

			l_fileName[ l_unzFileInfo.size_filename ] = '\0';

			m_entryList.insert( make_pair( string( l_fileName ), new FileInfo( l_unzFileInfo ) ) );

			delete[] l_fileName;
			l_fileName = NULL;
		}
	}

	catch ( ... )
	{
		if ( l_fileName )
			delete[] l_fileName;

		clearEntryList();
		throw;
	}
} // fillEntryList

void UnzipFile::clearEntryList()
{
	map<string, FileInfo*>::iterator	i;
	FileInfo*							l_entry;

	FOR_EACH( i, &m_entryList )
	{
		l_entry = ( *i ).second;
		delete l_entry;
	}

	m_entryList.clear();
} // clearEntryList



void UnzipFile::insertPtrBySeq( uint32 p_key, void* p_ptr )
{
	m_ptrsBySeq.insert( make_pair( p_key, p_ptr ) );
} // insertPtrBySeq

const map<uint32, void*>* UnzipFile::getPtrsBySeq() const
{
	return &m_ptrsBySeq;
} // getPtrByKey



bool UnzipFile::isFileOpen() const
{
	return m_fileOpen;
} // isFileOpen

size_t UnzipFile::getStreamPos() const
{
	return m_streamPos;
} // getStreamPos

string UnzipFile::getFileName() const
{
	return m_fileName;
} // getFileName

const map<string, FileInfo*>* UnzipFile::getEntryList() const
{
	return &m_entryList;
} // getEntryList
