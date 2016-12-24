%module WordProcessingCompiler
%{
#include "dfwc.h"
#include <exception>
%}

%include "java/std_string.i";

%rename("%(strip:[dfwc_])s") "";

%ignore dfwc_getErrorWhat;
%ignore dfwc_getErrorWhere;
%ignore dfwc_getErrorFlag;

/*** replaced by insert_safe_exceptions.js because of the swig java exceptions memory leak ***
%exception {
    $function
    if (dfwc_getErrorFlag()) {
		SWIG_JavaThrowException(jenv, SWIG_JavaRuntimeException, dfwc_getErrorWhat());
		return $null;
	}
}
***/

%include "dfwc.h";

%pragma(java) jniclasscode=%{
	static {
		try {
			System.loadLibrary("DocxFactoryJavaExt");
		} catch (UnsatisfiedLinkError e) {
			System.err.println("Native code library failed to load. \n" + e);
			System.exit(1);
		}
	}
%}
