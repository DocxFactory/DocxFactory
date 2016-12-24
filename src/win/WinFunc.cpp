
#if defined( _WIN32 ) || defined( _WIN64 )

#include "DocxFactory/os/OsFunc.h"
#include "DocxFactory/os/DirNotFoundException.h"

#include "DocxFactory/win/WinFunc.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/str/StrEntries.h"

#include "DocxFactory/util/DocxFactoryDefs.h"
#include "DocxFactory/util/tchar.h"

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <Windows.h>
#include <ShlObj.h>
#include <direct.h>

using namespace DocxFactory;
using namespace std;



bool			WinFunc::m_initialized	= false;
StrEntries*		WinFunc::m_pathEntries	= NULL;
string			WinFunc::m_workDir		= "";
string			WinFunc::m_tempDir		= "";

unsigned long	WinFunc::m_majorVersion	= 0;
unsigned long	WinFunc::m_minorVersion	= 0;
char			WinFunc::m_productType	= 0;



void WinFunc::breakPath(
	const string&	p_path,
	string&			p_protocol,
	string&			p_host,
	string&			p_sharedDir,
	string&			p_drive,
	string&			p_dir,
	string&			p_file,
	string&			p_ext )
{
	if ( !m_initialized )
		initialize();

	string	l_path = p_path;
	size_t	i;

	p_protocol	= "";
	p_host		= "";
	p_sharedDir	= "";
	p_drive		= "";
	p_dir		= "";
	p_file		= "";
	p_ext		= "";

	if ( l_path.empty() )
		return;

	if ( l_path.length()		>= 2
	  && l_path.substr( 0, 2 )	== "\\\\" )
	{
		p_protocol	= l_path.substr( 0, 2 );
		l_path		= l_path.substr( 2 );

		if ( !l_path.empty() )
		{
			i = l_path.find( '\\' );
			if ( i == string::npos ) i = l_path.length();

			p_host		= l_path.substr( 0, i );
			l_path		= l_path.substr( i );

			if ( !l_path.empty() )
			{
				i = l_path.find( '\\' );
				if ( i == string::npos ) i = l_path.length();

				p_sharedDir	= l_path.substr( 0, i );
				l_path		= l_path.substr( i );
			}
		}
	}

	else
	if ( l_path.length() >= 2
	  && IS_LETTER( l_path[0] )
	  && l_path[1] == ':' )
	{
		p_drive	= l_path.substr( 0, 2 );
		l_path	= l_path.substr( 2 );
	}

	i = l_path.find_last_of( '\\' );
	if ( i != string::npos )
	{
		p_dir	= l_path.substr( 0, i + 1 );
		l_path	= l_path.substr( i + 1 );
	}

	i = l_path.find_last_of( '.' );
	if ( i != string::npos )
	{
		p_ext	= l_path.substr( i );
		l_path	= l_path.substr( 0, i );
	}

	p_file = l_path;
} // breakPath

string WinFunc::getSubPath(
	const string&				p_path,
	const WinFunc::SubPathPos&	p_from,
	const WinFunc::SubPathPos&	p_to )
{
	if ( !m_initialized )
		initialize();

	string			l_retVal	= "";
	string			l_protocol;
	string			l_host;
	string			l_sharedDir;
	string			l_drive;
	string			l_dir;
	string			l_file;
	string			l_ext;
	unsigned short	l_pos;

	breakPath(
		p_path,
		l_protocol,
		l_host,
		l_sharedDir,
		l_drive,
		l_dir,
		l_file,
		l_ext );

	for ( l_pos = p_from; l_pos <= p_to; ++l_pos )
	{
		switch ( SubPathPos( l_pos ) )
		{
		case SUBPATH_PROTOCOL	: l_retVal += l_protocol;	break;
		case SUBPATH_HOST		: l_retVal += l_host;		break;
		case SUBPATH_SHARED_DIR	: l_retVal += l_sharedDir;	break;
		case SUBPATH_DRIVE		: l_retVal += l_drive;		break;
		case SUBPATH_DIR		: l_retVal += l_dir;		break;
		case SUBPATH_FILE		: l_retVal += l_file;		break;
		case SUBPATH_EXT		: l_retVal += l_ext;		break;
		}
	}

	return l_retVal;
} // getSubPath

