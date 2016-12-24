
#ifndef __DOCXFACTORY_ZIP_FUNC_H__
#define __DOCXFACTORY_ZIP_FUNC_H__

#include "DocxFactory/util/DocxFactoryDefs.h"

#include "zlib/zip.h"

#include <string>
#include <ctime>



namespace DocxFactory
{
	using namespace std;

	struct DoublePack
	{
		int32	m_exp;
		int64	m_frac;
	};

	class ZipFunc
	{
	public:
		enum SubPathPos
		{
			SUBPATH_DIR,
			SUBPATH_FILE,
			SUBPATH_EXT
		};

		static void breakPath(
			const string&		p_path,
			string&				p_dir,
			string&				p_file,
			string&				p_ext );

		static string getSubPath(
			const string&		p_path,
			const SubPathPos&	p_from,
			const SubPathPos&	p_to );

		static string normalizePath( const string& p_path );

		// similar to std::localtime.
		// receives unix time and returns tm_zip struct instead of tm.
		static tm_zip getLocalTime( time_t p_time );



		// serializing functions.
		// at the moment, serializing functions are done in the zip, not a separate serializer or the compiler/merger.

		template<class T> static void toLittleEndian	( T& p_val );
		template<class T> static void fromLittleEndian	( T& p_val );

		static DoublePack	pack	( double p_num );
		static double		unpack	( const DoublePack p_dpack );
		static void			reverse	( char* p_buf, size_t p_bufSize );

	protected:

	private:
		ZipFunc();
		ZipFunc( const ZipFunc& p_other );
		ZipFunc& operator = ( const ZipFunc& p_other );

		static void initialize();
		static void setLittleEndian();

		static bool m_initialized;
		static bool m_littleEndian;
	};

	template<class T>
	void ZipFunc::toLittleEndian( T& p_val )
	{
		if ( !m_initialized )
			initialize();

		if ( m_littleEndian )
			return;

		reverse( ( char* ) p_val, sizeof( p_val ) );

		/*** return value version ***
		if ( m_littleEndian )
			return p_val;

		T		l_retVal;
		char*	l_src = ( char* ) &p_val;
		char*	l_dst = ( char* ) &l_retVal;

		for ( size_t i = 0; i < sizeof(T); ++i )
		{
			l_dst[ sizeof(T) - i - 1 ] = l_src[i];
		}

		return l_retVal;
		***/
	} // toLittleEndian

	template<class T>
	void ZipFunc::fromLittleEndian( T& p_val )
	{
		if ( !m_initialized )
			initialize();

		if ( m_littleEndian )
			return;

		reverse( ( char* ) &p_val, sizeof( p_val ) );
	} // fromLittleEndian

};

#endif
