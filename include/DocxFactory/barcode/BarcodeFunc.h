
#ifndef __DOCXFACTORY_BARCODE_FUNC_H__
#define __DOCXFACTORY_BARCODE_FUNC_H__

#include <list>
#include <string>



namespace DocxFactory
{
	using namespace std;

	struct	BarcodeFormat;
	class	BarcodeFunc
	{
	public:
		enum BarcodePrinter
		{
			PRINT_VML
		};

		static bool encodeAndPrint( 
			BarcodeFormat*		p_barcodeFormat,
			const string&		p_value,
			BarcodePrinter		p_printer,
			string&				p_errTxt,
			string&				p_retVal );

	protected:

	private:
		BarcodeFunc();
		BarcodeFunc( const BarcodeFunc& p_other );
		BarcodeFunc& operator = ( const BarcodeFunc& p_other );

	};
};

#endif