
#include "DocxFactory/os/OsFunc.h"
#include "DocxFactory/os/FileNotOpenException.h"
#include "DocxFactory/os/FileNotFoundException.h"
#include "DocxFactory/os/FileNotRemovedException.h"
#include "DocxFactory/os/InvalidFileUrlException.h"

#if defined( _WIN32 ) || defined( _WIN64 )
#include "DocxFactory/win/WinFunc.h"
#else
#include "DocxFactory/unix/UnixFunc.h"
#endif

#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/str/StrEntries.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <ctime>

using namespace DocxFactory;
using namespace std;



void OsFunc::breakPath(
	const string&	p_path,
	string&			p_dir,
	string&			p_file,
	string&			p_ext )
{
	#if defined( _WIN32 ) || defined( _WIN64 )
		string l_protocol;
		string l_host;
		string l_sharedDir;
		string l_drive;

		WinFunc::breakPath(
			p_path,
			l_protocol,
			l_host,
			l_sharedDir,
			l_drive,
			p_dir,
			p_file,
			p_ext );

		p_dir = l_protocol + l_host + l_sharedDir + l_drive + p_dir;
	#else
		UnixFunc::breakPath(
			p_path,
			p_dir,
			p_file,
			p_ext );
	#endif
} // breakPath

string OsFunc::getSubPath(
	const string&		p_path,
	const SubPathPos&	p_from,
	const SubPathPos&	p_to )
{
	string			l_retVal = "";
	string			l_dir;
	string			l_file;
	string			l_ext;
	unsigned short	l_pos;

	breakPath(
		p_path, 
		l_dir, 
		l_file, 
		l_ext );

	for ( l_pos = p_from; l_pos <= p_to; ++l_pos )
	{
		switch ( SubPathPos( l_pos ) )
		{
		case SUBPATH_DIR	: l_retVal += l_dir;	break;
		case SUBPATH_FILE	: l_retVal += l_file;	break;
		case SUBPATH_EXT	: l_retVal += l_ext;	break;
		}
	}

	return l_retVal;
} // getSubPath

string OsFunc::getRelativePath( const string& p_sourceDir, const string& p_path )
{
	#if defined( _WIN32 ) || defined( _WIN64 )
		return WinFunc::getRelativePath( p_sourceDir, p_path );
	#else
		return UnixFunc::getRelativePath( p_sourceDir, p_path );
	#endif
} // getFullPath

string OsFunc::getFullPath( const string& p_path )
{
	#if defined( _WIN32 ) || defined( _WIN64 )
		return WinFunc::getFullPath( p_path );
	#else
		return UnixFunc::getFullPath( p_path );
	#endif
} // getFullPath

string OsFunc::normalizePath( const string& p_path )
{
	#if defined( _WIN32 ) || defined( _WIN64 )
		return WinFunc::normalizePath( p_path );
	#else
		return UnixFunc::normalizePath( p_path );
	#endif
} // normalizePath

string OsFunc::searchPath( const string& p_fileName )
{
	#if defined( _WIN32 ) || defined( _WIN64 )
		StrEntries* l_pathEntries = WinFunc::getPath();
	#else
		StrEntries* l_pathEntries = UnixFunc::getPath();
	#endif

	string	l_retVal	= "";
	string	l_fullPath;
	size_t	l_pos;

	for ( l_pos = 0; l_pos < l_pathEntries ->getNumEntries(); ++l_pos )
	{
		l_fullPath = normalizePath( l_pathEntries ->getEntry( l_pos ) + '/' + p_fileName );

		if ( OsFunc::isFileExists( l_fullPath ) )
		{
			l_retVal = l_fullPath;
			break;
		}
	}

	return l_retVal;
} // searchPath



bool OsFunc::isRelativePath( const string& p_path )
{
	#if defined( _WIN32 ) || defined( _WIN64 )
		return WinFunc::isRelativePath( p_path );
	#else
		return UnixFunc::isRelativePath( p_path );
	#endif
} // isRelativePath

