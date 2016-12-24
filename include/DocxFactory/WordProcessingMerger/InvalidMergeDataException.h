
#ifndef __DOCXFACTORY_INVALID_MERGE_DATA_EXCEPTION_H__
#define __DOCXFACTORY_INVALID_MERGE_DATA_EXCEPTION_H__

#include "DocxFactory/WordProcessingMerger/WordProcessingException.h"

#include <string>



namespace DocxFactory
{
	using namespace std;

	class InvalidMergeDataException : public WordProcessingException
	{
	public:
		InvalidMergeDataException(
			const string&	p_file,
			const int		p_line );

		virtual ~InvalidMergeDataException() throw();

	protected:

	private:

	};
};

#endif
