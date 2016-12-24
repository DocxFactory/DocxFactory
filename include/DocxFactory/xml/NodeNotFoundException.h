
#ifndef __DOCXFACTORY_NODE_NOT_FOUND_EXCEPTION_H__
#define __DOCXFACTORY_NODE_NOT_FOUND_EXCEPTION_H__

#include "DocxFactory/xml/XmlException.h"



namespace DocxFactory
{
	using namespace std;

	class NodeNotFoundException : public XmlException
	{
	public:
		NodeNotFoundException(
			const string&	p_name,
			const string&	p_file,
			const int		p_line );

		virtual ~NodeNotFoundException() throw();

	protected:

	private:

	};
};

#endif
