
#ifndef __DOCXFACTORY_XML_TREE_DRILLER_H__
#define __DOCXFACTORY_XML_TREE_DRILLER_H__

#include "xercesc/util/XercesDefs.hpp"

#include <set>
#include <string>



XERCES_CPP_NAMESPACE_BEGIN

class DOMNode;

XERCES_CPP_NAMESPACE_END

namespace DocxFactory
{
	using namespace std;

	class XmlTreeDriller
	{
	public:
		enum CursorPos
		{
			CURSOR_NODE,
			CURSOR_AFTER,
			CURSOR_BEFORE
		};

		XmlTreeDriller( xercesc::DOMNode* p_rootNode,

			const string& p_drillTagNsList,		const string& p_drillTagList,
			const string& p_tagNsList,			const string& p_tagList,
			const string& p_attrNsList = "",	const string& p_attrList = "", const string& p_attrValueList = "" );

		virtual ~XmlTreeDriller();

		xercesc::DOMNode* nextNode();

		void deleteNode();
		void setCurrentNode( xercesc::DOMNode* p_cursorNode, CursorPos p_cursorPos = CURSOR_NODE );

		xercesc::DOMNode* getRoot() const;
		xercesc::DOMNode* getCurrentNode() const;

	protected:

	private:
		xercesc::DOMNode*	drillNode();

		xercesc::DOMNode*	m_rootNode;
		xercesc::DOMNode*	m_cursorNode;
		CursorPos			m_cursorPos;

		set<string>	m_drillTagNsList;
		set<string>	m_drillTagList;
		set<string>	m_tagNsList;
		set<string>	m_tagList;
		set<string>	m_attrNsList;
		set<string>	m_attrList;
		set<string>	m_attrValueList;

		bool		m_drillTagNsAll;
		bool		m_drillTagAll;
		bool		m_tagNsAll;
		bool		m_tagAll;
		bool		m_attrNsAll;
		bool		m_attrAll;
		bool		m_attrValueAll;
	};
};

#endif
