
#ifndef __DOCXFACTORY_PARSER_FUNC_H__
#define __DOCXFACTORY_PARSER_FUNC_H__

#include <vector>
#include <string>



namespace DocxFactory
{
	using namespace std;

	class ParserFunc
	{
	public:
		enum SpaceHandle
		{
			SPACE_REMOVE,
			SPACE_TRIM,
			SPACE_KEEP
		};

		static vector<string>	parseWordList	( const string& p_str, SpaceHandle p_space, bool p_drill ); 
		static string			unquote			( const string& p_str );

	protected:

	private:
		ParserFunc();
		ParserFunc( const ParserFunc& p_other );
		ParserFunc& operator = ( const ParserFunc& p_other );

	};
};

#endif