bool OsFunc::isFileExists( const string& p_fileName )
{
	ifstream l_fileStream;

	l_fileStream.open( p_fileName.c_str(), ios::in );

	if ( l_fileStream.fail() )
	return false;

	l_fileStream.close();
	return true;
} // isFileExists

bool OsFunc::isDirExists( const string& p_dir )
{
	#if defined( _WIN32 ) || defined( _WIN64 )
		return WinFunc::isDirExists( p_dir );
	#else
		return UnixFunc::isDirExists( p_dir );
	#endif
} // isDirExists



void OsFunc::breakFileUrl(
	const string&	p_url,
	string&			p_protocol,
	string&			p_host,
	string&			p_path )
{
	string	l_url = p_url;
	size_t	i;

	p_protocol	= "";
	p_host		= "";
	p_path		= "";

	if ( !StrFunc::begins( StrFunc::lc( l_url ), "file://" ) )
		throw InvalidFileUrlException( l_url, __FILE__, __LINE__ );

	p_protocol	= l_url.substr( 0, 7 );
	l_url		= l_url.substr( 7 );

	if ( !l_url.empty() )
	{
		i = l_url.find( "/" );
		if ( i == l_url.npos ) i = l_url.length();

		p_host	= l_url.substr( 0, i );
		l_url	= l_url.substr( i );

		if ( !l_url.empty() )
		{
			p_path	= l_url;
		}
	}
} // breakFileUrl

string OsFunc::fileUrlToPath( const string& p_url )
{
	string l_protocol;
	string l_host;
	string l_path;

	breakFileUrl(
		p_url,
		l_protocol,
		l_host,
		l_path );

	l_path = StrFunc::replace( l_path, "%20", " " );

	#if defined( _WIN32 ) || defined( _WIN64 )

		if ( l_path.length() > 1 && l_path[0] == '/' )
			l_path = l_path.substr(1);

		if ( l_host.empty() )
			l_host = "localhost";

		if ( l_host != "localhost" || l_path.length() < 2 || !IS_LETTER( l_path[0] ) || l_path[1] != ':' )
			l_path = "\\\\" + l_host + "\\" + l_path;

		return WinFunc::normalizePath( l_path );
	#else
		return UnixFunc::normalizePath( l_path );
	#endif
} // fileUrlToPath

string OsFunc::normalizeFileUrl( const string& p_url )
{
	string	l_url = StrFunc::trim( p_url );
	size_t	i;

	if ( l_url.empty() )
		return l_url;

	if ( !StrFunc::begins( StrFunc::lc( l_url ), "file://" ) )
		return l_url;

	if ( l_url.length() > 7 )
	{
		l_url = l_url.substr(7);
		l_url = StrFunc::replace( l_url, "\\", "/" );

		while ( l_url.find( "//" ) != l_url.npos )
		{
			l_url = StrFunc::replace( l_url, "//", "/" );
		}

		while ( l_url.find( "::" ) != l_url.npos )
		{
			l_url = StrFunc::replace( l_url, "::", ":" );
		}

		l_url = StrFunc::replace( l_url, " ", "%20" );
		l_url = "file://" + l_url;

		i = l_url.find( "/", 7 );

		if ( i != l_url.npos
		  && i + 2 < l_url.length()
		  && IS_LETTER( l_url[ i + 1 ] )
		  && l_url[ i + 2 ] == '|' )

			l_url[ i + 2 ] = ':';
	}

	return l_url;
} // normalizeFileUrl



byte* OsFunc::readFile(
	const string&	p_fileName,
	size_t&			p_bufSize )
{
	ifstream	l_fileStream;
	byte*		l_buf	= NULL;

	try
	{
		l_fileStream.open( p_fileName.c_str(), ios::in | ios::binary | ios::ate );

		if ( l_fileStream.fail() )
			throw FileNotFoundException( p_fileName, __FILE__, __LINE__ );

		p_bufSize	= ( size_t ) l_fileStream.tellg();
		l_buf		= new byte[ p_bufSize ];

		l_fileStream.seekg( 0, ios::beg );
		l_fileStream.read( ( char* ) l_buf, p_bufSize );
		l_fileStream.close();
	}

	catch ( ... )
	{
		if ( l_buf )
			delete[] l_buf;

		if ( l_fileStream.is_open() )
			l_fileStream.close();

		throw;
	}

	return l_buf;
} // readFile

