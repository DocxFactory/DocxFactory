
#ifndef __DOCXFACTORY_OPC_RELATIONSHIP_H__
#define __DOCXFACTORY_OPC_RELATIONSHIP_H__

#include <string>



namespace DocxFactory
{
	using namespace std;

	class OpcPackage;
	class OpcPart;
	class OpcRelationships;
	class OpcRelationship
	{
	public:
		enum TargetMode
		{
			INTERNAL_MODE,
			EXTERNAL_MODE
		};

		// create an internal relationship
		OpcRelationship(
			OpcRelationships*	p_relationships,
			const string&		p_id,
			const string&		p_type,
			OpcPart*			p_part );

		// create an external relationship
		OpcRelationship(
			OpcRelationships*	p_relationships,
			const string&		p_id,
			const string&		p_type,
			const string&		p_externalPath );

		virtual ~OpcRelationship();

		OpcPackage*			getPackage() const;
		OpcRelationships*	getRelationships() const;
		string				getId() const;
		string				getType() const;
		OpcPart*			getTargetPart() const;
		string				getExternalPath() const;
		TargetMode			getTargetMode() const;

	protected:

	private:
		OpcRelationship( const OpcRelationship& p_other );
		OpcRelationship operator = ( const OpcRelationship& p_other );

		OpcPackage*			m_package;
		OpcRelationships*	m_relationships;
		string				m_id;
		string				m_type;
		OpcPart*			m_targetPart;
		string				m_externalPath;
		TargetMode			m_targetMode;
	};
};

#endif
