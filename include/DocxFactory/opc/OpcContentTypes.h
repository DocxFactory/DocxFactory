
#ifndef __DOCXFACTORY_OPC_CONTENT_TYPES_H__
#define __DOCXFACTORY_OPC_CONTENT_TYPES_H__

#include <map>
#include <string>



namespace DocxFactory
{
	using namespace std;

	class OpcPackage;
	class OpcPart;
	class OpcContentTypes
	{
	public:
		OpcContentTypes( OpcPackage* p_package );
		virtual ~OpcContentTypes();

		void load();
		void save();

		void insertDefault	( const string& p_ext,	const string& p_contentType );
		void insertOverride	( OpcPart* p_part,		const string& p_contentType );

		void deleteDefault	( const string& p_ext );
		void deleteOverride	( OpcPart* p_part );

		OpcPackage*						getPackage() const;
		const map<string, string>*		getDefaultsByExt() const;
		const map<OpcPart*, string>*	getOverridesByPart() const;
		
	protected:

	private:
		OpcContentTypes( const OpcContentTypes& p_other );
		OpcContentTypes operator = ( const OpcContentTypes& p_other );

		OpcPackage*				m_package;
		const string			m_fullPath;
		map<string, string>		m_defaultsByExt;
		map<OpcPart*, string>	m_overridesByPart;
		bool					m_changed;

	};
};

#endif