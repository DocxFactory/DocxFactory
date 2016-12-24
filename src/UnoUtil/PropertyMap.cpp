
#include "DocxFactory/UnoUtil/PropertyMap.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

using namespace DocxFactory;
using namespace std;



PropertyMap::PropertyMap()
{

} // c'tor

PropertyMap::~PropertyMap()
{
	PropsByFamilyExt::iterator	l_extIterator;
	PropsByExt::iterator		l_propsIterator;
	PropsByExt*					l_propsByExt;

	FOR_EACH( l_extIterator, &m_properties )
	{
		l_propsByExt = l_extIterator ->second;

		for ( l_propsIterator = l_propsByExt ->begin(); l_propsIterator != l_propsByExt ->end(); ++l_propsIterator )
		{
			delete l_propsIterator ->second;
		}

		delete l_propsByExt;
	}
} // d'tor



void PropertyMap::setProperty(
	const string& p_family,
	const string& p_ext,
	const string& p_key,
	const string& p_value )
{
	PropsByFamilyExt::iterator	l_extIterator;
	PropsByExt::iterator		l_propsIterator;
	PropsByExt*					l_propsByExt;
	PropertyValueList*			l_propertyValueList;

	l_extIterator = m_properties.find( p_family );
	if ( l_extIterator == m_properties.end() )
	{
		l_propsByExt				= new PropsByExt();
		m_properties[ p_family ]	= l_propsByExt;
	}

	else
		l_propsByExt = l_extIterator ->second;
		
	l_propsIterator = l_propsByExt ->find( p_ext );
	if ( l_propsIterator == l_propsByExt ->end() )
	{
		l_propertyValueList			= new PropertyValueList();
		( *l_propsByExt )[ p_ext ]	= l_propertyValueList;
	}

	else
		l_propertyValueList = l_propsIterator ->second;

	l_propertyValueList ->push_back( PropertyValuePair( p_key, p_value ) );
} // setProperty

const PropertyValueList* PropertyMap::getProperties(
	const string& p_family,
	const string& p_ext ) const
{
	PropsByFamilyExt::const_iterator	l_extIterator;
	PropsByExt::const_iterator			l_propsIterator;
	PropsByExt*							l_propsByExt;
	PropertyValueList*					l_propertyValueList = NULL;

	l_extIterator = m_properties.find( p_family );
	if ( l_extIterator != m_properties.end() )
	{
		l_propsByExt	= l_extIterator ->second;
		l_propsIterator	= l_propsByExt ->find( p_ext );

		if( l_propsIterator != l_propsByExt ->end() )
			l_propertyValueList = l_propsIterator ->second;
	}

	return l_propertyValueList;
} // getProperties
