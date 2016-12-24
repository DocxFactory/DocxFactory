
#ifndef __DOCXFACTORY_XML_EXCEPTION_H__
#define __DOCXFACTORY_XML_EXCEPTION_H__

#include "DocxFactory/util/DocxFactoryException.h"



namespace DocxFactory
{
	using namespace std;

	class XmlException : public DocxFactoryException
	{
	public:
		virtual ~XmlException() throw();

	protected:
		XmlException( const string& p_file, int p_line );

	private:

	};
};

#endif
