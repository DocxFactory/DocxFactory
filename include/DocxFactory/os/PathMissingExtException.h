
#ifndef __DOCXFACTORY_PATH_MISSING_EXT_EXCEPTION_H__
#define __DOCXFACTORY_PATH_MISSING_EXT_EXCEPTION_H__

#include "DocxFactory/os/OsException.h"



namespace DocxFactory
{
	using namespace std;

	class PathMissingExtException : public OsException
	{
	public:
		PathMissingExtException(
			const string&	p_path,
			const string&	p_file,
			const int		p_line );

		virtual ~PathMissingExtException() throw();

	protected:

	private:

	};
};

#endif
