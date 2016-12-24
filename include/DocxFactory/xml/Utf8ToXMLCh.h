
#ifndef __DOCXFACTORY_UTF8_TO_XMLCH_H__
#define __DOCXFACTORY_UTF8_TO_XMLCH_H__

#include "xercesc/util/XercesDefs.hpp"

#include <string>



namespace DocxFactory
{
	using namespace std;

	class Utf8ToXMLCh
	{
	public:
		Utf8ToXMLCh( const string& p_str );
		operator XMLCh*();

		virtual ~Utf8ToXMLCh();

	protected:

	private:
		Utf8ToXMLCh( const Utf8ToXMLCh& p_other );
		Utf8ToXMLCh operator = ( const Utf8ToXMLCh& p_other );

		XMLCh* m_xmlCh;

	};
};

#endif
