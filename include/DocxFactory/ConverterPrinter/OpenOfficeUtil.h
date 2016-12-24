
#ifndef __DOCXFACTORY_OPEN_OFFICE_UTIL_H__
#define __DOCXFACTORY_OPEN_OFFICE_UTIL_H__

#include "DocxFactory/ConverterPrinter/ConverterPrinterUtil.h"

#include <list>
#include <string>



namespace DocxFactory
{
	using namespace std;

	class OdfConverterUtil;
	class UnoUtil;
	class GradeGroup;
	class OpenOfficeUtil : public ConverterPrinterUtil
	{
	public:
		OpenOfficeUtil( OdfConverterUtil* p_odfConverter = NULL );
		virtual ~OpenOfficeUtil();

		virtual void	saveAs	( const string& p_importFile, const string& p_exportFile );
		virtual void	print	( const string& p_fileName, const string& p_printerName, int p_copyCnt );

		virtual double	getConversionGrade	( const string& p_importExt, const string& p_exportExt );
		virtual double	getPrintingGrade	( const string& p_ext );

	protected:

	private:
		OpenOfficeUtil( const OpenOfficeUtil& p_other );
		OpenOfficeUtil& operator = ( const OpenOfficeUtil& p_other );

		void initializeGrades();
		void initializeUnoUtil();
		void ( *m_destroyUnoUtilFunc )( UnoUtil* );

		string	openXmlToOdf( const string& p_openXmlExt );
		bool	isOpenXml	( const string& p_ext );

		OdfConverterUtil*	m_odfConverter;
		UnoUtil*			m_unoUtil;
		list<GradeGroup*>	m_gradeGroups;

	};
};

#endif
