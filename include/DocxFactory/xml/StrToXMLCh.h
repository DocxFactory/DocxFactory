
#ifndef __DOCXFACTORY_STR_TO_XMLCH_H__
#define __DOCXFACTORY_STR_TO_XMLCH_H__

#include "xercesc/util/XercesDefs.hpp"

#include <string>



namespace DocxFactory
{
	using namespace std;

	class StrToXMLCh
	{
	public:
		StrToXMLCh( const string& p_str );
		operator XMLCh*();

		virtual ~StrToXMLCh();

	protected:

	private:
		StrToXMLCh( const StrToXMLCh& p_other );
		StrToXMLCh operator = ( const StrToXMLCh& p_other );

		XMLCh* m_xmlCh;

	};
};

#endif
