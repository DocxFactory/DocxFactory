
#include "DocxFactory/json/JsonFunc.h"

#include "DocxFactory/locale/LocaleFunc.h"

using namespace DocxFactory;
using namespace std;



string JsonFunc::getStr( rapidjson::Value* p_value )
{	
	if ( p_value ->IsString() )
		return p_value ->GetString();

	else
	if ( p_value ->IsNumber() )
		return LocaleFunc::numToStr( p_value ->GetDouble() );

	else
	if ( p_value ->IsBool() )
		return ( p_value ->GetBool() ? "true" : "false" );

	return "";
} // getStr

double JsonFunc::getNum( rapidjson::Value* p_value )
{
	if ( p_value ->IsString() )
		return LocaleFunc::strToNum( p_value ->GetString() );

	else
	if ( p_value ->IsNumber() )
		return p_value ->GetDouble();

	else
	if ( p_value ->IsBool() )
		return ( p_value ->GetBool() ? 1.0f : 0.0f );

	return 0.0f;
} // getNum


