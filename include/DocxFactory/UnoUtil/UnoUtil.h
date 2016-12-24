
#ifndef __DOCXFACTORY_UNO_UTIL_H__
#define __DOCXFACTORY_UNO_UTIL_H__

#include <string>

#ifdef __unix__
#define __declspec( dllexport )
#endif



namespace DocxFactory
{
	using namespace std;

	class UnoUtil
	{
	public:
		virtual ~UnoUtil() {}

		virtual void saveAs	( const string& p_importFile, const string& p_exportFile ) = 0;
		virtual void print	( const string& p_fileName, const string& p_printerName, int p_copyCnt ) = 0;

	protected:
		UnoUtil() {}
		UnoUtil( const UnoUtil& p_other );
		UnoUtil& operator = ( const UnoUtil& p_other );

	private:

	};
};



using namespace DocxFactory;
using namespace std;

extern "C"
{
	__declspec( dllexport ) UnoUtil* createUnoUtil();
	__declspec( dllexport ) void destroyUnoUtil( UnoUtil* p_util );
}

#endif
