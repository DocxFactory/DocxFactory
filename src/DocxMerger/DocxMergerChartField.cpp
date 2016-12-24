
#include "DocxFactory/DocxMerger/DocxMergerFile.h"
#include "DocxFactory/DocxMerger/DocxMergerItemFile.h"
#include "DocxFactory/DocxMerger/DocxMergerItem.h"
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerChartField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteFieldGroup.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteField.h"
#include "DocxFactory/DocxMerger/DocxMergerPasteChartField.h"

#include "DocxFactory/opc/OpcPart.h"
#include "DocxFactory/opc/OpcRelationships.h"
#include "DocxFactory/opc/OpcRelationship.h"
#include "DocxFactory/opc/OpcContentTypes.h"
#include "DocxFactory/opc/OpcFunc.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/zip/UnzipFile.h"
#include "DocxFactory/str/StrFunc.h"

using namespace DocxFactory;
using namespace std;



DocxMergerChartField::DocxMergerChartField( ChartType p_chartType ) : DocxMergerField( DocxMergerField::TYPE_CHART )
{
	m_chartType = p_chartType;
} // c'tor

DocxMergerChartField::~DocxMergerChartField()
{

} // d'tor



void DocxMergerChartField::save( DocxMergerPasteFieldGroup* p_pasteFieldGroup )
{
	if ( !p_pasteFieldGroup )
		return;

	map<DocxMergerField*, DocxMergerPasteField*>::const_iterator	l_pasteFieldIterator;
	const map<DocxMergerField*, DocxMergerPasteField*>*				l_pasteFields = p_pasteFieldGroup ->getPasteFieldsByField();

	l_pasteFieldIterator = l_pasteFields ->find( this );
	if ( l_pasteFieldIterator != l_pasteFields ->end() )

		saveChart(
			m_itemFile ->getZipFile(),
			( ( DocxMergerPasteChartField* ) l_pasteFieldIterator ->second ) ->getValue() );
} // save



void DocxMergerChartField::saveChart( ZipFile* p_zipFile, const string* p_value )
{
	list<pair<string, char>>::const_iterator					l_stringIterator;
	list<boost::tuple<string, string, string, bool>>::iterator	l_relationshipIterator;

	DocxMergerFile*		l_file = m_itemFile ->getFile();
	OpcPart*			l_part;
	OpcRelationships*	l_relationships;
	OpcRelationship*	l_relationship;

	string				l_path;
	string				l_fileRId;

	l_path			= l_file ->getChartDir() + "chart-dfw-" + StrFunc::intToStr( l_file ->nextChartSeq() ) + ".xml";
	l_part			= l_file ->insertPart( l_path );
	l_relationships	= l_part ->getRelationships();
	l_file ->getContentTypes() ->insertOverride( l_part, "application/vnd.openxmlformats-officedocument.drawingml.chart+xml" );

	l_part ->setBuf( p_value ->c_str(), p_value ->length() );
	l_part ->setChangeStatus( OpcPart::PART_CHANGED_IN_BUFFER );

	l_relationship	= m_itemFile ->getPart() ->getRelationships() ->insertRelationship( "http://schemas.openxmlformats.org/officeDocument/2006/relationships/chart", l_part );
	l_fileRId		= l_relationship ->getId();

	FOR_EACH( l_stringIterator, &m_drawingStrings )
	{
		( *p_zipFile ) << l_stringIterator ->first;

		switch ( l_stringIterator ->second )
		{
		case 'r': ( *p_zipFile ) << l_fileRId; break;
		case 'i': ( *p_zipFile ) << StrFunc::intToStr( m_file ->nextIdSeq() ); break;
		}
	}



	FOR_EACH( l_relationshipIterator, &m_chartRelationships )
	{
		if ( get<3>( *l_relationshipIterator ) )
			l_relationships ->insertRelationship(
				get<0>( *l_relationshipIterator ),
				get<1>( *l_relationshipIterator ),
				l_file ->getPartsByFullPath() ->at( get<2>( *l_relationshipIterator ) ),
				"",
				OpcRelationship::INTERNAL_MODE );

		else
			l_relationships ->insertRelationship(
				get<0>( *l_relationshipIterator ),
				get<1>( *l_relationshipIterator ),
				NULL,
				get<2>( *l_relationshipIterator ),
				OpcRelationship::EXTERNAL_MODE );
	}
} // saveChart



void DocxMergerChartField::setChartValue( const string& p_series, double p_category, double p_value )
{

} // setChartValue

void DocxMergerChartField::setChartValue( const string& p_series, const string& p_category, double p_value )
{

} // setChartValue

void DocxMergerChartField::setChartValue( double p_series, double p_category, double p_value )
{

} // setChartValue



void DocxMergerChartField::deserialize( UnzipFile* p_unzipFile )
{
	string	l_str;
	string	l_str2;
	string	l_str3;
	char	l_ch;
	bool	l_ok;

	uint32	l_size;
	uint32	i;

	DocxMergerField::deserialize( p_unzipFile );

	m_chartMlPrefix = p_unzipFile ->readStr();
	m_relMlPrefix = p_unzipFile ->readStr();

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_str	=			p_unzipFile ->readStr();
		l_ch	= ( char )	p_unzipFile ->readNum<uint8>();

		m_drawingStrings.push_back( make_pair( l_str, l_ch ) );
	}

	l_size = p_unzipFile ->readNum<uint32>();
	for ( i = 0; i < l_size; ++i )
	{
		l_str	= p_unzipFile ->readStr();
		l_str2	= p_unzipFile ->readStr();
		l_str3	= p_unzipFile ->readStr();
		l_ok	= p_unzipFile ->readNum<uint8>() == 1;

		m_chartRelationships.push_back( boost::make_tuple( l_str, l_str2, l_str3, l_ok ) );
	}
} // deserialize



DocxMergerChartField::ChartType DocxMergerChartField::getChartType() const
{
	return m_chartType;
} // getChartType

string DocxMergerChartField::getChartMlPrefix() const
{
	return m_chartMlPrefix;
} // getChartMlPrefix

string DocxMergerChartField::getRelMlPrefix() const
{
	return m_relMlPrefix;
} // getRelMlPrefix

const list<pair<string, char>>* DocxMergerChartField::getDrawingStrings() const
{
	return &m_drawingStrings;
} // getDrawingStrings

const list<boost::tuple<string, string, string, bool>>*	DocxMergerChartField::getChartRelationships() const
{
	return &m_chartRelationships;
} // getChartRelationships
