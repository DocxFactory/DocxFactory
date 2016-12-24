
#ifndef __DOCXFACTORY_PROPERTY_MAP_H__
#define __DOCXFACTORY_PROPERTY_MAP_H__

#include <map>
#include <list>
#include <utility>
#include <string>



namespace DocxFactory
{
	using namespace std;

	typedef pair<string, string>			PropertyValuePair;
	typedef list<PropertyValuePair>			PropertyValueList;
	typedef map<string, PropertyValueList*>	PropsByExt;
	typedef map<string, PropsByExt*>		PropsByFamilyExt;

	class PropertyMap
	{
	public:
		PropertyMap();
		virtual ~PropertyMap();

		void setProperty(
			const string& p_family,
			const string& p_ext,
			const string& p_key,
			const string& p_value );

		const PropertyValueList* getProperties(
			const string& p_family,
			const string& p_ext ) const;

	protected:

	private:
		PropertyMap( const PropertyMap& p_other );
		PropertyMap& operator = ( const PropertyMap& p_other );

		PropsByFamilyExt m_properties;

	};
};

#endif
