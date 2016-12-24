
#ifndef __DOCXFACTORY_DOCX_COMPILER_ITEM_LAYOUT_H__
#define __DOCXFACTORY_DOCX_COMPILER_ITEM_LAYOUT_H__

#include "xercesc/util/XercesDefs.hpp"

#include <utility>
#include <vector>
#include <list>
#include <string>



XERCES_CPP_NAMESPACE_BEGIN

class DOMElement;
class DOMNode;

XERCES_CPP_NAMESPACE_END

namespace DocxFactory
{
	using namespace std;

	class ZipFile;

	class DocxCompilerItemFile;
	class DocxCompilerItemGroup;
	class DocxCompilerItemLayout
	{
	public:
		static DocxCompilerItemLayout* createItemLayout(
			DocxCompilerItemFile*	p_itemFile,
			xercesc::DOMElement*	p_containerNode,
			xercesc::DOMNode*		p_fromNode,
			double					p_fixedTrCnt );

		virtual ~DocxCompilerItemLayout();

		static void	serializeItemLayout(
			ZipFile*				p_zipFile,
			DocxCompilerItemLayout*	p_itemLayout );

		double																			getFixedTrCnt() const;
		const list<pair<bool, vector<pair<unsigned short, DocxCompilerItemLayout*>>>>*	getTrList() const;
		const list<DocxCompilerItemGroup*>*												getChildItemGroups() const;
		bool																			empty() const;

	protected:

	private:
		DocxCompilerItemLayout();
		DocxCompilerItemLayout(
			DocxCompilerItemFile*	p_itemFile,
			xercesc::DOMElement*	p_containerNode,
			xercesc::DOMNode*		p_fromNode,
			double					p_fixedTrCnt );

		DocxCompilerItemLayout( const DocxCompilerItemLayout& p_other );
		DocxCompilerItemLayout& operator = ( const DocxCompilerItemLayout& p_other );

		void serialize( ZipFile* p_zipFile );

		double																	m_fixedTrCnt;
		list<pair<bool, vector<pair<unsigned short, DocxCompilerItemLayout*>>>>	m_trList;
		list<DocxCompilerItemGroup*>											m_childItemGroups;

	};
};

#endif
