
#ifndef __DOCXFACTORY_ZIP_EXCEPTION_H__
#define __DOCXFACTORY_ZIP_EXCEPTION_H__

#include "DocxFactory/util/DocxFactoryException.h"



namespace DocxFactory
{
	using namespace std;

	class ZipException : public DocxFactoryException
	{
	public:
		virtual ~ZipException() throw();

	protected:
		ZipException( const string& p_file, int p_line );

	private:

	};
};

#endif
