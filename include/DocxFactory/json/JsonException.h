
#ifndef __DOCXFACTORY_JSON_EXCEPTION_H__
#define __DOCXFACTORY_JSON_EXCEPTION_H__

#include "DocxFactory/util/DocxFactoryException.h"



namespace DocxFactory
{
	using namespace std;

	class JsonException : public DocxFactoryException
	{
	public:
		virtual ~JsonException() throw();

	protected:
		JsonException( const string& p_file, int p_line );

	private:

	};
};

#endif
