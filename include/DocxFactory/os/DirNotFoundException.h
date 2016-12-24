
#ifndef __DOCXFACTORY_DIR_NOT_FOUND_EXCEPTION_H__
#define __DOCXFACTORY_DIR_NOT_FOUND_EXCEPTION_H__

#include "DocxFactory/os/OsException.h"



namespace DocxFactory
{
	using namespace std;

	class DirNotFoundException : public OsException
	{
	public:
		DirNotFoundException(
			const string&	p_dirName,
			const string&	p_file,
			const int		p_line );

		virtual ~DirNotFoundException() throw();

	protected:

	private:

	};
};

#endif
