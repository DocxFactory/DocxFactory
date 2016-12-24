
#ifndef __DOCXFACTORY_TCHAR_H__
#define __DOCXFACTORY_TCHAR_H__

#if defined( _WIN32 ) || defined( _WIN64 )

	#include <tchar.h>

#else

	#define _tmain		main
	typedef char		TCHAR;

	#define _T(x)		x
	#define _TEXT(x)	x

#endif

#ifdef  _UNICODE
	typedef std::wstring	tstring;
#else
	typedef std::string		tstring;
#endif

#endif