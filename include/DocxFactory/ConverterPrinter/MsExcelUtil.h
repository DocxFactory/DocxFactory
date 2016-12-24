
#ifndef __DOCXFACTORY_MS_EXCEL_UTIL_H__
#define __DOCXFACTORY_MS_EXCEL_UTIL_H__

#include "DocxFactory/ConverterPrinter/MsOfficeUtil.h"



namespace DocxFactory
{
	using namespace std;

	class MsExcelUtil : public MsOfficeUtil
	{
	public:
		MsExcelUtil();
		virtual ~MsExcelUtil();

		virtual void saveDocAs	( IDispatch* p_docDispatch, const string& p_fileName );
		virtual void printDoc	( IDispatch* p_appDispatch, IDispatch* p_docDispatch, const string& p_printerName, int p_copyCnt );

	protected:

	private:
		MsExcelUtil( const MsExcelUtil& p_other );
		MsExcelUtil& operator = ( const MsExcelUtil& p_other );

		void initializeGrades();
		void initializeSaveProperties();

	};
};

#endif
