
#ifndef __DOCXFACTORY_INVALID_XML_PATH_EXCEPTION_H__
#define __DOCXFACTORY_INVALID_XML_PATH_EXCEPTION_H__

#include "DocxFactory/xml/XmlException.h"



namespace DocxFactory
{
	using namespace std;

	class InvalidXmlPathException : public XmlException
	{
	public:
		InvalidXmlPathException(
			const string&	p_file,
			const int		p_line );

		virtual ~InvalidXmlPathException() throw();

	protected:

	private:

	};
};

#endif
