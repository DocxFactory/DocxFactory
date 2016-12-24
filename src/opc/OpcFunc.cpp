
#include "DocxFactory/opc/OpcFunc.h"

#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/str/StrEntries.h"

#include "DocxFactory/util/DocxFactoryDefs.h"

using namespace DocxFactory;
using namespace std;



void OpcFunc::breakPath(
	const string&	p_path,
	string&			p_dir,
	string&			p_file,
	string&			p_ext )
{
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

string OpcFunc::getSubPath(
	const string&				p_path,
	const OpcFunc::SubPathPos&	p_from,
	const OpcFunc::SubPathPos&	p_to )
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

string OpcFunc::getRelativePath( const string& p_sourceDir, const string& p_path )
{
	size_t	l_sourceLen;
	size_t	l_pathLen;

	string	l_retVal = "";
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

string OpcFunc::normalizePath( const string& p_path )
{
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

			else
			if ( l_dir == ".." )
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



string OpcFunc::opcToZipPath( const string& p_path )
{
	if ( p_path.empty() )
		return "";

	return p_path.substr(1);
} // opcToZipPath

string OpcFunc::zipToOpcPath( const string& p_path )
{
	return '/' + p_path;
} // zipToOpcPath

