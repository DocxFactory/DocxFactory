
#ifndef __DOCXFACTORY_INVALID_FILE_URL_EXCEPTION_H__
#define __DOCXFACTORY_INVALID_FILE_URL_EXCEPTION_H__

#include "DocxFactory/os/OsException.h"



namespace DocxFactory
{
	using namespace std;

	class InvalidFileUrlException : public OsException
	{
	public:
		InvalidFileUrlException(
			const string&	p_url,
			const string&	p_file,
			const int		p_line );

		virtual ~InvalidFileUrlException() throw();

	protected:

	private:

	};
};

#endif
