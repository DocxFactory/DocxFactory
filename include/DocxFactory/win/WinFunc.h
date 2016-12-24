
#ifndef __DOCXFACTORY_WIN_FUNC_H__
#define __DOCXFACTORY_WIN_FUNC_H__

#if defined( _WIN32 ) || defined( _WIN64 )

#include "DocxFactory/str/StrEntries.h"

#include <string>



namespace DocxFactory
{
	using namespace std;

	class WinFunc
	{
	public:
		enum SubPathPos
		{
			SUBPATH_PROTOCOL,
			SUBPATH_HOST,
			SUBPATH_SHARED_DIR,
			SUBPATH_DRIVE,
			SUBPATH_DIR,
			SUBPATH_FILE,
			SUBPATH_EXT
		};

		static void breakPath(
			const string&		p_path,
			string&				p_protocol,
			string&				p_host,
			string&				p_sharedDir,
			string&				p_drive,
			string&				p_dir,
			string&				p_file,
			string&				p_ext );

		static string getSubPath(
			const string&		p_path,
			const SubPathPos&	p_from,
			const SubPathPos&	p_to );

		static string			getRelativePath	( const string& p_sourceDir, const string& p_path );
		static string			getFullPath		( const string& p_path );
		static string			normalizePath	( const string& p_path );

		static bool				isRelativePath	( const string& p_path );
		static bool				isDirExists		( const string& p_dir );

		static void				setTempDir();
		static void				setTempDir( const string& p_dir );

		static string			getWorkDir();
		static string			getTempDir();
		static StrEntries*		getPath();
		static string			getFolderPath( int p_CSIDL );

		static unsigned long	getMajorVersion();
		static unsigned long	getMinorVersion();
		static bool				isWorkStation();

	protected:

	private:
		WinFunc();
		WinFunc( const WinFunc& p_other );
		WinFunc& operator = ( const WinFunc& p_other );

		static void initialize();
		static void setWorkDir();
		static void setPath();

		static bool				m_initialized;
		static string			m_workDir;
		static string			m_tempDir;
		static StrEntries*		m_pathEntries;

		static unsigned long	m_majorVersion;
		static unsigned long	m_minorVersion;
		static char				m_productType;

	};
};

#endif
#endif
