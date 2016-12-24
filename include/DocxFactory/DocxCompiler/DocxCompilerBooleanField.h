
#ifndef __DOCXFACTORY_DOCX_COMPILER_BOOLEAN_FIELD_H__
#define __DOCXFACTORY_DOCX_COMPILER_BOOLEAN_FIELD_H__

#include "DocxFactory/DocxCompiler/DocxCompilerField.h"

#include "xercesc/util/XercesDefs.hpp"



XERCES_CPP_NAMESPACE_BEGIN

class DOMDocument;
class DOMElement;

XERCES_CPP_NAMESPACE_END

namespace DocxFactory
{
	using namespace std;

	class ZipFile;

	class DocxCompilerBooleanField : public DocxCompilerField
	{
	public:
		enum BooleanValue
		{
			BOOLEAN_FALSE		= 0,
			BOOLEAN_TRUE		= 1,
			BOOLEAN_UNDEFINED
		};

		DocxCompilerBooleanField(
			const string&			p_name,
			xercesc::DOMElement*	p_placeHolderNode,
			BooleanValue			p_value );

		virtual ~DocxCompilerBooleanField();

		virtual void serialize( ZipFile* p_zipFile );

		void concatStr( const string& p_str );

		xercesc::DOMDocument* getDoc() const;

	protected:

	private:
		DocxCompilerBooleanField( const DocxCompilerBooleanField& p_other );
		DocxCompilerBooleanField& operator = ( const DocxCompilerBooleanField& p_other );

		xercesc::DOMDocument*	m_doc;

		string					m_booleanString;
		BooleanValue			m_value;

	};
};

#endif
