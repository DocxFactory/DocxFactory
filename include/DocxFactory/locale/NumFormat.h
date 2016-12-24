
#ifndef __DOCXFACTORY_NUM_FORMAT_H__
#define __DOCXFACTORY_NUM_FORMAT_H__

#include <string>



namespace DocxFactory
{
	using namespace std;

	struct NumFormat
	{
		string			m_posColor;
		string			m_posPrefix;
		unsigned short	m_posLeftDigCnt;
		unsigned short	m_posRightDigCnt;
		unsigned short	m_posRightExtCnt;		
		string			m_posSuffix;
		bool			m_posSep;
		bool			m_posPercent;

		string			m_negColor;
		string			m_negPrefix;
		unsigned short	m_negLeftDigCnt;
		unsigned short	m_negRightDigCnt;
		unsigned short	m_negRightExtCnt;
		string			m_negSuffix;
		bool			m_negSep;
		bool			m_negPercent;

		string			m_zeroColor;
		string			m_zeroPrefix;
		unsigned short	m_zeroLeftDigCnt;
		unsigned short	m_zeroRightDigCnt;
		string			m_zeroSuffix;
		bool			m_zeroSep;
	};
};

#endif
