
#ifndef __DOCXFACTORY_OS_FUNC_H__
#define __DOCXFACTORY_OS_FUNC_H__

#include "DocxFactory/util/DocxFactoryDefs.h"

#include <string>



namespace DocxFactory
{
	using namespace std;

	class OsFunc
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

		static string	getRelativePath	( const string& p_sourceDir, const string& p_path );
		static string	getFullPath		( const string& p_path );
		static string	normalizePath	( const string& p_path );
		static string	searchPath		( const string& p_fileName );

		static bool		isRelativePath	( const string& p_path );
		static bool		isFileExists	( const string& p_fileName );
		static bool		isDirExists		( const string& p_dir );

		static void breakFileUrl(
			const string&	p_url,
			string&			p_protocol,
			string&			p_host,
			string&			p_path );

		static string fileUrlToPath		( const string& p_url );
		static string normalizeFileUrl	( const string& p_url );

		static byte* readFile(
			const string&	p_fileName,
			size_t&			p_bufSize );

		static void writeFile(
			const string&	p_fileName,
			const byte*		p_buf,
			size_t			p_bufSize,
			bool			p_append = false );

		static void copy	( const string& p_srcFileName, const string& p_dstFileName );
		static void remove	( const string& p_fileName );

		static void		setTempDir();
		static void		setTempDir( const string& p_dir );

		static string	getWorkDir();
		static string	getTempDir();
		static string	getTempFile( const string& p_ext );

	protected:

	private:
		OsFunc();
		OsFunc( const OsFunc& p_other );
		OsFunc& operator = ( const OsFunc& p_other );

	};
};

#endif
