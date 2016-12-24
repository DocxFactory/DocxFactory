
#ifndef __DOCXFACTORY_ZIP_FILE_H__
#define __DOCXFACTORY_ZIP_FILE_H__

#include "DocxFactory/zip/ZipFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include "zlib/zip.h"
#include "zlib/unzip.h"

#include "boost/type_traits.hpp"
#include "boost/utility/enable_if.hpp"

#include <map>
#include <string>



namespace DocxFactory
{
	using namespace std;

	class FileInfo;

	class ZipFile
	{
	public:
		enum ZipWriteMode
		{
			ZIP_CREATE = 0,
			ZIP_APPEND = 2
		};

		ZipFile();
		virtual ~ZipFile();

		void open(
			const string&	p_fileName,
			ZipWriteMode	p_writeMode );

		void close();



		// functions for adding file entries and content to the zip file

		void addEntryFromBuf(
			const string&	p_path,
			const byte*		p_buf,
			size_t			p_bufSize,
			int				p_method	= Z_DEFLATED,
			int				p_level		= Z_DEFAULT_COMPRESSION ); // note that the default compression is not the fastest!

		void addEntryFromRaw(
			const string&	p_path,
			const byte*		p_buf,
			size_t			p_bufSize,
			int				p_method,
			int				p_level,
			FileInfo*		p_fileInfo );

		void addEntryFromFile(
			const string&	p_fileName,
			const string&	p_path,
			int				p_method	= Z_DEFLATED,
			int				p_level		= Z_DEFAULT_COMPRESSION );



		// file streaming functions

		void openStream(
			const string&	p_path,
			int				p_method	= Z_DEFLATED,
			int				p_level		= Z_DEFAULT_COMPRESSION );

		void closeStream();

		// write functions for serializing data structures.
		// at the moment, serializing functions are done in the zip, not a separate serializer or the compiler/merger.

		// write functions save atomic values that can later be completely retrieved.
		// e.g. strings are saved preceeded by their length so they can later be completely retrieved.

		// write functions save numeric values in the same byte order independently of the operating system
		// so the values can be retrieved from the file in the same way independently of where they were saved.

		template <class T>
		typename boost::enable_if<boost::is_integral<T>, void>::type		writeNum( T p_val );

		template <class T>
		typename boost::enable_if<boost::is_floating_point<T>, void>::type	writeNum( T p_val );

		void writePtr	( void*			p_ptr );
		void writeStr	( const string&	p_str );
		void writeStr	( const char*	p_str );
		void write		( const char*	p_buf, size_t p_bufSize );

		// i/o operators are designed for stream continuous data unlike write functions
		// e.g. for streaming xml files.

		ZipFile& operator << ( const char*		p_str );
		ZipFile& operator << ( const string&	p_str );

		bool	isFileOpen() const;
		string	getFileName() const;
		size_t	getStreamPos() const;

	protected:

	private:
		ZipFile( ZipFile& p_other );
		ZipFile operator = ( ZipFile& p_other );

		uint32 ptrToSeq( void* p_ptr );

		zipFile				m_zipFile;
		string				m_fileName;

		bool				m_fileOpen;
		bool				m_streamOpen;
		size_t				m_streamPos;

		map<void*, uint32>	m_seqsByPtr;
		uint32				m_ptrSeq;

	};

	template <class T>
	typename boost::enable_if<boost::is_integral<T>, void>::type ZipFile::writeNum( T p_val )
	{
		ZipFunc::toLittleEndian( p_val );

		write( ( char* ) &p_val, sizeof( p_val ) );
	} // writeNum<integral>

	template <class T>
	typename boost::enable_if<boost::is_floating_point<T>, void>::type ZipFile::writeNum( T p_val )
	{
		DoublePack l_pack = ZipFunc::pack( ( double ) p_val );

		writeNum( l_pack.m_exp );
		writeNum( l_pack.m_frac );
	} // writeNum<floating_point>

};

#endif
