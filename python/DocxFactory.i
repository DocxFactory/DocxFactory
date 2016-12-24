%module docxfactory
%{
#include "WordProcessingCompiler.h"
#include "WordProcessingMerger.h"
#include <exception>
%}

%include "python/std_string.i"

%rename ("%(undercase)s") "";
%rename (print_doc) print;
%rename (WordProcessingCompiler) WordProcessingCompiler;
%rename (WordProcessingMerger) WordProcessingMerger;
%rename (setClipboardValue) setClipboardValue;
%rename (setChartValue) setChartValue;

%exception {
	try {
		$action
	} catch ( std::exception &_e ) {
		PyErr_SetString( PyExc_Exception, const_cast<char*>( _e.what() ) );
		return NULL;
	}
}

%include "WordProcessingCompiler.h"
%include "WordProcessingMerger.h"

%pythoncode %{
from datetime	import datetime, date
from sys		import version_info

def WordProcessingMerger_set_clipboard_value(self, item_name, field_name, val):
  if val is None: return
  if isinstance(val, datetime) or isinstance(val, date): val = str(val)
  return self.setClipboardValue(item_name, field_name, val)
WordProcessingMerger.set_clipboard_value = WordProcessingMerger_set_clipboard_value

def WordProcessingMerger_set_chart_value(self, item_name, field_name, series, category, val):
  if category is None: return
  if isinstance(category, datetime) or isinstance(category, date): val = str(category)
  return self.setChartValue(item_name, field_name, series, category, val)
WordProcessingMerger.set_chart_value = WordProcessingMerger_set_chart_value
%}
