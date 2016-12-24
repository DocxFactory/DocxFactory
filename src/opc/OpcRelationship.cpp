
#include "DocxFactory/opc/OpcRelationships.h"
#include "DocxFactory/opc/OpcRelationship.h"

using namespace DocxFactory;
using namespace std;



// create an internal relationship
OpcRelationship::OpcRelationship(
	OpcRelationships*	p_relationships,
	const string&		p_id,
	const string&		p_type,
	OpcPart*			p_part )
{
	m_package		= p_relationships ->getPackage();
	m_relationships	= p_relationships;
	m_id			= p_id;
	m_type			= p_type;
	m_targetPart	= p_part;
	m_externalPath	= "";
	m_targetMode	= INTERNAL_MODE;
} // c'tor

// create an external relationship
OpcRelationship::OpcRelationship(
	OpcRelationships*	p_relationships,
	const string&		p_id,
	const string&		p_type,
	const string&		p_externalPath )
{
	m_package		= p_relationships ->getPackage();
	m_relationships	= p_relationships;
	m_id			= p_id;
	m_type			= p_type;
	m_targetPart	= NULL;
	m_externalPath	= p_externalPath;
	m_targetMode	= EXTERNAL_MODE;
} // c'tor

OpcRelationship::~OpcRelationship()
{

} // d'tor



OpcPackage* OpcRelationship::getPackage() const
{
	return m_package;
} // getPackage

OpcRelationships* OpcRelationship::getRelationships() const
{
	return m_relationships;
} // getRelationships

string OpcRelationship::getId() const
{
	return m_id;
} // getId

string OpcRelationship::getType() const
{
	return m_type;
} // getType

OpcPart* OpcRelationship::getTargetPart() const
{
	return m_targetPart;
} // getTargetPart

string OpcRelationship::getExternalPath() const
{
	return m_externalPath;
} // getExternalPath

OpcRelationship::TargetMode OpcRelationship::getTargetMode() const
{
	return m_targetMode;
} // getTargetMode
