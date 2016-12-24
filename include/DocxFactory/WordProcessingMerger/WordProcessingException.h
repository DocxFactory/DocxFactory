
#ifndef __DOCXFACTORY_WORD_PROCESSING_EXCEPTION_H__
#define __DOCXFACTORY_WORD_PROCESSING_EXCEPTION_H__

#include "DocxFactory/util/DocxFactoryException.h"



namespace DocxFactory
{
	using namespace std;

	class WordProcessingException : public DocxFactoryException
	{
	public:
		virtual ~WordProcessingException() throw();

	protected:
		WordProcessingException( const string& p_file, int p_line );

	private:

	};
};

#endif
