
#ifndef __DOCXFACTORY_GRADE_GROUP_H__
#define __DOCXFACTORY_GRADE_GROUP_H__

#include <map>
#include <string>



namespace DocxFactory
{
	using namespace std;

	class GradeGroup
	{
	public:
		GradeGroup();
		virtual ~GradeGroup();

		void	insertImportGrade	( const string& p_ext, double p_grade );
		void	insertExportGrade	( const string& p_ext, double p_grade );

		double	getImportGrade		( const string& p_ext ) const;
		double	getExportGrade		( const string& p_ext ) const;

	protected:

	private:
		GradeGroup( const GradeGroup& p_other );
		GradeGroup& operator = ( const GradeGroup& p_other );

		map<string, double>	m_importGradeGroup;
		map<string, double>	m_exportGradeGroup;

	};
};

#endif
