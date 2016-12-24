
#ifndef __DOCXFACTORY_MINIZIP_EXCEPTION_H__
#define __DOCXFACTORY_MINIZIP_EXCEPTION_H__

#include "DocxFactory/zip/ZipException.h"



namespace DocxFactory
{
	using namespace std;

	class MinizipException : public ZipException
	{
	public:
		MinizipException(
			const string&	p_funcName,
			const int		p_errorCode,
			const string&	p_file,
			const int		p_line );

		MinizipException(
			const string&	p_funcName,
			const string&	p_file,
			const int		p_line );

		virtual ~MinizipException() throw();

	protected:

	private:

	};
};

#endif
