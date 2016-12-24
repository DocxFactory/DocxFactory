
#ifndef __DOCXFACTORY_BARCODE_FORMAT_H__
#define __DOCXFACTORY_BARCODE_FORMAT_H__

#include "DocxFactory/util/DocxFactoryDefs.h"

#include <string>



namespace DocxFactory
{
	using namespace std;

	struct BarcodeFormat
	{
		int		m_symbology;
		string	m_symbologyName;
		int		m_option1;
		int		m_option2;
		int		m_option3;
		int		m_inputMode;

		float	m_scale;
		int		m_height;
		int 	m_showHrt;
		int		m_angle;
		int		m_fontSize;
		string	m_font;
		string	m_fgcolor;
		string	m_bgcolor;
		int		m_whiteSpaceWidth;
		int		m_borderWidth;
		int		m_outputOptions;
	};
};

#endif
