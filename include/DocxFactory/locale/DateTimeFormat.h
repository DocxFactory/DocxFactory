
#ifndef __DOCXFACTORY_DATE_TIME_FORMAT_H__
#define __DOCXFACTORY_DATE_TIME_FORMAT_H__

#include <string>



namespace DocxFactory
{
	using namespace std;

	struct DateTimeFormat
	{
		string			m_str;
		char			m_entry;
		unsigned short	m_entryLen;
	};
};

#endif
