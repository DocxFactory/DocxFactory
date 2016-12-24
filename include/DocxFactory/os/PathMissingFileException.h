
#ifndef __DOCXFACTORY_PATH_MISSING_FILE_EXCEPTION_H__
#define __DOCXFACTORY_PATH_MISSING_FILE_EXCEPTION_H__

#include "DocxFactory/os/OsException.h"



namespace DocxFactory
{
	using namespace std;

	class PathMissingFileException : public OsException
	{
	public:
		PathMissingFileException(
			const string&	p_path,
			const string&	p_file,
			const int		p_line );

		virtual ~PathMissingFileException() throw();

	protected:

	private:

	};
};

#endif
