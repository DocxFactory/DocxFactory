
#ifndef __DOCXFACTORY_PART_TYPE_NOT_FOUND_EXCEPTION_H__
#define __DOCXFACTORY_PART_TYPE_NOT_FOUND_EXCEPTION_H__

#include "DocxFactory/opc/OpcException.h"



namespace DocxFactory
{
	using namespace std;

	class PartTypeNotFoundException : public OpcException
	{
	public:
		PartTypeNotFoundException(
			const string&	p_type,
			const string&	p_file,
			const int		p_line );

		virtual ~PartTypeNotFoundException() throw();

	protected:

	private:

	};
};

#endif
