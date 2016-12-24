
#ifndef __DOCXFACTORY_GENERAL_EXCEPTION_H__
#define __DOCXFACTORY_GENERAL_EXCEPTION_H__

#include "DocxFactory/util/DocxFactoryException.h"

#include <string>



namespace DocxFactory
{
	using namespace std;

	class GeneralException : public DocxFactoryException
	{
	public:
		GeneralException(
			const string&	p_msg,
			const string&	p_file,
			const int		p_line );

		virtual ~GeneralException() throw();

	protected:

	private:

	};
};

#endif
