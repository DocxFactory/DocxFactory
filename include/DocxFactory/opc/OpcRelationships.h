
#ifndef __DOCXFACTORY_OPC_RELATIONSHIPS_H__
#define __DOCXFACTORY_OPC_RELATIONSHIPS_H__

#include "DocxFactory/opc/OpcRelationship.h"

#include <map>
#include <string>



namespace DocxFactory
{
	using namespace std;

	class OpcPackage;
	class OpcPart;
	class OpcRelationships
	{
	public:
		OpcRelationships( OpcPackage* p_package );
		OpcRelationships( OpcPart* p_part );
		virtual ~OpcRelationships();

		void load();
		void save();

		// insert an internal relationship
		OpcRelationship* insertRelationship(
			const string&	p_type,
			OpcPart*		p_part );

		// insert an external relationship
		OpcRelationship* insertRelationship(
			const string&	p_type,
			const string&	p_externalPath );

		// insert raw relationship. can be used to recreate a relationships file with the same id's
		OpcRelationship* insertRelationship(
			const string&				p_id,
			const string&				p_type,
			OpcPart*					p_part,
			const string&				p_externalPath,
			OpcRelationship::TargetMode	p_targetMode );

		void deleteRelationship( OpcRelationship* p_relationship );

		OpcPackage*								getPackage() const;
		OpcPart*								getPart() const;
		const map<string, OpcRelationship*>*	getRelationshipsById() const;

	protected:

	private:
		OpcRelationships( const OpcRelationships& p_other );
		OpcRelationships operator = ( const OpcRelationships& p_other );

		OpcPackage*						m_package;
		OpcPart*						m_part;
		string							m_fullPath;
		string							m_relativeDir; // the part dir is used to get the relative dir to the part
		map<string, OpcRelationship*>	m_relationshipsById;
		unsigned int					m_idSeq;
		bool							m_changed;

	};
};

#endif
