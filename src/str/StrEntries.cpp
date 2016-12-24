
#include "DocxFactory/str/StrEntries.h"

#include "DocxFactory/util/DocxFactoryDefs.h"

using namespace DocxFactory;
using namespace std;



StrEntries::StrEntries( const string& p_str, const string& p_delim )
	: m_delim( p_delim )
{
	string	l_entry;
	size_t	l_start = 0;
	size_t	l_pos;

	m_entryVec.clear();

	// an empty string parameter will construct an empty object
	// not an object with one blank entry
	if ( !p_str.empty() )
	{
		while ( l_start < p_str.length() )
		{
			l_pos = p_str.find( m_delim, l_start );

			if ( l_pos == string::npos )
				 l_pos = p_str.length();

			l_entry = p_str.substr( l_start, l_pos - l_start );

			m_entryVec.push_back( l_entry );

			if ( l_pos == p_str.length() - 1 )
				m_entryVec.push_back( "" );

			l_start = l_pos + 1;
		}
	}
} // c'tor

StrEntries::~StrEntries()
{

} // d'tor



void StrEntries::setEntry( size_t p_index, const string& p_entry )
{
	m_entryVec.at( p_index ) = p_entry;
	// throws out_of_range on error
} // setEntry

void StrEntries::addEntry( const string& p_entry )
{
	m_entryVec.push_back( p_entry );
}

void StrEntries::deleteEntry( size_t p_index )
{
	/*** i think this check is not needed for this type of low-level operations. the index needs to be checked before calling the function. ***

	m_entryVec.at( p_index );
	// the reason at() is called is to generate
	// an exception in case of illegal index
	***/

	m_entryVec.erase( m_entryVec.begin() + p_index );
} // deleteEntry



size_t StrEntries::lookup( const string& p_entry ) const
{
	vector<string>::const_iterator i;
	size_t l_index = 0;

	FOR_EACH( i, &m_entryVec )
	{
		if ( *i == p_entry )
			return l_index;

		++l_index;
	}

	// entry not found
	return npos;
} // lookup

size_t StrEntries::getNumEntries() const
{
	return m_entryVec.size();
} // getNumEntries

string StrEntries::getEntry( size_t p_index ) const
{
	return m_entryVec.at( p_index );
	// throws out_of_range on error
} // getEntry

string StrEntries::getStr() const
{
	string l_str = "";
	vector<string>::const_iterator i;

	FOR_EACH( i, &m_entryVec )
	{
		l_str += ( i != m_entryVec.begin() ? m_delim : "" ) + ( *i );
	}

	return l_str;
} // makeStr
