
cd /D "%~dp0"

del *.h /F /Q
del docxfactory.py /F /Q
del DocxFactory_wrap.cxx /F /Q

copy ..\include\DocxFactory\WordProcessingCompiler\WordProcessingCompiler.h . /Y
copy ..\include\DocxFactory\WordProcessingMerger\WordProcessingMerger.h . /Y

"C:\Program Files (x86)\swig\swig.exe" -c++ -python DocxFactory.i

del *.h /F /Q
