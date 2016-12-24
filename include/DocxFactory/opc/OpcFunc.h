
#ifndef __DOCXFACTORY_OPC_FUNC_H__
#define __DOCXFACTORY_OPC_FUNC_H__

#include <string>



namespace DocxFactory
{
	using namespace std;

	class OpcFunc
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

		static string getRelativePath	( const string& p_sourceDir, const string& p_path );
		static string normalizePath		( const string& p_path );

		static string opcToZipPath		( const string& p_path );
		static string zipToOpcPath		( const string& p_path );

	protected:

	private:
		OpcFunc();
		OpcFunc( const OpcFunc& p_other );
		OpcFunc& operator = ( const OpcFunc& p_other );

	};
};

#endif
