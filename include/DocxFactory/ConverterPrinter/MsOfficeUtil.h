
#ifndef __DOCXFACTORY_MS_OFFICE_UTIL_H__
#define __DOCXFACTORY_MS_OFFICE_UTIL_H__

#include "DocxFactory/ConverterPrinter/ConverterPrinterUtil.h"
#include "DocxFactory/ConverterPrinter/OleException.h"

#include <map>
#include <Windows.h>

#define THROW_IF_FAILED( result, exception ) \
	if ( FAILED( result ) ) \
		throw exception



namespace DocxFactory
{
	using namespace std;

	struct SaveProperties
	{
		string	m_methodName;
		int		m_format;
	};

	class GradeGroup;
	class MsOfficeUtil : public ConverterPrinterUtil
	{
	public:
		MsOfficeUtil( const string& p_appName, const string& p_preOpenMethodName );
		virtual ~MsOfficeUtil();

		virtual void	saveAs	( const string& p_importFile,	const string& p_exportFile );
		virtual void	print	( const string& p_fileName,		const string& p_printerName, int p_copyCnt );

		virtual double	getConversionGrade	( const string& p_importExt, const string& p_exportExt );
		virtual double	getPrintingGrade	( const string& p_ext );

	protected:
		virtual IDispatch*		connect();
		virtual IDispatch*		openDoc		( IDispatch* p_docDispatch, const string& p_fileName );
		virtual void			closeDoc	( IDispatch* p_docDispatch );
		virtual void			saveDocAs	( IDispatch* p_docDispatch, const string& p_fileName ) = 0;
		virtual void			printDoc	( IDispatch* p_appDispatch, IDispatch* p_docDispatch, const string& p_printerName, int p_copyCnt ) = 0;
		virtual void			setVisible	( IDispatch* p_appDispatch, bool p_visible );

		void					insertImportGrade		( const string& p_ext, double p_grade );
		void					insertExportGrade		( const string& p_ext, double p_grade );
		void					insertSaveExt			( const string& p_ext, const string& p_methodName, int p_format );
		const SaveProperties*	getSavePropertiesByExt	( const string& p_ext ) const;

		double					getAppVersion() const;
		double					getAppBuild() const;
		bool					isCompatibilityPackInstalled() const;
		bool					isPdfExportInstalled() const;

	private:
		MsOfficeUtil( const MsOfficeUtil& p_other );
		MsOfficeUtil& operator = ( const MsOfficeUtil& p_other );

		void initializeAppId		( const string& p_appName );
		void initializeAppVersion	( IDispatch* p_appDispatch );

		GradeGroup*						m_gradeGroup;
		map<string, SaveProperties*>	m_savePropertiesByExt;
		OLECHAR*						m_preOpenMethodName;
		CLSID							m_appId;
		double							m_appBuild;
		double							m_appVersion;
		bool							m_pdfExportInstalled;
		bool							m_compatibilityPackInstalled;

	};
};

#endif
