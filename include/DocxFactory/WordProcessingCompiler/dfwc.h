
#ifndef __DOCXFACTORY_H__
#define __DOCXFACTORY_H__



#if defined( __unix__ ) || defined( SWIG )
	#define DOCXFACTORY_DLL
#else
	#ifdef  DOCXFACTORY_DLL_EXPORT
	#define DOCXFACTORY_DLL __declspec(dllexport)
	#else
	#define DOCXFACTORY_DLL __declspec(dllimport)
	#endif
#endif



#ifdef __cplusplus
extern "C" {
#endif

DOCXFACTORY_DLL void			dfwc_compile( const char* p_srcFileName, const char* p_dstFileName );

DOCXFACTORY_DLL void			dfwc_setTempDir( const char* p_dir );
DOCXFACTORY_DLL const char*		dfwc_getTempDir();
DOCXFACTORY_DLL const char*		dfwc_getWorkDir();

DOCXFACTORY_DLL const char*		dfwc_getErrorWhat();
DOCXFACTORY_DLL const char*		dfwc_getErrorWhere();
DOCXFACTORY_DLL unsigned char	dfwc_getErrorFlag();

#ifdef __cplusplus
}
#endif

#ifdef DOCXFACTORY_DLL
#undef DOCXFACTORY_DLL
#endif
#endif
