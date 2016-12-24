
#ifndef __DOCXFACTORY_STR_ENTRIES_H__
#define __DOCXFACTORY_STR_ENTRIES_H__

#include <vector>
#include <string>



namespace DocxFactory
{
	using namespace std;

	class StrEntries
	{
	public:

		// since size_t is unsigned the following line will
		// assign npos with the maximum value of size_t
		static const size_t npos = -1;

		StrEntries( const string& p_str, const string& p_delim = "," );
		virtual ~StrEntries();

		void setEntry	( size_t p_index, const string& p_entry );
		void addEntry	( const string& p_entry );
		void deleteEntry( size_t p_index );

		size_t lookup( const string& p_entry )	const;
		size_t getNumEntries()					const;
		string getEntry( size_t p_index )		const;
		string getStr()							const;

	protected:

	private:
		StrEntries( const StrEntries& p_other );
		StrEntries operator = ( const StrEntries& p_other );

		string			m_delim;
		vector<string>	m_entryVec;

	};
};

#endif
