
#ifndef __DOCXFACTORY_FILE_NOT_OPEN_EXCEPTION_H__
#define __DOCXFACTORY_FILE_NOT_OPEN_EXCEPTION_H__

#include "DocxFactory/os/OsException.h"



namespace DocxFactory
{
	using namespace std;

	class FileNotOpenException : public OsException
	{
	public:
		FileNotOpenException(
			const string&	p_fileName,
			const string&	p_file,
			const int		p_line );

		virtual ~FileNotOpenException() throw();

	protected:

	private:

	};
};

#endif
