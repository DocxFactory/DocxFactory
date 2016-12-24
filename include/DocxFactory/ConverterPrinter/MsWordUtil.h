
#ifndef __DOCXFACTORY_MS_WORD_UTIL_H__
#define __DOCXFACTORY_MS_WORD_UTIL_H__

#include "DocxFactory/ConverterPrinter/MsOfficeUtil.h"



namespace DocxFactory
{
	using namespace std;

	class MsWordUtil : public MsOfficeUtil
	{
	public:
		MsWordUtil();
		virtual ~MsWordUtil();

		virtual IDispatch*	openDoc		( IDispatch* p_docDispatch, const string& p_fileName );
		virtual void		saveDocAs	( IDispatch* p_docDispatch, const string& p_fileName );
		virtual void		printDoc	( IDispatch* p_appDispatch, IDispatch* p_docDispatch, const string& p_printerName, int p_copyCnt );

	protected:

	private:
		MsWordUtil( const MsWordUtil& p_other );
		MsWordUtil& operator = ( const MsWordUtil& p_other );

		void updateToc( IDispatch* p_docDispatch );

		void initializeGrades();
		void initializeSaveProperties();

	};
};

#endif
