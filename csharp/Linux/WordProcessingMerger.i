%module WordProcessingMerger
%{
#include "dfw.h"
#include <exception>
%}

%include "csharp/std_string.i";

%rename("%(regex:/(\\w+)_(.*)/\\u\\2/)s") "";

%rename(Merge)					dfw_merge;
%rename(SetClipboardValue)		dfw_setClipboardValueByStr;
%rename(SetClipboardValue)		dfw_setClipboardValueByDouble;
%rename(SetChartValue)			dfw_setChartValueByStrStr;
%rename(SetChartValue)			dfw_setChartValueByStrDouble;
%rename(SetChartValue)			dfw_setChartValueByDoubleDouble;
%rename(SetWeekDayNames)		dfw_setWeekDayNames0;
%rename(SetWeekDayNames)		dfw_setWeekDayNames7;
%rename(SetWeekDayNames)		dfw_setWeekDayNames14;
%rename(SetMonthNames)			dfw_setMonthNames0;
%rename(SetMonthNames)			dfw_setMonthNames12;
%rename(SetMonthNames)			dfw_setMonthNames24;
%rename(GetWeekDayFullNames)	dfw_getWeekDayFullNames;
%rename(GetWeekDayShortNames)	dfw_getWeekDayShortNames;
%rename(GetMonthFullNames)		dfw_getMonthFullNames;
%rename(GetMonthShortNames)		dfw_getMonthShortNames;

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

%exception {
	$function
	if (dfw_getErrorFlag()) {
		SWIG_CSharpSetPendingExceptionArgument(SWIG_CSharpArgumentException, dfw_getErrorWhat(), "");
		return $null;
	}
}

%include "dfw.h";

%pragma(csharp) moduleimports=%{
using System;
%}

%pragma(csharp) modulecode=%{
  static private readonly DateTime _epoch;
  static WordProcessingMerger()
  {
    _epoch = new DateTime(1970, 1, 1, 0, 0, 0, 0);
  }

  private static double ConvertToUnixTime(DateTime date)
  {
    return (date - _epoch).TotalSeconds;
  }

  public static void SetClipboardValue(string p_itemName, string p_fieldName, DateTime p_value) {
    WordProcessingMergerPINVOKE.SetClipboardValue__SWIG_1(p_itemName, p_fieldName, ConvertToUnixTime(p_value));
    if (WordProcessingMergerPINVOKE.SWIGPendingException.Pending) throw WordProcessingMergerPINVOKE.SWIGPendingException.Retrieve();
  }

  public static void SetChartValue(string p_itemName, string p_fieldName, string p_series, DateTime p_category, double p_value) {
    WordProcessingMergerPINVOKE.SetChartValue__SWIG_1(p_itemName, p_fieldName, p_series, ConvertToUnixTime(p_category), p_value);
    if (WordProcessingMergerPINVOKE.SWIGPendingException.Pending) throw WordProcessingMergerPINVOKE.SWIGPendingException.Retrieve();
  }
%}
