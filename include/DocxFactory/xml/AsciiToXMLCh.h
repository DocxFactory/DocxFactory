
#ifndef __DOCXFACTORY_ASCII_TO_XMLCH_H__
#define __DOCXFACTORY_ASCII_TO_XMLCH_H__

#include "xercesc/util/XercesDefs.hpp"

#include <string>



// intended for literal ascii values only

#ifdef _NATIVE_WCHAR_T_DEFINED
#define _X( str ) L ## str
#else
#define _X( str ) AsciiToXMLCh( str )
#endif



namespace DocxFactory
{
	using namespace std;

	class AsciiToXMLCh
	{
	public:
		AsciiToXMLCh( const string& p_str );
		operator XMLCh*();

		virtual ~AsciiToXMLCh();

	protected:

	private:
		AsciiToXMLCh( const AsciiToXMLCh& p_other );
		AsciiToXMLCh operator = ( const AsciiToXMLCh& p_other );

		XMLCh* m_xmlCh;

	};
};

#endif
