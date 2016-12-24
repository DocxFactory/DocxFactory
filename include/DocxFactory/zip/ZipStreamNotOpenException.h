
#ifndef __DOCXFACTORY_ZIP_STREAM_NOT_OPEN_EXCEPTION_H__
#define __DOCXFACTORY_ZIP_STREAM_NOT_OPEN_EXCEPTION_H__

#include "DocxFactory/zip/ZipException.h"



namespace DocxFactory
{
	using namespace std;

	class ZipStreamNotOpenException : public ZipException
	{
	public:
		ZipStreamNotOpenException(
			const string&	p_file,
			const int		p_line );

		virtual ~ZipStreamNotOpenException() throw();

	protected:

	private:

	};
};

#endif
