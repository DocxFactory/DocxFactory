
cd /D "%~dp0"

del *.cs /F /Q
del *.cxx /F /Q
del *.h /F /Q

copy ..\..\include\DocxFactory\WordProcessingCompiler\dfwc.h . /Y
copy ..\..\include\DocxFactory\WordProcessingMerger\dfw.h . /Y

"C:\Program Files (x86)\swig\swig.exe" -c++ -csharp -dllimport DocxFactoryCSharpExt -namespace DocxFactory WordProcessingCompiler.i
"C:\Program Files (x86)\swig\swig.exe" -c++ -csharp -dllimport DocxFactoryCSharpExt -namespace DocxFactory WordProcessingMerger.i

cscript.exe ".\fix_wchar.js"

del *wchar*.cs
del *.h /F /Q
