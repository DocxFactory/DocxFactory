
#ifndef __DOCXFACTORY_ZIP_STREAM_OPEN_EXCEPTION_H__
#define __DOCXFACTORY_ZIP_STREAM_OPEN_EXCEPTION_H__

#include "DocxFactory/zip/ZipException.h"



namespace DocxFactory
{
	using namespace std;

	class ZipStreamOpenException : public ZipException
	{
	public:
		ZipStreamOpenException(
			const string&	p_file,
			const int		p_line );

		virtual ~ZipStreamOpenException() throw();

	protected:

	private:

	};
};

#endif
