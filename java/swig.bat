
cd /D "%~dp0"

rmdir .\com /S /Q
mkdir .\com
mkdir .\com\docxfactory

del *.jar /F /Q
del *.h /F /Q
del *.cxx /F /Q

copy ..\include\DocxFactory\WordProcessingCompiler\dfwc.h . /Y
copy ..\include\DocxFactory\WordProcessingMerger\dfw.h . /Y

"C:\Program Files (x86)\swig\swig.exe" -c++ -java -package com.docxfactory -outdir com\docxfactory WordProcessingCompiler.i
"C:\Program Files (x86)\swig\swig.exe" -c++ -java -package com.docxfactory -outdir com\docxfactory WordProcessingMerger.i

cscript.exe ".\insert_safe_exceptions.js" ".\WordProcessingCompiler_wrap.cxx"
cscript.exe ".\insert_safe_exceptions.js" ".\WordProcessingMerger_wrap.cxx"

set path=C:\jdk1.5.0_22\bin;%path%
set classpath=.\com\docxfactory;%classpath%

javac.exe ".\com\docxfactory\*"

jar cf docxfactory.jar .\com\docxfactory

rmdir .\com /S /Q

del *.h /F /Q
