
#ifndef __DOCXFACTORY_PARSE_XML_FILE_EXCEPTION_H__
#define __DOCXFACTORY_PARSE_XML_FILE_EXCEPTION_H__

#include "DocxFactory/xml/ParseXmlException.h"



namespace DocxFactory
{
	using namespace std;

	class ParseXmlFileException : public ParseXmlException
	{
	public:
		ParseXmlFileException(
			const string&	p_fileName,
			const string&	p_file,
			const int		p_line );

		virtual ~ParseXmlFileException() throw();

	protected:

	private:

	};
};

#endif
