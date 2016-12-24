
#ifndef __DOCXFACTORY_FILE_NOT_REMOVED_EXCEPTION_H__
#define __DOCXFACTORY_FILE_NOT_REMOVED_EXCEPTION_H__

#include "DocxFactory/os/OsException.h"



namespace DocxFactory
{
	using namespace std;

	class FileNotRemovedException : public OsException
	{
	public:
		FileNotRemovedException(
			const string&	p_fileName,
			const string&	p_file,
			const int		p_line );

		virtual ~FileNotRemovedException() throw();

	protected:

	private:

	};
};

#endif
