%module WordProcessingCompiler
%{
#include "dfwc.h"
#include <exception>
%}

%include "csharp/std_string.i";

%rename("%(regex:/(\\w+)_(.*)/\\u\\2/)s") "";

%ignore dfwc_getErrorWhat;
%ignore dfwc_getErrorWhere;
%ignore dfwc_getErrorFlag;
%ignore dfwc_setTempDir;
%ignore dfwc_getTempDir;
%ignore dfwc_getWorkDir;

%exception {
	$function
	if (dfwc_getErrorFlag()) {
		SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentException, dfwc_getErrorWhat(), "");
		return $null;
	}
}

%include "dfwc.h";
