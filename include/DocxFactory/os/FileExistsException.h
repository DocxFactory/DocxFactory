
#ifndef __DOCXFACTORY_FILE_EXISTS_EXCEPTION_H__
#define __DOCXFACTORY_FILE_EXISTS_EXCEPTION_H__

#include "DocxFactory/os/OsException.h"



namespace DocxFactory
{
	using namespace std;

	class FileExistsException : public OsException
	{
	public:
		FileExistsException(
			const string&	p_fileName,
			const string&	p_file,
			const int		p_line );

		virtual ~FileExistsException() throw();

	protected:

	private:

	};
};

#endif
