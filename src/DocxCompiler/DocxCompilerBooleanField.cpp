
#include "DocxFactory/DocxCompiler/DocxCompilerBooleanField.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/xml/XmlFunc.h"

#include "xercesc/dom/DOM.hpp"

using namespace DocxFactory;
using namespace std;



DocxCompilerBooleanField::DocxCompilerBooleanField(
	const string&			p_name,
	xercesc::DOMElement*	p_placeHolderNode,
	BooleanValue			p_value )

	: DocxCompilerField(
		NULL,
		TYPE_BOOLEAN,
		p_name,
		"",
		NULL,
		p_placeHolderNode )
{
	xercesc::DOMImplementation*	l_impl;
	l_impl	= xercesc::DOMImplementationRegistry::getDOMImplementation( _X( "ls" ) );

	m_doc	= l_impl ->createDocument();
	m_value	= p_value;
} // c'tor

DocxCompilerBooleanField::~DocxCompilerBooleanField()
{
	if( m_doc )
		m_doc ->release();
} // d'tor



void DocxCompilerBooleanField::serialize( ZipFile* p_zipFile )
{
	DocxCompilerField::serialize( p_zipFile );

	p_zipFile ->writeStr		( m_booleanString );
	p_zipFile ->writeNum<int16>	( m_value );
} // serialize



void DocxCompilerBooleanField::concatStr( const string& p_str )
{
	m_booleanString += p_str;
} // concatStr

xercesc::DOMDocument* DocxCompilerBooleanField::getDoc() const
{
	return m_doc;
} // getDoc

