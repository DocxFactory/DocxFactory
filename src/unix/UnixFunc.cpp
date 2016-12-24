
#ifdef __unix__

#include "DocxFactory/os/OsFunc.h"
#include "DocxFactory/os/DirNotFoundException.h"

#include "DocxFactory/unix/UnixFunc.h"

#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/str/StrEntries.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <dirent.h>
#include <sys/utsname.h>

using namespace DocxFactory;
using namespace std;



bool			UnixFunc::m_initialized		= false;
StrEntries*		UnixFunc::m_pathEntries		= NULL;
string			UnixFunc::m_workDir			= "";
string			UnixFunc::m_tempDir			= "";

string			UnixFunc::m_sysName			= "";
string			UnixFunc::m_nodeName		= "";
string			UnixFunc::m_release			= "";
unsigned long	UnixFunc::m_majorRelease	= 0;
unsigned long	UnixFunc::m_minorRelease	= 0;
string			UnixFunc::m_version			= "";
string			UnixFunc::m_machine			= "";
string			UnixFunc::m_domainName		= "";



void UnixFunc::breakPath(
	const string&	p_path,
	string&			p_dir,
	string&			p_file,
	string&			p_ext )
{
	if ( !m_initialized )
		initialize();

	string	l_path = p_path;
	size_t	i;

	p_dir	= "";
	p_file	= "";
	p_ext	= "";

	i = l_path.find_last_of( '/' );
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

string UnixFunc::getSubPath(
	const string&				p_path,
	const UnixFunc::SubPathPos&	p_from,
	const UnixFunc::SubPathPos&	p_to )
{
	if ( !m_initialized )
		initialize();

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

string UnixFunc::getRelativePath( const string& p_sourceDir, const string& p_path )
{
	size_t	l_sourceLen;
	size_t	l_pathLen;

	string	l_retVal	= "";
	size_t	i;
	size_t	j;

	// both cases are not valid and would cause errors.
	if ( p_sourceDir.empty() )	return p_path;
	if ( p_path.empty() )		return p_path;

	StrEntries	l_sourceDirEntries	( p_sourceDir[ p_sourceDir.length() - 1 ] == '/' ? p_sourceDir.substr( 0, p_sourceDir.length() - 1 ) : p_sourceDir, "/" );
	StrEntries	l_pathEntries		( p_path, "/" );

	l_sourceLen	= l_sourceDirEntries.getNumEntries();
	l_pathLen	= l_pathEntries.getNumEntries();

	i = 0;

	if ( p_sourceDir[0] == '/' && p_path[0] == '/'
	  || p_sourceDir[0] != '/' && p_path[0] != '/' )
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
			l_retVal += ( !l_retVal.empty() ? "/" : "" ) + string( ".." );
		}

		for ( j = i; j < l_pathLen; ++j )
		{
			l_retVal += ( !l_retVal.empty() ? "/" : "" ) + l_pathEntries.getEntry(j);
		}
	}

	return l_retVal;
} // getRelativePath

string UnixFunc::getFullPath( const string& p_path )
{
	if ( !m_initialized )
		initialize();

	string l_fullPath;

	if ( isRelativePath( p_path ) )
		 l_fullPath = normalizePath( getWorkDir() + "/" + p_path );

	else
		 l_fullPath = p_path;

	return l_fullPath;
} // getFullPath