string WinFunc::getRelativePath( const string& p_sourceDir, const string& p_path )
{
	size_t	l_sourceLen;
	size_t	l_pathLen;

	string	l_retVal	= "";
	size_t	i;
	size_t	j;

	// both cases are not valid and would cause errors.
	if ( p_sourceDir.empty() )	return p_path;
	if ( p_path.empty() )		return p_path;

	StrEntries	l_sourceDirEntries	( p_sourceDir[ p_sourceDir.length() - 1 ] == '\\' ? p_sourceDir.substr( 0, p_sourceDir.length() - 1 ) : p_sourceDir, "\\" );
	StrEntries	l_pathEntries		( p_path, "/" );

	l_sourceLen	= l_sourceDirEntries.getNumEntries();
	l_pathLen	= l_pathEntries.getNumEntries();

	i = 0;

	if ( p_sourceDir[0] == '\\' && p_path[0] == '\\'
	  || p_sourceDir[0] != '\\' && p_path[0] != '\\' )
	{
		while ( i < l_sourceLen
			 && i < l_pathLen
			 && l_sourceDirEntries.getEntry(i) == l_pathEntries.getEntry(i) )
		{
			++i;
		}
	}

	if ( i == 0 )
		l_retVal = p_path;

	else
	{
		for ( j = i; j < l_sourceLen; ++j )
		{
			l_retVal += ( !l_retVal.empty() ? "\\" : "" ) + string( ".." );
		}

		for ( j = i; j < l_pathLen; ++j )
		{
			l_retVal += ( !l_retVal.empty() ? "\\" : "" ) + l_pathEntries.getEntry(j);
		}
	}

	return l_retVal;
} // getRelativePath

string WinFunc::getFullPath( const string& p_path )
{
	if ( !m_initialized )
		initialize();

	string l_fullPath;
	
	if ( isRelativePath( p_path ) )
		 l_fullPath = normalizePath( m_workDir + '\\' + p_path );

	else	
 		 l_fullPath = p_path;

	return l_fullPath;
} // getFullPath

string WinFunc::normalizePath( const string& p_path )
{
	if ( !m_initialized )
		initialize();

	string	l_retVal;
	string	l_path;
	string	l_dir;
	size_t	l_dirIndex;
	size_t	l_startIndex;
	size_t	i;

	l_path = StrFunc::trim( p_path );
	l_path = StrFunc::replace( l_path, "/", "\\" );

	// the reason to remove duplicate slashes only after the second position (1) 
	// is because there can be a unc \\host\shared_dir\file
	while ( ( i = l_path.find( "\\\\", 1 ) ) != l_path.npos )
	{
		l_path.replace( i, 2, "\\" );
	}

	while ( l_path.find( "::" ) != l_path.npos )
	{
		l_path = StrFunc::replace( l_path, "::", ":" );
	}

	if ( l_path.length() >= 2
	  && IS_LETTER( l_path[0] )
	  && l_path[1] == ':' )
	{
		l_path = string( 1, toupper( l_path[0] ) ) + ':' + l_path.substr(2);
	}

	StrEntries l_pathEntries( l_path, "\\" );

	if ( l_pathEntries.lookup( "." )	!= StrEntries::npos
	  || l_pathEntries.lookup( ".." )	!= StrEntries::npos )
	{
		l_startIndex = 0;
		StrEntries l_retValEntries( "", "\\" );

		if ( l_path.length()		>= 2
		  && l_path.substr( 0, 2 )	== "\\\\" )
		{
			l_startIndex = 2;
			l_retValEntries.addEntry( "" );
			l_retValEntries.addEntry( "" );
		} 

		else 
		if ( l_path.length() >= 2
		  && IS_LETTER ( l_path[0] )
		  && l_path[1] == ':' )
		{
			l_startIndex = 1;
			l_retValEntries.addEntry( l_path.substr( 0, 2 ) );
		}

		for ( l_dirIndex = l_startIndex; l_dirIndex < l_pathEntries.getNumEntries(); ++l_dirIndex )
		{
			l_dir = l_pathEntries.getEntry( l_dirIndex );

			if ( l_dir == "." )
			{
				if ( !l_path.empty() )
					continue;

				else
					l_retValEntries.addEntry( l_dir );
			}

			else if ( l_dir == ".." )
			{
				i = l_retValEntries.getNumEntries();

				if ( i > 0 && l_retValEntries.getEntry( i - 1 ) != ".." )
					l_retValEntries.deleteEntry( i - 1 );

				else
					l_retValEntries.addEntry( l_dir );
			}

			else
				l_retValEntries.addEntry( l_dir );
		}

		l_retVal = l_retValEntries.getStr();
	}

	else
		l_retVal = l_path;

	return l_retVal;
} // normalizePath



bool WinFunc::isRelativePath( const string& p_path )
{
	if ( !m_initialized )
		initialize();

	if ( p_path.empty() )
		return false;

	if ( p_path.length() >= 2 
	  && p_path[0] == '\\'
	  && p_path[1] == '\\'

	  || p_path.length() >= 3
	  && IS_LETTER( p_path[0] )
	  && p_path[1] == ':'
	  && p_path[2] == '\\' )
		return false;

	return true;
} // isRelativePath

bool WinFunc::isDirExists( const string& p_dir )
{
	if ( !m_initialized )
		initialize();

	DWORD l_attr = GetFileAttributesA( p_dir.c_str() );

	if ( l_attr != INVALID_FILE_ATTRIBUTES && l_attr & FILE_ATTRIBUTE_DIRECTORY )
		 return true;

	return false;
} // isDirExists



