
#ifndef __DOCXFACTORY_PARSE_JSON_FILE_EXCEPTION_H__
#define __DOCXFACTORY_PARSE_JSON_FILE_EXCEPTION_H__

#include "DocxFactory/json/ParseJsonException.h"

#include <string>



namespace DocxFactory
{
	using namespace std;

	class ParseJsonFileException : public ParseJsonException
	{
	public:
		ParseJsonFileException(
			const string&	p_fileName,
			const string&	p_file,
			const int		p_line );

		virtual ~ParseJsonFileException() throw();

	protected:

	private:

	};
};

#endif