string UnixFunc::normalizePath( const string& p_path )
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
	l_path = StrFunc::replace( l_path, "\\", "/" );

	while ( l_path.find( "//" ) != l_path.npos )
	{
		l_path = StrFunc::replace( l_path, "//", "/" );
	}

	StrEntries l_pathEntries( l_path, "/" );

	if ( l_pathEntries.lookup( "." )	!= StrEntries::npos
	  || l_pathEntries.lookup( ".." )	!= StrEntries::npos )
	{
		l_startIndex = 0;
		StrEntries l_retValEntries( "", "/" );

		if ( l_pathEntries.getEntry(0) == "" )
		{
			l_startIndex = 1;
			l_retValEntries.addEntry( "" );
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



bool UnixFunc::isRelativePath( const string& p_path )
{
	if ( !m_initialized )
		initialize();

	if ( p_path.length() >= 1 && p_path[0] == '/' )
		return false;

	return true;
} // isRelativePath

bool UnixFunc::isDirExists( const string& p_dir )
{
	if ( !m_initialized )
		initialize();

	bool	l_dirExists = false;
	DIR*	l_dir		= NULL;

	l_dir = opendir( p_dir.c_str() );
	if ( l_dir )
	{
		closedir( l_dir );
		l_dirExists = true;
	}

	return l_dirExists;
} // isDirExists



void UnixFunc::setTempDir()
{
	if ( !m_initialized )
		initialize();

	const char* l_tempDirEnv = getenv( "TMPDIR" );

	if ( l_tempDirEnv && isDirExists( l_tempDirEnv ) )
		m_tempDir = l_tempDirEnv;

	else if ( isDirExists( "/tmp" ) )
		m_tempDir = "/tmp";

	else
		m_tempDir = getWorkDir();
} // setTempDir()

void UnixFunc::setTempDir( const string& p_dir )
{
	if ( !m_initialized )
		initialize();

	string l_dir = normalizePath( p_dir );

	if ( !OsFunc::isDirExists( l_dir ) )
		throw DirNotFoundException( l_dir, __FILE__, __LINE__ );

	m_tempDir = l_dir;
} // setTempDir



string UnixFunc::getWorkDir()
{
	if ( !m_initialized )
		initialize();

	return m_workDir;
} // getWorkDir

string UnixFunc::getTempDir()
{
	if ( !m_initialized )
		initialize();

	return m_tempDir;
} // getTempDir

StrEntries* UnixFunc::getPath()
{
	if ( !m_initialized )
		initialize();

	return m_pathEntries;
} // getPath

string UnixFunc::getSysName()
{
	return m_sysName;
} // getSysName

string UnixFunc::getNodeName()
{
	return m_nodeName;
} // getNodeName

string UnixFunc::getRelease()
{
	return m_release;
} // getRelease

unsigned long UnixFunc::getMajorRelease()
{
	return m_majorRelease;
} // getMajorRelease

unsigned long UnixFunc::getMinorRelease()
{
	return m_minorRelease;
} // getMinorRelease

string UnixFunc::getVersion()
{
	return m_version;
} // getVersion

string UnixFunc::getMachine()
{
	return m_machine;
} // getMachine

string UnixFunc::getDomainName()
{
	return m_domainName;
} // getDomainName



void UnixFunc::initialize()
{
	m_initialized = true;

	setWorkDir();
	setTempDir();
	setPath();



	struct utsname	l_uname;
	string			l_release;
	string			str;
	size_t			i;

	if ( uname( &l_uname ) == 0 )
	{
		m_sysName		= l_uname.sysname;
		m_nodeName		= l_uname.nodename;
		m_release		= l_uname.release;
		m_version		= l_uname.version;
		m_machine		= l_uname.machine;
		m_domainName	= l_uname.domainname;

		l_release = m_release;
		if ( !l_release.empty() )
		{
			i = l_release.find_first_of( '-' );

			if( i != string::npos )
				l_release = l_release.substr( 0, i );



			if ( l_release.empty() )
				goto _end;

			i = l_release.find_first_of( '.' );
			if ( i != string::npos ) 
			{
				str			= l_release.substr( 0, i );
				l_release	= l_release.substr( i + 1 );
			}

			else
			{
				str = l_release;
				l_release.clear();
			}

			if( !StrFunc::isNum( str ) )
				goto _end;

			m_majorRelease = StrFunc::strToInt<unsigned long>( str );



			if ( l_release.empty() )
				goto _end;

			i = l_release.find_first_of( '.' );
			if ( i != string::npos ) 
			{
				str			= l_release.substr( 0, i );
				l_release	= l_release.substr( i + 1 );
			}

			else
			{
				str = l_release;
				l_release.clear();
			}

			if( !StrFunc::isNum( str ) )
				goto _end;

			m_minorRelease = StrFunc::strToInt<unsigned long>( str );

			_end:;
		}
	}
} // initialize

void UnixFunc::setWorkDir()
{
	if ( !m_initialized )
		initialize();

	char* l_buf;
	
	l_buf = new char[ FILENAME_MAX ];
	getcwd( l_buf, FILENAME_MAX );

	m_workDir = l_buf;
	delete[] l_buf;
} // setWorkDir()

void UnixFunc::setPath()
{
	if ( !m_initialized )
		initialize();

	string		l_path	= "";
	const char* l_buf;

	l_buf = getenv( "PATH" );
	if ( l_buf )
		l_path = l_buf;

	l_path = m_workDir
		+ ( !l_path.empty() ? ":" : "" ) 
		+ l_path;

	if ( m_pathEntries )
	{
		delete m_pathEntries;
		m_pathEntries = NULL;
	}

	m_pathEntries = new StrEntries( l_path, ":" );
} // setPath

#endif
