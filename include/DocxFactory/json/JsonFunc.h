#ifndef __DOCXFACTORY_JSON_FUNC_H__
#define __DOCXFACTORY_JSON_FUNC_H__

#include "rapidjson/document.h"

#include <string>



namespace DocxFactory
{	
	using namespace std;

	class JsonFunc
	{
	public:
		static string	getStr( rapidjson::Value* p_value );
		static double	getNum( rapidjson::Value* p_value );

	protected:

	private:
		JsonFunc();
		JsonFunc( const JsonFunc& p_other );
		JsonFunc& operator = ( const JsonFunc& p_other );

	};
};

#endif
