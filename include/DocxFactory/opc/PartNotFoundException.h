
#ifndef __DOCXFACTORY_PART_NOT_FOUND_EXCEPTION_H__
#define __DOCXFACTORY_PART_NOT_FOUND_EXCEPTION_H__

#include "DocxFactory/opc/OpcException.h"



namespace DocxFactory
{
	using namespace std;

	class PartNotFoundException : public OpcException
	{
	public:
		PartNotFoundException(
			const string&	p_fullPath,
			const string&	p_file,
			const int		p_line );

		virtual ~PartNotFoundException() throw();

	protected:

	private:

	};
};

#endif
