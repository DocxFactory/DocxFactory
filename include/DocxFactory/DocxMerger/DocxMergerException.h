
#ifndef __DOCXFACTORY_DOCX_MERGER_EXCEPTION_H__
#define __DOCXFACTORY_DOCX_MERGER_EXCEPTION_H__

#include "DocxFactory/util/DocxFactoryException.h"



namespace DocxFactory
{
	using namespace std;

	class DocxMergerException : public DocxFactoryException
	{
	public:
		virtual ~DocxMergerException() throw();

	protected:
		DocxMergerException( const string& p_file, int p_line );

	private:

	};
};

#endif
