
#ifndef __DOCXFACTORY_ZIP_FILE_NOT_OPEN_EXCEPTION_H__
#define __DOCXFACTORY_ZIP_FILE_NOT_OPEN_EXCEPTION_H__

#include "DocxFactory/zip/ZipException.h"



namespace DocxFactory
{
	using namespace std;

	class ZipFileNotOpenException : public ZipException
	{
	public:
		ZipFileNotOpenException(
			const string&	p_file,
			const int		p_line );

		virtual ~ZipFileNotOpenException() throw();

	protected:

	private:

	};
};

#endif
