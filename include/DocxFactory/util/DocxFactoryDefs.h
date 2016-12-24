
#ifndef __DOCXFACTORY_DEFS_H__
#define __DOCXFACTORY_DEFS_H__

#include <string>

#define COPYRIGHT				"www.docxfactory.com" // Copyright (C) 2013 DocxFactory Ltd."
#define CONTACTUS				"Contact us at www.docxfactory.com for support."
#define VERSION					"1.2.18"
#define DOCX_TEMPLATE_VERSION	"1.2.17" // the last time the docx template version was changed and is compatible



#if defined( __osf__ )
// Tru64 lacks stdint.h, but has inttypes.h which defines a superset of
// what stdint.h would define.
#include <inttypes.h>
#elif !defined( _MSC_VER )
#include <stdint.h>
#endif



namespace DocxFactory
{
	using namespace std;

	#ifdef _MSC_VER

	typedef __int8  int8;
	typedef __int16 int16;
	typedef __int32 int32;
	typedef __int64 int64;

	typedef unsigned __int8  uint8;
	typedef unsigned __int16 uint16;
	typedef unsigned __int32 uint32;
	typedef unsigned __int64 uint64;

	#else

	typedef int8_t  int8;
	typedef int16_t int16;
	typedef int32_t int32;
	typedef int64_t int64;

	typedef uint8_t  uint8;
	typedef uint16_t uint16;
	typedef uint32_t uint32;
	typedef uint64_t uint64;

	#endif

	typedef unsigned char byte;
};



#ifndef NULL
#define NULL 0
#endif

#define IS_DIGIT( val ) ( val >= '0' && val <= '9' )
#define IS_HEXDIGIT( val ) ( val >= '0' && val <= '9' || val >= 'A' && val <= 'F' || val >= 'a' && val <= 'f' )
#define IS_LETTER( val ) ( val >= 'A' && val <= 'Z' || val >= 'a' && val <= 'z' )
#define IS_UPPER( val ) ( val >= 'A' && val <= 'Z' )
#define IS_LOWER( val ) ( val >= 'a' && val <= 'z' )

#define FOR_EACH( iterator, containerPtr ) \
	for ( iterator = ( containerPtr ) ->begin(); iterator != ( containerPtr ) ->end(); ++iterator )
#define REVERSE_FOR_EACH( riterator, containerPtr ) \
	for ( riterator = ( containerPtr ) ->rbegin(); riterator != ( containerPtr ) ->rend(); ++riterator )
#endif
