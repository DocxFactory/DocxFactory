
#ifndef __DOCXFACTORY_COMPILE_VERSION_MISMATCH_EXCEPTION_H__
#define __DOCXFACTORY_COMPILE_VERSION_MISMATCH_EXCEPTION_H__

#include "DocxFactory/WordProcessingMerger/WordProcessingException.h"

#include <string>



namespace DocxFactory
{
	using namespace std;

	class CompileVersionMismatchException : public WordProcessingException
	{
	public:
		CompileVersionMismatchException(
			const string&	p_file,
			const int		p_line );

		virtual ~CompileVersionMismatchException() throw();

	protected:

	private:

	};
};

#endif
