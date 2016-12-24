
#ifndef __DOCXFACTORY_DOM_EXCEPTION_H__
#define __DOCXFACTORY_DOM_EXCEPTION_H__

#include "DocxFactory/xml/XmlException.h"



namespace DocxFactory
{
	using namespace std;

	class DomException : public XmlException
	{
	public:
		DomException(
			const string&	p_msg,
			const string&	p_file,
			const int		p_line );

		virtual ~DomException() throw();

	protected:

	private:

	};
};

#endif
