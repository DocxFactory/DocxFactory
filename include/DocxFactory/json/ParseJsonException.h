
#ifndef __DOCXFACTORY_PARSE_JSON_EXCEPTION_H__
#define __DOCXFACTORY_PARSE_JSON_EXCEPTION_H__

#include "DocxFactory/json/JsonException.h"

#include <string>



namespace DocxFactory
{
	using namespace std;

	class ParseJsonException : public JsonException
	{
	public:
		ParseJsonException(
			const string&	p_file,
			const int		p_line );

		virtual ~ParseJsonException() throw();

	protected:

	private:

	};
};

#endif
