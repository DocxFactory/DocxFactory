
#ifndef __DOCXFACTORY_TEMPLATE_NOT_LOADED_EXCEPTION_H__
#define __DOCXFACTORY_TEMPLATE_NOT_LOADED_EXCEPTION_H__

#include "DocxFactory/WordProcessingMerger/WordProcessingException.h"



namespace DocxFactory
{
	using namespace std;

	class TemplateNotLoadedException : public WordProcessingException
	{
	public:
		TemplateNotLoadedException(
			const string&	p_file,
			const int		p_line );

		virtual ~TemplateNotLoadedException() throw();

	protected:

	private:

	};
};

#endif
