
#ifndef __DOCXFACTORY_OS_EXCEPTION_H__
#define __DOCXFACTORY_OS_EXCEPTION_H__

#include "DocxFactory/util/DocxFactoryException.h"



namespace DocxFactory
{
	using namespace std;

	class OsException : public DocxFactoryException
	{
	public:
		virtual ~OsException() throw();

	protected:
		OsException( const string& p_file, int p_line );

	private:

	};
};

#endif
