
#ifndef __DOCXFACTORY_WORD_PROCESSING_COMPILER_H__
#define __DOCXFACTORY_WORD_PROCESSING_COMPILER_H__

#include <string>



namespace DocxFactory
{
	using namespace std;

	class WordProcessingCompiler
	{
	public:
		static WordProcessingCompiler& getInstance();
		virtual ~WordProcessingCompiler();

		void compile( const string& p_srcFileName, const string& p_dstFileName );

		// os functions
		void setTempDir();
		void setTempDir( const string& p_dir );

		string getWorkDir() const;
		string getTempDir() const;

	protected:

	private:
		WordProcessingCompiler();
		WordProcessingCompiler( const WordProcessingCompiler& p_other );
		WordProcessingCompiler& operator = ( const WordProcessingCompiler& p_other );

	};
};

#endif
