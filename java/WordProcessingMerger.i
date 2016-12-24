%module WordProcessingMerger
%{
#include "dfw.h"
#include <exception>
%}

%include "java/std_string.i";

%rename("%(strip:[dfw_])s") "";

%rename(setClipboardValue)	dfw_setClipboardValueByStr;
%rename(setClipboardValue)	dfw_setClipboardValueByDouble;
%rename(setChartValue)		dfw_setChartValueByStrStr;
%rename(setChartValue)		dfw_setChartValueByStrDouble;
%rename(setChartValue)		dfw_setChartValueByDoubleDouble;
%rename(setWeekDayNames)	dfw_setWeekDayNames0;
%rename(setWeekDayNames)	dfw_setWeekDayNames7;
%rename(setWeekDayNames)	dfw_setWeekDayNames14;
%rename(setMonthNames)		dfw_setMonthNames0;
%rename(setMonthNames)		dfw_setMonthNames12;
%rename(setMonthNames)		dfw_setMonthNames24;

%ignore dfw_mergeW;
%ignore dfw_setClipboardValueByStrW;
%ignore dfw_setChartValueByStrStrW;
%ignore dfw_setChartValueByStrDoubleW;
%ignore dfw_setWeekDayNames7W;
%ignore dfw_setWeekDayNames14W;
%ignore dfw_setMonthNames12W;
%ignore dfw_setMonthNames24W;
%ignore dfw_getWeekDayFullNamesW;
%ignore dfw_getWeekDayShortNamesW;
%ignore dfw_getMonthFullNamesW;
%ignore dfw_getMonthShortNamesW;
%ignore dfw_getErrorWhat;
%ignore dfw_getErrorWhere;
%ignore dfw_getErrorFlag;

/*** replaced by insert_safe_exceptions.js because of the swig java exceptions memory leak ***
%exception {
    $action
    if (dfw_getErrorFlag()) {
		SWIG_JavaThrowException(jenv, SWIG_JavaRuntimeException, dfw_getErrorWhat());
		return $null;
	}
}
***/

%include "dfw.h";

%pragma(java) moduleimports=%{
import java.util.Date;
%}

%pragma(java) modulecode=%{
  public static void setClipboardValue(String p_itemName, String p_fieldName, Date p_value) {
    WordProcessingMergerJNI.setClipboardValue__SWIG_1(p_itemName, p_fieldName, (double) p_value.getTime() / 1000);
  }

  public static void setChartValue(String p_itemName, String p_fieldName, String p_series, Date p_category, double p_value) {
    WordProcessingMergerJNI.setChartValue__SWIG_1(p_itemName, p_fieldName, p_series, (double) p_category.getTime() / 1000, p_value);
  }
%}

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
