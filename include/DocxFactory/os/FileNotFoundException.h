
#ifndef __DOCXFACTORY_FILE_NOT_FOUND_EXCEPTION_H__
#define __DOCXFACTORY_FILE_NOT_FOUND_EXCEPTION_H__

#include "DocxFactory/os/OsException.h"



namespace DocxFactory
{
	using namespace std;

	class FileNotFoundException : public OsException
	{
	public:
		FileNotFoundException(
			const string&	p_fileName,
			const string&	p_file,
			const int		p_line );

		virtual ~FileNotFoundException() throw();

	protected:

	private:

	};
};

#endif
