
#ifndef __DOCXFACTORY_UNSUPPORTED_FILE_TYPE_EXCEPTION_H__
#define __DOCXFACTORY_UNSUPPORTED_FILE_TYPE_EXCEPTION_H__

#include "DocxFactory/os/OsException.h"



namespace DocxFactory
{
	using namespace std;

	class UnsupportedFileTypeException : public OsException
	{
	public:
		UnsupportedFileTypeException(
			const string&	p_fileName,
			const string&	p_fileTypes,
			const string&	p_file,
			const int		p_line );

		virtual ~UnsupportedFileTypeException() throw();

	protected:

	private:

	};
};

#endif
