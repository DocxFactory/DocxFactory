
#include "DocxFactory/zip/ZipFunc.h"

#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/str/StrEntries.h"

#include "DocxFactory/util/DocxFactoryDefs.h"

using namespace DocxFactory;
using namespace std;



bool ZipFunc::m_initialized		= false;
bool ZipFunc::m_littleEndian	= false;



void ZipFunc::breakPath(
	const string&	p_path,
	string&			p_dir,
	string&			p_file,
	string&			p_ext )
{
	if ( !m_initialized )
		initialize();

	string	l_path = p_path;
	size_t	i;

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

string ZipFunc::getSubPath(
	const string&				p_path,
	const ZipFunc::SubPathPos&	p_from,
	const ZipFunc::SubPathPos&	p_to )
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

	l_retVal = "";

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

string ZipFunc::normalizePath( const string& p_path )
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

	// if begins with / delete
	if ( l_path.length() > 1 && l_path[0] == '/' )
		 l_path = l_path.substr(1);

	StrEntries l_pathEntries( l_path, "/" );

	if ( l_pathEntries.lookup( "." )	!= StrEntries::npos
	  || l_pathEntries.lookup( ".." )	!= StrEntries::npos )
	{
		l_startIndex = 0;
		StrEntries l_retValEntries( "", "/" );

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



tm_zip ZipFunc::getLocalTime( time_t p_time )
{
	if ( !m_initialized )
		initialize();

	tm_zip	l_retVal;
	tm*		l_tm	= localtime( &p_time );

	l_retVal.tm_hour	= ( uInt ) l_tm ->tm_hour;
	l_retVal.tm_mday	= ( uInt ) l_tm ->tm_mday;
	l_retVal.tm_min		= ( uInt ) l_tm ->tm_min;
	l_retVal.tm_mon		= ( uInt ) l_tm ->tm_mon;
	l_retVal.tm_sec		= ( uInt ) l_tm ->tm_sec;
	l_retVal.tm_year	= ( uInt ) l_tm ->tm_year;

	return l_retVal;
} // getLocalTime



#define FRAC_MAX 9223372036854775807LL // 2^63 - 1
// ggg not working in gcc *** #define FRAC_MAX 9223372036854775807i64 // 2^63 - 1

DoublePack ZipFunc::pack( double p_num )
{
	if ( !m_initialized )
		initialize();

	DoublePack	l_dPack;
	double		l_num;

	if ( p_num == 0 )
	{
		l_dPack.m_frac	= 0;
		l_dPack.m_exp	= 0;
	}

	else
	{
		l_num			= fabs( frexp( p_num, &l_dPack.m_exp ) ) - 0.5;
		l_dPack.m_frac	= 1 + ( int64 )( l_num * 2.0 * ( FRAC_MAX - 1 ) );

		if ( p_num < 0.0 )
			l_dPack.m_frac = - l_dPack.m_frac;
	}

	return l_dPack;
} // pack

double ZipFunc::unpack( const DoublePack p_dpack )
{
	if ( !m_initialized )
		initialize();

	double l_num;
	double l_retVal;

	if ( p_dpack.m_frac == 0 )
		l_retVal = 0;

	else
	{
		l_num = ( ( double ) ( llabs ( p_dpack.m_frac ) - 1 ) / ( FRAC_MAX - 1 ) ) / 2.0;

		l_retVal = ldexp( l_num + 0.5, p_dpack.m_exp );

		if ( p_dpack.m_frac < 0 )
			l_retVal = -l_retVal;
	}

	return l_retVal;
} // unpack

#undef FRAC_MAX



void ZipFunc::reverse( char* p_buf, size_t p_bufSize )
{
	if ( !m_initialized )
		initialize();

	char*	l_tempBuf = new char[ p_bufSize ];
	size_t	i;

	memcpy( l_tempBuf, p_buf, p_bufSize );

	for ( i = 0; i < p_bufSize; ++i )
	{
		p_buf[i] = l_tempBuf[ p_bufSize - i - 1 ];
	}

	delete[] l_tempBuf;
} // reverse



void ZipFunc::initialize()
{
	m_initialized = true;

	setLittleEndian();
} // initialize

void ZipFunc::setLittleEndian()
{
	if ( !m_initialized )
		initialize();

	int i = 1;
	char* ch = ( char* ) &i;

	if ( *ch == 1 )
		m_littleEndian = true;

	else
		m_littleEndian = false;
} // setLittleEndian()