void WinFunc::setTempDir()
{
	if ( !m_initialized )
		initialize();

	string		l_tempDir	= "";
	TCHAR		l_tempDirBuf[ MAX_PATH ];
	DWORD		l_result;

	const char*	l_getEnvBuf;

	l_result = GetTempPath( MAX_PATH, l_tempDirBuf );

	if ( l_result > 0 || l_result <= MAX_PATH )
	{
		l_tempDir = LocaleFunc::convertToUtf8( tstring( l_tempDirBuf ) );
	}

	if ( l_tempDir.empty() )
	{
		l_getEnvBuf = getenv( "temp" );
		if ( l_getEnvBuf )
			l_tempDir = l_getEnvBuf;
	}

	if ( l_tempDir.empty() )
		l_tempDir = m_workDir;

	m_tempDir = l_tempDir;
} // setTempDir()

void WinFunc::setTempDir( const string& p_dir )
{
	if ( !m_initialized )
		initialize();

	string l_dir = normalizePath( p_dir );

	if ( !OsFunc::isDirExists( l_dir ) )
		throw DirNotFoundException( l_dir, __FILE__, __LINE__ );

	m_tempDir = l_dir;
} // setTempDir



string WinFunc::getWorkDir()
{
	if ( !m_initialized )
		initialize();

	return m_workDir;
} // getWorkDir

string WinFunc::getTempDir()
{
	if ( !m_initialized )
		initialize();

	return m_tempDir;
} // getTempDir

StrEntries* WinFunc::getPath()
{
	if ( !m_initialized )
		initialize();

	return m_pathEntries;
} // getPath

string WinFunc::getFolderPath( int p_CSIDL )
{
	if ( !m_initialized )
		initialize();

	string	l_path		= "";
	TCHAR	l_buf[ MAX_PATH ];
	HRESULT	l_result	= SHGetFolderPath( 0, p_CSIDL, 0, 0, l_buf );

	if ( !FAILED( l_result ) )
	{
		l_path = LocaleFunc::convertToUtf8( tstring( l_buf ) );
	}

	return l_path;
} // getFolderPath

unsigned long WinFunc::getMajorVersion()
{
	if ( !m_initialized )
		initialize();

	return m_majorVersion;
} // getMajorVersion

unsigned long WinFunc::getMinorVersion()
{
	if ( !m_initialized )
		initialize();

	return m_minorVersion;
} // getMinorVersion

bool WinFunc::isWorkStation()
{
	if ( !m_initialized )
		initialize();

	return ( m_productType == VER_NT_WORKSTATION );
} // isWorkStation



void WinFunc::initialize()
{
	m_initialized = true;

	setWorkDir();
	setTempDir();
	setPath();



	LPTSTR lpszFilePath = _T( "kernel32.dll" );

	DWORD dwDummy;
	DWORD dwFVISize = GetFileVersionInfoSize( lpszFilePath, &dwDummy );

	LPBYTE lpVersionInfo = new BYTE[ dwFVISize ];

	GetFileVersionInfo( lpszFilePath, 0 , dwFVISize, lpVersionInfo );

	UINT uLen;
	VS_FIXEDFILEINFO *lpFfi;

	VerQueryValue( lpVersionInfo, _T("\\"), ( LPVOID* ) &lpFfi, &uLen );

   	m_majorVersion = ( unsigned long ) HIWORD( lpFfi->dwFileVersionMS );
	m_minorVersion = ( unsigned long ) LOWORD( lpFfi->dwFileVersionMS );

	delete [] lpVersionInfo;



    OSVERSIONINFOEX l_versionInfo;

    ZeroMemory( &l_versionInfo, sizeof( OSVERSIONINFOEX ) );
    l_versionInfo.dwOSVersionInfoSize = sizeof( OSVERSIONINFOEX );

    if ( GetVersionEx( ( OSVERSIONINFO* ) &l_versionInfo ) )
	{
		if( m_majorVersion == 0 )
			m_majorVersion = ( unsigned long ) l_versionInfo.dwMajorVersion;

		if( m_minorVersion == 0 )
			m_minorVersion = ( unsigned long ) l_versionInfo.dwMinorVersion;

		m_productType = ( unsigned char ) l_versionInfo.wProductType;
	}
} // initialize

void WinFunc::setWorkDir()
{
	if ( !m_initialized )
		initialize();

	char* l_buf;
	
	l_buf = new char[ FILENAME_MAX ];
	_getcwd( l_buf, FILENAME_MAX );

	m_workDir = l_buf;
	delete[] l_buf;
} // setWorkDir()

void WinFunc::setPath()
{
	if ( !m_initialized )
		initialize();

	string		l_path	= "";
	const char* l_buf;

	l_buf = getenv( "path" );
	if ( l_buf )
		l_path = l_buf;

	l_path = m_workDir
		+ ( !l_path.empty() ? ";" : "" ) 
		+ l_path;

	if ( m_pathEntries )
	{
		delete m_pathEntries;
		m_pathEntries = NULL;
	}

	m_pathEntries = new StrEntries( l_path, ";" );
} // setPath

#endif
