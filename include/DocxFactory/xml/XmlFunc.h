
#ifndef __DOCXFACTORY_XML_FUNC_H__
#define __DOCXFACTORY_XML_FUNC_H__

#include "DocxFactory/xml/AsciiToXMLCh.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include "xercesc/util/XercesDefs.hpp"

#include "unicode/ucnv.h"

#include <list>
#include <vector>
#include <string>



XERCES_CPP_NAMESPACE_BEGIN

class DOMLSParser;
class DOMDocument;
class DOMNode;
class DOMNodeList;
class DOMElement;

XERCES_CPP_NAMESPACE_END

namespace DocxFactory
{
	using namespace std;

	class XmlFunc
	{
	public:
		enum TagPos
		{
			TAG_UNDEFINED = -1,
			TAG_BEFORE,
			TAG_AFTER,
			TAG_START,
			TAG_END,
			TAG_EMPTY
		};

		// because the returned docs scope are tied to the parser scope
		// the parser is created independently and passed to the parse functions.

		// no validation is done on the file here (e.g. file exists).
		// all input should be validated in the top-level user function.
		static xercesc::DOMDocument* parseFileToDoc	( xercesc::DOMLSParser* p_parser, const string& p_fileName );
		static xercesc::DOMDocument* parseBufToDoc	( xercesc::DOMLSParser* p_parser, byte* p_buf, size_t p_bufSize );

		// no validation is done on the file here (e.g. dir exists).
		// all input should be validated in the top-level user function.
		static void saveDocToFile( const xercesc::DOMDocument* p_doc, const string& p_fileName );
		static byte* saveDocToBuf( const xercesc::DOMDocument* p_doc, size_t& p_bufSize );

		static xercesc::DOMElement* getChildByTagName(
			xercesc::DOMNode*	p_parentNode,
			xercesc::DOMNode*	p_fromChild,
			const string&		p_namespaceUri,
			const string&		p_tagNameList );

		static xercesc::DOMNode* getNextNode( xercesc::DOMNode* l_cursorNode );
		static xercesc::DOMNode* getPrevNode( xercesc::DOMNode* l_cursorNode );

		static bool getNextTag(
			xercesc::DOMElement*	p_containerNode,
			xercesc::DOMNode*&		p_cursorNode,
			TagPos&					p_cursorPos,
			string&					p_stringValue );

		static size_t				getChildNum		( xercesc::DOMNode* p_node );
		static list<size_t>			getNodePath		( xercesc::DOMNode* p_node );
		static xercesc::DOMNode*	getNodeByPath	( xercesc::DOMDocument* p_doc, list<size_t>* p_path );

		static void	unindentDoc( xercesc::DOMDocument* p_doc );
		static void removeAttrByNamespace(
			xercesc::DOMElement*	p_node,
			const string&			p_attrNsList,
			const string&			p_attrList );

		static void importAttributes( xercesc::DOMElement* p_sourceNode, xercesc::DOMElement* p_targetNode );

		static string	getXmlDeclaration( xercesc::DOMDocument* p_doc, bool p_standAlone = true );
		static string	normalizeNsPrefix( xercesc::DOMElement* p_rootNode, const string& p_nsPrefixDefault, const string& p_nsUri );

		// the function not only replaces special characters to xml entities,
		// it removes illegal characters that would corrupt the xml document
		// and normalizes all variations of newlines and newpages to "\n".
		static string	encodeXml( const string& p_str );

		static string	XMLChToUtf8	( const XMLCh*	p_xstr );
		static string	XMLChToStr	( const XMLCh*	p_xstr );
		static XMLCh*	utf8ToXMLCh	( const string&	p_str );
		static XMLCh*	strToXMLCh	( const string&	p_str );
		static bool		XMLChCmp	( const XMLCh*	p_xstr1, const XMLCh* p_xstr2 );

	protected:

	private:
		XmlFunc();
		XmlFunc( const XmlFunc& p_other );
		XmlFunc& operator = ( const XmlFunc& p_other );

		static XMLCh*	ucharToXMLCh( const UChar*	p_ustr, size_t p_len );
		static UChar*	XMLChToUChar( const XMLCh*	p_xstr, size_t p_len );
		static size_t	getXMLChLen	( const XMLCh*	p_xstr );

		static void unindentDocRecurr( const xercesc::DOMElement* p_parentNode );

	};
};

#endif
