
#ifndef __DOCXFACTORY_DOCX_COMPILER_ITEM_H__
#define __DOCXFACTORY_DOCX_COMPILER_ITEM_H__

#include "xercesc/util/XercesDefs.hpp"

#include <map>
#include <set>
#include <list>
#include <string>



XERCES_CPP_NAMESPACE_BEGIN

class DOMDocument;
class DOMElement;
class DOMNode;

XERCES_CPP_NAMESPACE_END

namespace DocxFactory
{
	using namespace std;

	class ZipFile;

	class DocxCompilerItemFile;
	class DocxCompilerItemGroup;
	class DocxCompilerItemLayout;
	class DocxCompilerField;
	class DocxCompilerXmlString;
	class DocxCompilerItem
	{
	public:
		static void getItemOptions(
			string&					p_name,
			double&					p_size,
			double&					p_pageSize,
			bool&					p_keepTogether,
			bool&					p_keepWithPrev,
			bool&					p_filler,
			bool&					p_spacer,
			bool&					p_alternate );

		DocxCompilerItem(	
			DocxCompilerItemFile*	p_itemFile,
			DocxCompilerItemGroup*	p_itemGroup,
			string					p_itemName,
			double					p_size,
			double					p_pageSize,
			bool					p_keepTogether,
			bool					p_keepWithPrev,
			bool					p_filler,
			bool					p_spacer,
			bool					p_alternate );
			 
		virtual ~DocxCompilerItem();

		void loadItemLayout();
		void loadXmlStrings();

		void serialize( ZipFile* p_zipFile );

		void removeChildItemGroup	( DocxCompilerItemGroup* p_itemGroup );
		void insertChildItemGroup	( DocxCompilerItemGroup* p_itemGroup );
		void insertField			( DocxCompilerField* p_field );

		void setPath();
		void setTopLevel			( bool p_topLevel );
		void setFillItem			( DocxCompilerItem* p_fillItem );

		void setSize				( double p_size );
		void setPageSize			( double p_pageSize );
		void setPageBottom			( bool p_pageBottom );
		void setKeepTogether		( bool p_keepTogether );
		void setKeepWithPrev		( bool p_keepWithPrev );
		void setFiller				( bool p_filler );
		void setSpacer				( bool p_spacer );
		void setAlternate			( bool p_alternate );

		DocxCompilerItemFile*						getItemFile() const;
		DocxCompilerItemGroup*						getItemGroup() const;
		string										getName() const;

		const list<DocxCompilerItem*>*				getPath() const;
		const set<DocxCompilerItemGroup*>*			getChildItemGroups() const;
		const multimap<string, DocxCompilerField*>*	getFieldsByName() const;

		xercesc::DOMDocument*						getDoc() const;
		xercesc::DOMElement*						getBodyNode() const;
		const list<DocxCompilerXmlString*>*			getXmlStrings() const;

		bool										isTopLevel() const;
		DocxCompilerItem*							getFillItemParent() const;
		DocxCompilerItemGroup*						getFillItemGroup() const;

		DocxCompilerItemLayout*						getItemLayout() const;
		double										getSize() const;
		double										getPageSize() const;
		bool										isPageBottom() const;
		bool										isKeepTogether() const;
		bool										isKeepWithPrev() const;
		bool										isFiller() const;
		bool										isSpacer() const;
		bool										isAlternate() const;

	protected:

	private:
		DocxCompilerItem( const DocxCompilerItem& p_other );
		DocxCompilerItem& operator = ( const DocxCompilerItem& p_other );

		DocxCompilerItemFile*					m_itemFile;
		DocxCompilerItemGroup*					m_itemGroup;
		string									m_name;

		list<DocxCompilerItem*>					m_path;
		set<DocxCompilerItemGroup*>				m_childItemGroups;
		multimap<string, DocxCompilerField*>	m_fieldsByName;

		xercesc::DOMDocument*					m_doc;
		xercesc::DOMElement*					m_bodyNode;
		list<DocxCompilerXmlString*>			m_xmlStrings;

		bool									m_topLevel;
		DocxCompilerItem*						m_fillItemParent;
		DocxCompilerItemGroup*					m_fillItemGroup;

		DocxCompilerItemLayout*					m_itemLayout;
		double									m_size;
		double									m_pageSize;
		bool									m_pageBottom;
		bool									m_keepTogether;
		bool									m_keepWithPrev;
		bool									m_filler;
		bool									m_spacer;
		bool									m_alternate;

	};
};

#endif
