
#ifndef __DOCXFACTORY_OPC_EXCEPTION_H__
#define __DOCXFACTORY_OPC_EXCEPTION_H__

#include "DocxFactory/util/DocxFactoryException.h"



namespace DocxFactory
{
	using namespace std;

	class OpcException : public DocxFactoryException
	{
	public:
		virtual ~OpcException() throw();

	protected:
		OpcException( const string& p_file, int p_line );

	private:

	};
};

#endif
