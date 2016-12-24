
#include "DocxFactory/ConverterPrinter/GradeGroup.h"

using namespace DocxFactory;
using namespace std;



GradeGroup::GradeGroup()
{

} // c'tor

GradeGroup::~GradeGroup()
{

} // d'tor



void GradeGroup::insertImportGrade( const string& p_ext, double p_grade )
{
	map<string, double>::iterator l_gradeIterator = m_importGradeGroup.find( p_ext );

	if ( l_gradeIterator == m_importGradeGroup.end() )
		m_importGradeGroup.insert( make_pair( p_ext, p_grade ) );

	else
		l_gradeIterator ->second = p_grade;
} // insertImportGrade

void GradeGroup::insertExportGrade( const string& p_ext, double p_grade )
{
	map<string, double>::iterator l_gradeIterator = m_exportGradeGroup.find( p_ext );

	if ( l_gradeIterator == m_exportGradeGroup.end() )
		m_exportGradeGroup.insert( make_pair( p_ext, p_grade ) );

	else
		l_gradeIterator ->second = p_grade;
} // insertExportGrade



double GradeGroup::getImportGrade( const string& p_ext ) const
{
	map<string, double>::const_iterator l_gradeIterator = m_importGradeGroup.find( p_ext );

	if ( l_gradeIterator != m_importGradeGroup.end() )
		return l_gradeIterator ->second;

	else
		return -1;
} // getImportGrade

double GradeGroup::getExportGrade( const string& p_ext ) const
{
	map<string, double>::const_iterator l_gradeIterator = m_exportGradeGroup.find( p_ext );

	if ( l_gradeIterator != m_exportGradeGroup.end() )
		return l_gradeIterator ->second;

	else
		return -1;
} // getExportGrade
