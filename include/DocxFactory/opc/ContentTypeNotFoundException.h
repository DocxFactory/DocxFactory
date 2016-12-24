
#ifndef __DOCXFACTORY_CONTENT_TYPE_NOT_FOUND_EXCEPTION_H__
#define __DOCXFACTORY_CONTENT_TYPE_NOT_FOUND_EXCEPTION_H__

#include "DocxFactory/opc/OpcException.h"



namespace DocxFactory
{
	using namespace std;

	class ContentTypeNotFoundException : public OpcException
	{
	public:
		ContentTypeNotFoundException(
			const string&	p_contentType,
			const string&	p_file,
			const int		p_line );

		virtual ~ContentTypeNotFoundException() throw();

	protected:

	private:

	};
};

#endif
