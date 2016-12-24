
#ifndef __DOCXFACTORY_ODF_CONVERTER_UTIL_H__
#define __DOCXFACTORY_ODF_CONVERTER_UTIL_H__

#include "DocxFactory/ConverterPrinter/ConverterPrinterUtil.h"

#include <list>



namespace DocxFactory
{
	using namespace std;

	class GradeGroup;
	class OdfConverterUtil : public ConverterPrinterUtil
	{
	public:
		OdfConverterUtil();
		virtual ~OdfConverterUtil();

		virtual void	saveAs	( const string& p_importFile,	const string& p_exportFile );
		virtual void	print	( const string& p_fileName,		const string& p_printerName, int p_copyCnt );

		virtual double	getConversionGrade	( const string& p_importExt, const string& p_exportExt );
		virtual double	getPrintingGrade	( const string& p_ext );

	protected:

	private:
		OdfConverterUtil( const OdfConverterUtil& p_other );
		OdfConverterUtil& operator = ( const OdfConverterUtil& p_other );

		void initializeGrades();

		string	openXmlToOdf( const string& p_openXmlExt );
		bool	isOpenXml	( const string& p_ext );

		list<GradeGroup*>	m_gradeGroups;
		string				m_exec;

	};
};

#endif
