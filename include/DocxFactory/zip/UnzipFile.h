
#ifndef __DOCXFACTORY_UNZIP_FILE_H__
#define __DOCXFACTORY_UNZIP_FILE_H__

#include "DocxFactory/zip/ZipFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include "zlib/unzip.h"

#include "boost/type_traits.hpp"
#include "boost/utility/enable_if.hpp"

#include <map>
#include <string>


namespace DocxFactory
{
	using namespace std;

	class FileInfo;
	class UnzipFile
	{
	public:
		UnzipFile();
		virtual ~UnzipFile();

		void open( const string& p_fileName );
		void close();



		// functions for extracting file entries content from the zip file

		byte* extractEntryToBuf(
			const string&	p_path,
			size_t&			p_bufSize ) const;

		byte* extractEntryToRaw(
			const string&	p_path,
			int&			p_method,
			int&			p_level,
			FileInfo*&		p_fileInfo,
			size_t&			p_bufSize ) const;

		void extractEntryToFile(
			const string&	p_fileName,
			const string&	p_path ) const;



		// file streaming functions

		void openStream( const string& p_path );
		void closeStream();

		template <class T>
		typename boost::enable_if<boost::is_integral<T>, T>::type		readNum();

		template <class T>
		typename boost::enable_if<boost::is_floating_point<T>, T>::type	readNum();

		string	readStr	();
		void	read	( char* p_buf, size_t p_bufSize );

		void	insertPtrBySeq( uint32 p_key, void* p_ptr );
		const	map<uint32, void*>* getPtrsBySeq() const;

		bool	isFileOpen() const;
		string	getFileName() const;
		size_t	getStreamPos() const;
		const	map<string, FileInfo*>* getEntryList() const;

	protected:

	private:
		UnzipFile( const UnzipFile& p_other );
		UnzipFile operator = ( const UnzipFile& p_other );

		void fillEntryList();
		void clearEntryList();

		unzFile					m_unzipFile;
		string					m_fileName;

		bool					m_fileOpen;
		bool					m_streamOpen;
		size_t					m_streamPos;

		map<string, FileInfo*>	m_entryList;
		map<uint32, void*>		m_ptrsBySeq;

	};

	template <class T>
	typename boost::enable_if<boost::is_integral<T>, T>::type UnzipFile::readNum()
	{
		T l_retVal;

		read( ( char* ) &l_retVal, sizeof( l_retVal ) );
		
		ZipFunc::fromLittleEndian( l_retVal );

		return l_retVal;
	} // readNum<integral>

	template <class T>
	typename boost::enable_if<boost::is_floating_point<T>, T>::type UnzipFile::readNum()
	{
		DoublePack	l_pack;
		double		l_retVal;

		l_pack.m_exp	= readNum<int32>();
		l_pack.m_frac	= readNum<int64>();

		l_retVal = ZipFunc::unpack( l_pack );

		return (T) l_retVal;
	} // readNum<floating_point>

};

#endif
