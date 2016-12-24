
#ifndef __DOCXFACTORY_PARSE_XML_EXCEPTION_H__
#define __DOCXFACTORY_PARSE_XML_EXCEPTION_H__

#include "DocxFactory/xml/XmlException.h"



namespace DocxFactory
{
	using namespace std;

	class ParseXmlException : public XmlException
	{
	public:
		ParseXmlException(
			const string&	p_file,
			const int		p_line );

		virtual ~ParseXmlException() throw();

	protected:

	private:

	};
};

#endif
