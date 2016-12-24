
#ifndef __DOCXFACTORY_INVALID_ITEM_NAME_EXCEPTION_H__
#define __DOCXFACTORY_INVALID_ITEM_NAME_EXCEPTION_H__

#include "DocxFactory/WordProcessingMerger/WordProcessingException.h"

#include <string>



namespace DocxFactory
{
	using namespace std;

	class InvalidItemNameException : public WordProcessingException
	{
	public:
		InvalidItemNameException(
			const string&	p_itemName,
			const string&	p_file,
			const int		p_line );

		virtual ~InvalidItemNameException() throw();

	protected:

	private:

	};
};

#endif
