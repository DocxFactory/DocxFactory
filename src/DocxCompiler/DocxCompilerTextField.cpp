
#include "DocxFactory/DocxCompiler/DocxCompilerTextField.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

using namespace DocxFactory;
using namespace std;



DocxCompilerTextField::DocxCompilerTextField(
	DocxCompilerItem*				p_item,
	const string&					p_name,
	const string&					p_format,
	const map<FieldParam, string>*	p_valueByParam,
	xercesc::DOMElement*			p_placeHolderNode )

	: DocxCompilerField(
		p_item,
		TYPE_TEXT,
		p_name,
		p_format,
		p_valueByParam,
		p_placeHolderNode )
{

} // c'tor

DocxCompilerTextField::~DocxCompilerTextField()
{

} // d'tor



void DocxCompilerTextField::serialize( ZipFile* p_zipFile )
{
	DocxCompilerField::serialize( p_zipFile );
} // serialize
