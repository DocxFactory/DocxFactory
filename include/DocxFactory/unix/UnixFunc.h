
#ifndef __DOCXFACTORY_UNIX_FUNC_H__
#define __DOCXFACTORY_UNIX_FUNC_H__

#ifdef __unix__

#include "DocxFactory/str/StrEntries.h"

#include <string>



namespace DocxFactory
{
	using namespace std;

	class UnixFunc
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

		static string			getSysName();
		static string			getNodeName();
		static string			getRelease();
		static unsigned long	getMajorRelease();
		static unsigned long	getMinorRelease();
		static string			getVersion();
		static string			getMachine();
		static string			getDomainName();

	protected:

	private:
		UnixFunc();
		UnixFunc( const UnixFunc& p_other );
		UnixFunc& operator = ( const UnixFunc& p_other );

		static void initialize();
		static void setWorkDir();
		static void setPath();

		static bool				m_initialized;
		static string			m_workDir;
		static string			m_tempDir;
		static StrEntries*		m_pathEntries;

		static string			m_sysName;
		static string			m_nodeName;
		static string			m_release;
		static unsigned long	m_majorRelease;
		static unsigned long	m_minorRelease;
		static string			m_version;
		static string			m_machine;
		static string			m_domainName;

	};
};

#endif
#endif