void OsFunc::writeFile(
	const string&	p_fileName,
	const byte*		p_buf,
	size_t			p_bufSize,
	bool			p_append )
{
	ofstream l_fileStream;

	try
	{
		if ( p_append )
			l_fileStream.open( p_fileName.c_str(), ios::out | ios::binary | ios::app );
		else 
			l_fileStream.open( p_fileName.c_str(), ios::out | ios::binary | ios::trunc );

		if ( l_fileStream.fail() )
			throw FileNotOpenException( p_fileName, __FILE__, __LINE__ );

		l_fileStream.write( ( char* ) p_buf, p_bufSize );
		l_fileStream.close();
	}

	catch ( ... )
	{
		if ( l_fileStream.is_open() )
			l_fileStream.close();
	}
} // writeFile

void OsFunc::copy( const string& p_srcFileName, const string& p_dstFileName )
{
	#if defined( _WIN32 ) || defined( _WIN64 )
		const size_t l_bufSize = 4096;
	#else
		const size_t l_bufSize = 8192;
	#endif

	FILE*	l_srcFile	= NULL;
	FILE*	l_dstFile	= NULL;

	char	l_buf[ l_bufSize ];
	size_t	l_size;

	try
	{
		l_srcFile = fopen( p_srcFileName.c_str(), "rb" );
		if ( !l_srcFile ) throw FileNotOpenException( p_srcFileName, __FILE__, __LINE__ );

		l_dstFile = fopen( p_dstFileName.c_str(), "wb" );
		if ( !l_dstFile ) throw FileNotOpenException( p_dstFileName, __FILE__, __LINE__ );
	
		while ( l_size = fread( l_buf, 1, l_bufSize, l_srcFile ) )
		{
			fwrite( l_buf, 1, l_size, l_dstFile );
		}

		fclose( l_srcFile );
		fclose( l_dstFile );
	}

	catch ( ... )
	{
		if ( l_srcFile ) fclose( l_srcFile );
		if ( l_dstFile ) fclose( l_dstFile );
	}
} // copy

void OsFunc::remove( const string& p_fileName )
{
	if ( std::remove( p_fileName.c_str() ) != 0 )
		throw FileNotRemovedException( p_fileName, __FILE__, __LINE__ );
} // remove



void OsFunc::setTempDir()
{
	#if defined( _WIN32 ) || defined( _WIN64 )
		return WinFunc::setTempDir();
	#else
		return UnixFunc::setTempDir();
	#endif
} // setTempDir()

void OsFunc::setTempDir( const string& p_dir )
{
	#if defined( _WIN32 ) || defined( _WIN64 )
		return WinFunc::setTempDir( p_dir );
	#else
		return UnixFunc::setTempDir( p_dir );
	#endif
} // setTempDir



string OsFunc::getWorkDir()
{
	#if defined( _WIN32 ) || defined( _WIN64 )
		return WinFunc::getWorkDir();
	#else
		return UnixFunc::getWorkDir();
	#endif
} // getWorkDir

string OsFunc::getTempDir()
{
	#if defined( _WIN32 ) || defined( _WIN64 )
		return WinFunc::getTempDir();
	#else
		return UnixFunc::getTempDir();
	#endif
} // getTempDir

string OsFunc::getTempFile( const string& p_ext )
{
	string	l_fileName;
	int		l_num;
	char	l_buf[6];

	srand( ( unsigned int ) time( NULL ) );

	do
	{
		l_num = rand() % 10000;
		sprintf( l_buf, "%05d", l_num );
		l_fileName = normalizePath( getTempDir() + "/p" + l_buf + "tmp" + p_ext );
	} while ( isFileExists( l_fileName ) );

	return l_fileName;
} // getTempFile
