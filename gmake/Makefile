PLATFORM=$(shell uname -m)
FLAGS=-fpermissive -DNO_PNG
BUILD_PATH=./../../build/Linux/$(PLATFORM)/DocxFactory
INCLUDE_PATH=-I./../include -I./../../zint/include -I./../../boost/include -I./../../icu/include/Linux -I./../../imagemagick/include/Linux -I./../../rapidjson/include -I./../../uno/include/Linux -I./../../xercesc/include/Linux -I./../../zlib/include
LIB_INCLUDE_PATH=-I./../include/DocxFactory/WordProcessingCompiler -I./../include/DocxFactory/WordProcessingMerger
JAVA_INCLUDE_PATH=-I/jdk1.5.0_22/include -I/jdk1.5.0_22/include/linux
EXTERNAL_LIB=-l:libzint.so -l:libicudata.so.49 -l:libicui18n.so.49 -l:libicuio.so.49 -l:libicule.so.49 -l:libiculx.so.49 -l:libicutu.so.49 -l:libicuuc.so.49 -l:libMagick++-6.Q16.so.6 -l:libMagickCore-6.Q16.so.2 -l:libMagickWand-6.Q16.so.2 -l:libxerces-c-3.1.so -l:libzlib.so
UNO_LIB=-l:libuno_cppu.so.3 -l:libuno_cppuhelpergcc3.so.3 -l:libuno_sal.so.3 -l:libuno_salhelpergcc3.so.3
export LD_LIBRARY_PATH=.

BUILD_OBJ=word-processing-compiler word-processing-merger libDocxFactoryCSharpExt.so libDocxFactoryJavaExt.so libDocxFactory.so libUnoUtil.so libDocxFactoryLib.so
CSHARP_EXT_OBJ=CSharpWordProcessingCompiler_wrap.o CSharpWordProcessingMerger_wrap.o
JAVA_EXT_OBJ=JavaWordProcessingCompiler_wrap.o JavaWordProcessingMerger_wrap.o
WORD_PROCESSING_OBJ=dfwc.o dfw.o WordProcessingException.o InvalidMergeDataException.o InvalidItemNameException.o TemplateNotLoadedException.o CompileVersionMismatchException.o
CONVERTER_PRINTER_OBJ=ConversionNotSupportedException.o ConverterPrinter.o ConverterPrinterException.o ExportFormatNotSupportedException.o GradeGroup.o OdfConverterUtil.o OleException.o OpenOfficeUtil.o PrintingNotSupportedException.o UtilFailedException.o UtilNotFoundException.o
DOCX_COMPILER_OBJ=DocxCompilerAltChunkField.o DocxCompilerBarcodeField.o DocxCompilerBooleanField.o DocxCompilerBookmark.o DocxCompilerDateTimeField.o DocxCompilerField.o DocxCompilerFile.o DocxCompilerItem.o DocxCompilerItemFile.o DocxCompilerItemGroup.o DocxCompilerItemLayout.o DocxCompilerNumberField.o DocxCompilerPicField.o DocxCompilerStyle.o DocxCompilerStyleFile.o DocxCompilerSettingsFile.o DocxCompilerTableStyle.o DocxCompilerTextField.o DocxCompilerXmlString.o DocxCompilerId.o DocxCompilerChartField.o DocxCompilerChartFieldSeries.o DocxCompilerChartFieldSingleSeries.o DocxCompilerChartFieldFixedSeries.o DocxCompilerChartFieldXY.o DocxCompilerChartFieldXYSize.o
DOCX_MERGER_OBJ=DocxMergerAltChunkField.o DocxMergerBarcodeField.o DocxMergerBooleanField.o DocxMergerDateTimeField.o DocxMergerException.o DocxMergerField.o DocxMergerFile.o DocxMergerSettingsFile.o DocxMergerItem.o DocxMergerItemFile.o DocxMergerItemGroup.o DocxMergerItemLayout.o DocxMergerNumberField.o DocxMergerPasteAltChunkField.o DocxMergerPasteBarcodeField.o DocxMergerPasteBooleanField.o DocxMergerPasteDateTimeField.o DocxMergerPasteField.o DocxMergerPasteFieldGroup.o DocxMergerPasteItem.o DocxMergerPasteItemGroup.o DocxMergerPasteNumberField.o DocxMergerPastePicField.o DocxMergerPasteTextField.o DocxMergerPicField.o DocxMergerTextField.o DocxMergerXmlString.o DocxMergerId.o DocxMergerChartField.o DocxMergerChartFieldSeries.o DocxMergerChartFieldSingleSeries.o DocxMergerChartFieldFixedSeries.o DocxMergerChartFieldXY.o DocxMergerChartFieldXYSize.o DocxMergerChartAxis.o DocxMergerChartAxisText.o DocxMergerChartAxisNumber.o DocxMergerChartAxisDateTime.o DocxMergerChartValues.o DocxMergerPasteChartField.o DocxMergerPasteChartFieldSeries.o DocxMergerPasteChartFieldSingleSeries.o DocxMergerPasteChartFieldFixedSeries.o DocxMergerPasteChartFieldXY.o DocxMergerPasteChartFieldXYSize.o DocxMergerPasteChartAxis.o DocxMergerPasteChartAxisText.o DocxMergerPasteChartAxisNumber.o DocxMergerPasteChartAxisDateTime.o DocxMergerPasteChartValues.o
OPC_OBJ=ContentTypeNotFound.o OpcContentTypes.o OpcException.o OpcFunc.o OpcImageCache.o OpcImageFile.o OpcPackage.o OpcPart.o OpcRelationship.o OpcRelationships.o PartNotFoundException.o PartTypeNotFoundException.o
UNO_OBJ=PropertyMap.o UnoException.o UnoUtilImpl.o
BARCODE_OBJ=BarcodeFunc.o
CONSOLE_OBJ=ConsoleException.o ConsoleUsageException.o getopt.o GetOptException.o InvalidArgException.o InvalidOptException.o MissingArgException.o MissingOptException.o UnexpectedArgException.o UnexpectedOptException.o
JSON_OBJ=JsonFunc.o JsonException.o ParseJsonException.o ParseJsonFileException.o
LOCALE_OBJ=IcuException.o InvalidCodePageException.o InvalidDateFormatException.o InvalidDateTimeStrException.o InvalidFirstWeekDayException.o InvalidYearOffsetException.o LocaleException.o LocaleFunc.o
MATH_OBJ=MathFunc.o
OS_OBJ=DirNotFoundException.o FileExistsException.o FileNotFoundException.o FileNotOpenException.o FileNotRemovedException.o InvalidFileUrlException.o OsException.o OsFunc.o PathMissingExtException.o PathMissingFileException.o UnsupportedFileTypeException.o
PARSER_OBJ=ParserException.o ParserFunc.o UnclosedBracketException.o UnexpectedBracketException.o UnmatchedQuoteException.o
STR_OBJ=StrEntries.o StrFunc.o
TIME_OBJ=TimeFunc.o
UNIX_OBJ=UnixFunc.o
UTIL_OBJ=DocxFactoryException.o DocxFactoryTimer.o GeneralException.o UnknownException.o ArgTypeMismatchException.o OsNotCompatibleException.o
XML_OBJ=AsciiToXMLCh.o DomException.o InvalidXmlPathException.o NodeNotFoundException.o ParseXmlException.o ParseXmlFileException.o StrToXMLCh.o Utf8ToXMLCh.o XmlException.o XmlFunc.o XmlInitializer.o XmlTreeDriller.o
ZIP_OBJ=FileInfo.o MinizipException.o UnzipFile.o ZipException.o ZipFile.o ZipFileNotOpenException.o ZipFunc.o ZipStreamNotOpenException.o ZipStreamOpenException.o



all: build

build: $(BUILD_OBJ)
	rm -rf ./../../build/Linux/$(PLATFORM); \
	mkdir -p $(BUILD_PATH)/include; \
	mkdir -p $(BUILD_PATH)/src; \
	mkdir -p $(BUILD_PATH)/lib; \
	mkdir -p $(BUILD_PATH)/bin; \
	cp -f ./../include/DocxFactory/WordProcessingCompiler/dfwc.h $(BUILD_PATH)/include; \
	cp -f ./../include/DocxFactory/WordProcessingMerger/dfw.h $(BUILD_PATH)/include; \
	cp -f ./../include/DocxFactory/WordProcessingCompiler/WordProcessingCompiler.h $(BUILD_PATH)/include; \
	cp -f ./../include/DocxFactory/WordProcessingMerger/WordProcessingMerger.h $(BUILD_PATH)/include; \
	cp -f ./../src/WordProcessingCompiler/WordProcessingCompiler.cpp $(BUILD_PATH)/src; \
	cp -f ./../src/WordProcessingMerger/WordProcessingMerger.cpp $(BUILD_PATH)/src; \
	cp -f word-processing-compiler $(BUILD_PATH)/bin; \
	cp -f word-processing-merger $(BUILD_PATH)/bin; \
	cp -f libDocxFactoryCSharpExt.so $(BUILD_PATH)/lib; \
	cp -f libDocxFactoryJavaExt.so $(BUILD_PATH)/lib; \
	cp -f libDocxFactory.so $(BUILD_PATH)/lib; \
	cp -f libDocxFactoryLib.so $(BUILD_PATH)/lib; \
	cp -f libUnoUtil.so $(BUILD_PATH)/lib; \
	cp -f libzint.so $(BUILD_PATH)/lib; \
	cp -f libicudata.so.49 $(BUILD_PATH)/lib; \
	cp -f libicui18n.so.49 $(BUILD_PATH)/lib; \
	cp -f libicuio.so.49 $(BUILD_PATH)/lib; \
	cp -f libicule.so.49 $(BUILD_PATH)/lib; \
	cp -f libiculx.so.49 $(BUILD_PATH)/lib; \
	cp -f libicutu.so.49 $(BUILD_PATH)/lib; \
	cp -f libicuuc.so.49 $(BUILD_PATH)/lib; \
	cp -f libMagick++-6.Q16.so.6 $(BUILD_PATH)/lib; \
	cp -f libMagickCore-6.Q16.so.2 $(BUILD_PATH)/lib; \
	cp -f libMagickWand-6.Q16.so.2 $(BUILD_PATH)/lib; \
	cp -rf ./../../imagemagick/lib/Linux/$(PLATFORM)/coders $(BUILD_PATH)/lib; \
	cp -f /usr/lib64/liblzma.so.0 $(BUILD_PATH)/lib; \
	cp -f /usr/lib64/libltdl.so.3 $(BUILD_PATH)/lib; \
	cp -f /usr/lib64/libfftw3.so.3 $(BUILD_PATH)/lib; \
	cp -f /usr/lib/liblzma.so.0 $(BUILD_PATH)/lib; \
	cp -f /usr/lib/libltdl.so.3 $(BUILD_PATH)/lib; \
	cp -f /usr/lib/libfftw3.so.3 $(BUILD_PATH)/lib; \
	cp -f libxerces-c-3.1.so $(BUILD_PATH)/lib; \
	cp -f libzlib.so $(BUILD_PATH)/lib; \
	cp -rf ./../exercises $(BUILD_PATH); \
	cp -rf ./../progress $(BUILD_PATH); \
	cp -rf ./../python $(BUILD_PATH); \
	cp -rf ./../java $(BUILD_PATH); \
	cp -rf ./../csharp/Linux $(BUILD_PATH)/csharp; \
	chown -R root:root $(BUILD_PATH); \
	find $(BUILD_PATH) -type d -exec chmod 777 {} +; \
	find $(BUILD_PATH) -type f -exec chmod 666 {} +; \
	chmod 777 $(BUILD_PATH)/lib/*; \
	chmod 777 $(BUILD_PATH)/bin/*; \
	cd ./../../build/Linux/$(PLATFORM); \
	tar -zcvf ../../DocxFactoryLinux$(PLATFORM).tar.gz DocxFactory; \
	chown root:root ../../DocxFactoryLinux$(PLATFORM).tar.gz; \
	chmod 666 ../../DocxFactoryLinux$(PLATFORM).tar.gz;

clean:
	rm -rf *.so *.so.* *.o word-processing-compiler word-processing-merger



# CSharp Ext

libDocxFactoryCSharpExt.so: libDocxFactory.so $(CSHARP_EXT_OBJ)
	g++ $(FLAGS) -std=c++0x -shared -o $@ $(CSHARP_EXT_OBJ) -L. -l:libDocxFactory.so

CSharpWordProcessingCompiler_wrap.o: ./../csharp/Linux/WordProcessingCompiler_wrap.cxx
	g++ $(FLAGS) -std=c++0x -c -fpic ./../csharp/Linux/WordProcessingCompiler_wrap.cxx -o $@ $(LIB_INCLUDE_PATH)

CSharpWordProcessingMerger_wrap.o: ./../csharp/Linux/WordProcessingMerger_wrap.cxx
	g++ $(FLAGS) -std=c++0x -c -fpic ./../csharp/Linux/WordProcessingMerger_wrap.cxx -o $@ $(LIB_INCLUDE_PATH)



# Java Ext

libDocxFactoryJavaExt.so: libDocxFactory.so $(JAVA_EXT_OBJ)
	g++ $(FLAGS) -std=c++0x -shared -o $@ $(JAVA_EXT_OBJ) -L. -l:libDocxFactory.so

JavaWordProcessingCompiler_wrap.o: ./../java/WordProcessingCompiler_wrap.cxx
	g++ $(FLAGS) -std=c++0x -c -fpic ./../java/WordProcessingCompiler_wrap.cxx -o $@ $(LIB_INCLUDE_PATH) $(JAVA_INCLUDE_PATH)

JavaWordProcessingMerger_wrap.o: ./../java/WordProcessingMerger_wrap.cxx
	g++ $(FLAGS) -std=c++0x -c -fpic ./../java/WordProcessingMerger_wrap.cxx -o $@ $(LIB_INCLUDE_PATH) $(JAVA_INCLUDE_PATH)



# DocxFactory

word-processing-compiler: libDocxFactory.so
	g++ $(FLAGS) -std=c++0x -o word-processing-compiler ./../src/WordProcessingCompiler/word-processing-compiler.cpp -L. -l:libDocxFactory.so -l:libDocxFactoryLib.so $(INCLUDE_PATH)

word-processing-merger: libDocxFactory.so
	g++ $(FLAGS) -std=c++0x -o word-processing-merger ./../src/WordProcessingMerger/word-processing-merger.cpp -L. -l:libDocxFactory.so -l:libDocxFactoryLib.so $(INCLUDE_PATH)

libDocxFactory.so: libUnoUtil.so $(CONVERTER_PRINTER_OBJ) $(OPC_OBJ) $(WORD_PROCESSING_OBJ) $(DOCX_COMPILER_OBJ) $(DOCX_MERGER_OBJ)
	g++ $(FLAGS) -std=c++0x -shared -o libDocxFactory.so $(CONVERTER_PRINTER_OBJ) $(OPC_OBJ) $(WORD_PROCESSING_OBJ) $(DOCX_COMPILER_OBJ) $(DOCX_MERGER_OBJ) -L. -l:libDocxFactoryLib.so $(EXTERNAL_LIB) -ldl

libUnoUtil.so: libDocxFactoryLib.so libuno_cppu.so.3 libuno_cppuhelpergcc3.so.3 libuno_sal.so.3 libuno_salhelpergcc3.so.3 $(UNO_OBJ)
	g++ $(FLAGS) -std=c++0x -shared -o libUnoUtil.so $(UNO_OBJ) -L. -l:libDocxFactoryLib.so $(UNO_LIB)

libDocxFactoryLib.so: libzint.so libicudata.so.49 libicui18n.so.49 libicuio.so.49 libicule.so.49 libiculx.so.49 libicutu.so.49 libicuuc.so.49 libMagick++-6.Q16.so.6 libMagickCore-6.Q16.so.2 libMagickWand-6.Q16.so.2 libxerces-c-3.1.so libzlib.so $(BARCODE_OBJ) $(CONSOLE_OBJ) $(JSON_OBJ) $(LOCALE_OBJ) $(MATH_OBJ) $(OS_OBJ) $(PARSER_OBJ) $(STR_OBJ) $(TIME_OBJ) $(UNIX_OBJ) $(UTIL_OBJ) $(XML_OBJ) $(ZIP_OBJ)
	g++ $(FLAGS) -std=c++0x -shared -o libDocxFactoryLib.so $(BARCODE_OBJ) $(CONSOLE_OBJ) $(JSON_OBJ) $(LOCALE_OBJ) $(MATH_OBJ) $(OS_OBJ) $(PARSER_OBJ) $(STR_OBJ) $(TIME_OBJ) $(UNIX_OBJ) $(UTIL_OBJ) $(XML_OBJ) $(ZIP_OBJ) -L. $(EXTERNAL_LIB)



# WordProcessing

dfwc.o: ./../src/WordProcessingCompiler/dfwc.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/WordProcessingCompiler/dfwc.cpp -o $@ $(INCLUDE_PATH)

dfw.o: ./../src/WordProcessingMerger/dfw.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/WordProcessingMerger/dfw.cpp -o $@ $(INCLUDE_PATH)

WordProcessingException.o: ./../src/WordProcessingMerger/WordProcessingException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/WordProcessingMerger/WordProcessingException.cpp -o $@ $(INCLUDE_PATH)

InvalidMergeDataException.o: ./../src/WordProcessingMerger/InvalidMergeDataException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/WordProcessingMerger/InvalidMergeDataException.cpp -o $@ $(INCLUDE_PATH)

InvalidItemNameException.o: ./../src/WordProcessingMerger/InvalidItemNameException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/WordProcessingMerger/InvalidItemNameException.cpp -o $@ $(INCLUDE_PATH)

TemplateNotLoadedException.o: ./../src/WordProcessingMerger/TemplateNotLoadedException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/WordProcessingMerger/TemplateNotLoadedException.cpp -o $@ $(INCLUDE_PATH)

CompileVersionMismatchException.o: ./../src/WordProcessingMerger/CompileVersionMismatchException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/WordProcessingMerger/CompileVersionMismatchException.cpp -o $@ $(INCLUDE_PATH)



# DocxCompiler

DocxCompilerAltChunkField.o: ./../src/DocxCompiler/DocxCompilerAltChunkField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerAltChunkField.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerBarcodeField.o: ./../src/DocxCompiler/DocxCompilerBarcodeField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerBarcodeField.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerBooleanField.o: ./../src/DocxCompiler/DocxCompilerBooleanField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerBooleanField.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerBookmark.o: ./../src/DocxCompiler/DocxCompilerBookmark.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerBookmark.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerDateTimeField.o: ./../src/DocxCompiler/DocxCompilerDateTimeField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerDateTimeField.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerField.o: ./../src/DocxCompiler/DocxCompilerField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerField.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerFile.o: ./../src/DocxCompiler/DocxCompilerFile.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerFile.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerItem.o: ./../src/DocxCompiler/DocxCompilerItem.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerItem.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerItemFile.o: ./../src/DocxCompiler/DocxCompilerItemFile.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerItemFile.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerItemGroup.o: ./../src/DocxCompiler/DocxCompilerItemGroup.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerItemGroup.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerItemLayout.o: ./../src/DocxCompiler/DocxCompilerItemLayout.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerItemLayout.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerNumberField.o: ./../src/DocxCompiler/DocxCompilerNumberField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerNumberField.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerPicField.o: ./../src/DocxCompiler/DocxCompilerPicField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerPicField.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerStyle.o: ./../src/DocxCompiler/DocxCompilerStyle.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerStyle.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerStyleFile.o: ./../src/DocxCompiler/DocxCompilerStyleFile.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerStyleFile.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerSettingsFile.o: ./../src/DocxCompiler/DocxCompilerSettingsFile.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerSettingsFile.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerTableStyle.o: ./../src/DocxCompiler/DocxCompilerTableStyle.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerTableStyle.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerTextField.o: ./../src/DocxCompiler/DocxCompilerTextField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerTextField.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerXmlString.o: ./../src/DocxCompiler/DocxCompilerXmlString.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerXmlString.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerId.o: ./../src/DocxCompiler/DocxCompilerId.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerId.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerChartField.o: ./../src/DocxCompiler/DocxCompilerChartField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerChartField.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerChartFieldSeries.o: ./../src/DocxCompiler/DocxCompilerChartFieldSeries.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerChartFieldSeries.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerChartFieldSingleSeries.o: ./../src/DocxCompiler/DocxCompilerChartFieldSingleSeries.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerChartFieldSingleSeries.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerChartFieldFixedSeries.o: ./../src/DocxCompiler/DocxCompilerChartFieldFixedSeries.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerChartFieldFixedSeries.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerChartFieldXY.o: ./../src/DocxCompiler/DocxCompilerChartFieldXY.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerChartFieldXY.cpp -o $@ $(INCLUDE_PATH)

DocxCompilerChartFieldXYSize.o: ./../src/DocxCompiler/DocxCompilerChartFieldXYSize.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxCompiler/DocxCompilerChartFieldXYSize.cpp -o $@ $(INCLUDE_PATH)



# DocxMerger

DocxMergerAltChunkField.o: ./../src/DocxMerger/DocxMergerAltChunkField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerAltChunkField.cpp -o $@ $(INCLUDE_PATH)

DocxMergerBarcodeField.o: ./../src/DocxMerger/DocxMergerBarcodeField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerBarcodeField.cpp -o $@ $(INCLUDE_PATH)

DocxMergerBooleanField.o: ./../src/DocxMerger/DocxMergerBooleanField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerBooleanField.cpp -o $@ $(INCLUDE_PATH)

DocxMergerDateTimeField.o: ./../src/DocxMerger/DocxMergerDateTimeField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerDateTimeField.cpp -o $@ $(INCLUDE_PATH)

DocxMergerException.o: ./../src/DocxMerger/DocxMergerException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerException.cpp -o $@ $(INCLUDE_PATH)

DocxMergerField.o: ./../src/DocxMerger/DocxMergerField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerField.cpp -o $@ $(INCLUDE_PATH)

DocxMergerFile.o: ./../src/DocxMerger/DocxMergerFile.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerFile.cpp -o $@ $(INCLUDE_PATH)

DocxMergerSettingsFile.o: ./../src/DocxMerger/DocxMergerSettingsFile.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerSettingsFile.cpp -o $@ $(INCLUDE_PATH)

DocxMergerItem.o: ./../src/DocxMerger/DocxMergerItem.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerItem.cpp -o $@ $(INCLUDE_PATH)

DocxMergerItemFile.o: ./../src/DocxMerger/DocxMergerItemFile.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerItemFile.cpp -o $@ $(INCLUDE_PATH)

DocxMergerItemGroup.o: ./../src/DocxMerger/DocxMergerItemGroup.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerItemGroup.cpp -o $@ $(INCLUDE_PATH)

DocxMergerItemLayout.o: ./../src/DocxMerger/DocxMergerItemLayout.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerItemLayout.cpp -o $@ $(INCLUDE_PATH)

DocxMergerNumberField.o: ./../src/DocxMerger/DocxMergerNumberField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerNumberField.cpp -o $@ $(INCLUDE_PATH)

DocxMergerPasteAltChunkField.o: ./../src/DocxMerger/DocxMergerPasteAltChunkField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPasteAltChunkField.cpp -o $@ $(INCLUDE_PATH)

DocxMergerPasteBarcodeField.o: ./../src/DocxMerger/DocxMergerPasteBarcodeField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPasteBarcodeField.cpp -o $@ $(INCLUDE_PATH)

DocxMergerPasteBooleanField.o: ./../src/DocxMerger/DocxMergerPasteBooleanField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPasteBooleanField.cpp -o $@ $(INCLUDE_PATH)

DocxMergerPasteDateTimeField.o: ./../src/DocxMerger/DocxMergerPasteDateTimeField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPasteDateTimeField.cpp -o $@ $(INCLUDE_PATH)

DocxMergerPasteField.o: ./../src/DocxMerger/DocxMergerPasteField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPasteField.cpp -o $@ $(INCLUDE_PATH)

DocxMergerPasteFieldGroup.o: ./../src/DocxMerger/DocxMergerPasteFieldGroup.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPasteFieldGroup.cpp -o $@ $(INCLUDE_PATH)

DocxMergerPasteItem.o: ./../src/DocxMerger/DocxMergerPasteItem.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPasteItem.cpp -o $@ $(INCLUDE_PATH)

DocxMergerPasteItemGroup.o: ./../src/DocxMerger/DocxMergerPasteItemGroup.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPasteItemGroup.cpp -o $@ $(INCLUDE_PATH)

DocxMergerPasteNumberField.o: ./../src/DocxMerger/DocxMergerPasteNumberField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPasteNumberField.cpp -o $@ $(INCLUDE_PATH)

DocxMergerPastePicField.o: ./../src/DocxMerger/DocxMergerPastePicField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPastePicField.cpp -o $@ $(INCLUDE_PATH)

DocxMergerPasteTextField.o: ./../src/DocxMerger/DocxMergerPasteTextField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPasteTextField.cpp -o $@ $(INCLUDE_PATH)

DocxMergerPicField.o: ./../src/DocxMerger/DocxMergerPicField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPicField.cpp -o $@ $(INCLUDE_PATH)

DocxMergerTextField.o: ./../src/DocxMerger/DocxMergerTextField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerTextField.cpp -o $@ $(INCLUDE_PATH)

DocxMergerXmlString.o: ./../src/DocxMerger/DocxMergerXmlString.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerXmlString.cpp -o $@ $(INCLUDE_PATH)

DocxMergerId.o: ./../src/DocxMerger/DocxMergerId.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerId.cpp -o $@ $(INCLUDE_PATH)

DocxMergerChartField.o: ./../src/DocxMerger/DocxMergerChartField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerChartField.cpp -o $@ $(INCLUDE_PATH)

DocxMergerChartFieldSeries.o: ./../src/DocxMerger/DocxMergerChartFieldSeries.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerChartFieldSeries.cpp -o $@ $(INCLUDE_PATH)

DocxMergerChartFieldSingleSeries.o: ./../src/DocxMerger/DocxMergerChartFieldSingleSeries.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerChartFieldSingleSeries.cpp -o $@ $(INCLUDE_PATH)

DocxMergerChartFieldFixedSeries.o: ./../src/DocxMerger/DocxMergerChartFieldFixedSeries.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerChartFieldFixedSeries.cpp -o $@ $(INCLUDE_PATH)

DocxMergerChartFieldXY.o: ./../src/DocxMerger/DocxMergerChartFieldXY.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerChartFieldXY.cpp -o $@ $(INCLUDE_PATH)

DocxMergerChartFieldXYSize.o: ./../src/DocxMerger/DocxMergerChartFieldXYSize.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerChartFieldXYSize.cpp -o $@ $(INCLUDE_PATH)	

DocxMergerChartAxis.o: ./../src/DocxMerger/DocxMergerChartAxis.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerChartAxis.cpp -o $@ $(INCLUDE_PATH)

DocxMergerChartAxisText.o: ./../src/DocxMerger/DocxMergerChartAxisText.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerChartAxisText.cpp -o $@ $(INCLUDE_PATH)
	
DocxMergerChartAxisNumber.o: ./../src/DocxMerger/DocxMergerChartAxisNumber.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerChartAxisNumber.cpp -o $@ $(INCLUDE_PATH)

DocxMergerChartAxisDateTime.o: ./../src/DocxMerger/DocxMergerChartAxisDateTime.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerChartAxisDateTime.cpp -o $@ $(INCLUDE_PATH)

DocxMergerChartValues.o: ./../src/DocxMerger/DocxMergerChartValues.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerChartValues.cpp -o $@ $(INCLUDE_PATH)

DocxMergerPasteChartField.o: ./../src/DocxMerger/DocxMergerPasteChartField.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPasteChartField.cpp -o $@ $(INCLUDE_PATH)

DocxMergerPasteChartFieldSeries.o: ./../src/DocxMerger/DocxMergerPasteChartFieldSeries.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPasteChartFieldSeries.cpp -o $@ $(INCLUDE_PATH)

DocxMergerPasteChartFieldSingleSeries.o: ./../src/DocxMerger/DocxMergerPasteChartFieldSingleSeries.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPasteChartFieldSingleSeries.cpp -o $@ $(INCLUDE_PATH)

DocxMergerPasteChartFieldFixedSeries.o: ./../src/DocxMerger/DocxMergerPasteChartFieldFixedSeries.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPasteChartFieldFixedSeries.cpp -o $@ $(INCLUDE_PATH)

DocxMergerPasteChartFieldXY.o: ./../src/DocxMerger/DocxMergerPasteChartFieldXY.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPasteChartFieldXY.cpp -o $@ $(INCLUDE_PATH)

DocxMergerPasteChartFieldXYSize.o: ./../src/DocxMerger/DocxMergerPasteChartFieldXYSize.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPasteChartFieldXYSize.cpp -o $@ $(INCLUDE_PATH)

DocxMergerPasteChartAxis.o: ./../src/DocxMerger/DocxMergerPasteChartAxis.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPasteChartAxis.cpp -o $@ $(INCLUDE_PATH)

DocxMergerPasteChartAxisText.o: ./../src/DocxMerger/DocxMergerPasteChartAxisText.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPasteChartAxisText.cpp -o $@ $(INCLUDE_PATH)
	
DocxMergerPasteChartAxisNumber.o: ./../src/DocxMerger/DocxMergerPasteChartAxisNumber.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPasteChartAxisNumber.cpp -o $@ $(INCLUDE_PATH)

DocxMergerPasteChartAxisDateTime.o: ./../src/DocxMerger/DocxMergerPasteChartAxisDateTime.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPasteChartAxisDateTime.cpp -o $@ $(INCLUDE_PATH)

DocxMergerPasteChartValues.o: ./../src/DocxMerger/DocxMergerPasteChartValues.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/DocxMerger/DocxMergerPasteChartValues.cpp -o $@ $(INCLUDE_PATH)



# ConverterPrinter

ConversionNotSupportedException.o: ./../src/ConverterPrinter/ConversionNotSupportedException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/ConverterPrinter/ConversionNotSupportedException.cpp -o $@ $(INCLUDE_PATH)

ConverterPrinter.o: ./../src/ConverterPrinter/ConverterPrinter.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/ConverterPrinter/ConverterPrinter.cpp -o $@ $(INCLUDE_PATH)

ConverterPrinterException.o: ./../src/ConverterPrinter/ConverterPrinterException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/ConverterPrinter/ConverterPrinterException.cpp -o $@ $(INCLUDE_PATH)

ExportFormatNotSupportedException.o: ./../src/ConverterPrinter/ExportFormatNotSupportedException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/ConverterPrinter/ExportFormatNotSupportedException.cpp -o $@ $(INCLUDE_PATH)

GradeGroup.o: ./../src/ConverterPrinter/GradeGroup.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/ConverterPrinter/GradeGroup.cpp -o $@ $(INCLUDE_PATH)

OdfConverterUtil.o: ./../src/ConverterPrinter/OdfConverterUtil.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/ConverterPrinter/OdfConverterUtil.cpp -o $@ $(INCLUDE_PATH)

OleException.o: ./../src/ConverterPrinter/OleException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/ConverterPrinter/OleException.cpp -o $@ $(INCLUDE_PATH)

OpenOfficeUtil.o: ./../src/ConverterPrinter/OpenOfficeUtil.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/ConverterPrinter/OpenOfficeUtil.cpp -o $@ $(INCLUDE_PATH)

PrintingNotSupportedException.o: ./../src/ConverterPrinter/PrintingNotSupportedException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/ConverterPrinter/PrintingNotSupportedException.cpp -o $@ $(INCLUDE_PATH)

UtilFailedException.o: ./../src/ConverterPrinter/UtilFailedException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/ConverterPrinter/UtilFailedException.cpp -o $@ $(INCLUDE_PATH)

UtilNotFoundException.o: ./../src/ConverterPrinter/UtilNotFoundException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/ConverterPrinter/UtilNotFoundException.cpp -o $@ $(INCLUDE_PATH)



# opc

ContentTypeNotFound.o: ./../src/opc/ContentTypeNotFound.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/opc/ContentTypeNotFound.cpp -o $@ $(INCLUDE_PATH)

OpcContentTypes.o: ./../src/opc/OpcContentTypes.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/opc/OpcContentTypes.cpp -o $@ $(INCLUDE_PATH)

OpcException.o: ./../src/opc/OpcException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/opc/OpcException.cpp -o $@ $(INCLUDE_PATH)

OpcFunc.o: ./../src/opc/OpcFunc.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/opc/OpcFunc.cpp -o $@ $(INCLUDE_PATH)

OpcImageCache.o: ./../src/opc/OpcImageCache.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/opc/OpcImageCache.cpp -o $@ $(INCLUDE_PATH)

OpcImageFile.o: ./../src/opc/OpcImageFile.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/opc/OpcImageFile.cpp -o $@ $(INCLUDE_PATH)

OpcPackage.o: ./../src/opc/OpcPackage.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/opc/OpcPackage.cpp -o $@ $(INCLUDE_PATH)

OpcPart.o: ./../src/opc/OpcPart.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/opc/OpcPart.cpp -o $@ $(INCLUDE_PATH)

OpcRelationship.o: ./../src/opc/OpcRelationship.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/opc/OpcRelationship.cpp -o $@ $(INCLUDE_PATH)

OpcRelationships.o: ./../src/opc/OpcRelationships.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/opc/OpcRelationships.cpp -o $@ $(INCLUDE_PATH)

PartNotFoundException.o: ./../src/opc/PartNotFoundException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/opc/PartNotFoundException.cpp -o $@ $(INCLUDE_PATH)

PartTypeNotFoundException.o: ./../src/opc/PartTypeNotFoundException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/opc/PartTypeNotFoundException.cpp -o $@ $(INCLUDE_PATH)



# libUnoUtil

PropertyMap.o: ./../src/UnoUtil/PropertyMap.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/UnoUtil/PropertyMap.cpp -o $@ $(INCLUDE_PATH)

UnoException.o: ./../src/UnoUtil/UnoException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/UnoUtil/UnoException.cpp -o $@ $(INCLUDE_PATH)

UnoUtilImpl.o: ./../src/UnoUtil/UnoUtilImpl.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic -DUNX -DGCC -DLINUX -DCPPU_ENV=gcc3 ./../src/UnoUtil/UnoUtilImpl.cpp -o $@ $(INCLUDE_PATH)


	
# barcode

BarcodeFunc.o: ./../src/barcode/BarcodeFunc.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/barcode/BarcodeFunc.cpp -o $@ $(INCLUDE_PATH)



# json

JsonFunc.o: ./../src/json/JsonFunc.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/json/JsonFunc.cpp -o $@ $(INCLUDE_PATH)

JsonException.o: ./../src/json/JsonException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/json/JsonException.cpp -o $@ $(INCLUDE_PATH)

ParseJsonException.o: ./../src/json/ParseJsonException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/json/ParseJsonException.cpp -o $@ $(INCLUDE_PATH)

ParseJsonFileException.o: ./../src/json/ParseJsonFileException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/json/ParseJsonFileException.cpp -o $@ $(INCLUDE_PATH)



# console
	
ConsoleException.o: ./../src/console/ConsoleException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/console/ConsoleException.cpp -o $@ $(INCLUDE_PATH)

ConsoleUsageException.o: ./../src/console/ConsoleUsageException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/console/ConsoleUsageException.cpp -o $@ $(INCLUDE_PATH)

getopt.o: ./../src/console/getopt.c
	gcc -c -fpic ./../src/console/getopt.c -o $@ $(INCLUDE_PATH)

GetOptException.o: ./../src/console/GetOptException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/console/GetOptException.cpp -o $@ $(INCLUDE_PATH)

InvalidArgException.o: ./../src/console/InvalidArgException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/console/InvalidArgException.cpp -o $@ $(INCLUDE_PATH)

InvalidOptException.o: ./../src/console/InvalidOptException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/console/InvalidOptException.cpp -o $@ $(INCLUDE_PATH)

MissingArgException.o: ./../src/console/MissingArgException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/console/MissingArgException.cpp -o $@ $(INCLUDE_PATH)

MissingOptException.o: ./../src/console/MissingOptException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/console/MissingOptException.cpp -o $@ $(INCLUDE_PATH)

UnexpectedArgException.o: ./../src/console/UnexpectedArgException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/console/UnexpectedArgException.cpp -o $@ $(INCLUDE_PATH)

UnexpectedOptException.o: ./../src/console/UnexpectedOptException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/console/UnexpectedOptException.cpp -o $@ $(INCLUDE_PATH)



# locale

IcuException.o: ./../src/locale/IcuException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/locale/IcuException.cpp -o $@ $(INCLUDE_PATH)

InvalidCodePageException.o: ./../src/locale/InvalidCodePageException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/locale/InvalidCodePageException.cpp -o $@ $(INCLUDE_PATH)

InvalidDateFormatException.o: ./../src/locale/InvalidDateFormatException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/locale/InvalidDateFormatException.cpp -o $@ $(INCLUDE_PATH)

InvalidDateTimeStrException.o: ./../src/locale/InvalidDateTimeStrException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/locale/InvalidDateTimeStrException.cpp -o $@ $(INCLUDE_PATH)

InvalidFirstWeekDayException.o: ./../src/locale/InvalidFirstWeekDayException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/locale/InvalidFirstWeekDayException.cpp -o $@ $(INCLUDE_PATH)

InvalidYearOffsetException.o: ./../src/locale/InvalidYearOffsetException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/locale/InvalidYearOffsetException.cpp -o $@ $(INCLUDE_PATH)

LocaleException.o: ./../src/locale/LocaleException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/locale/LocaleException.cpp -o $@ $(INCLUDE_PATH)

LocaleFunc.o: ./../src/locale/LocaleFunc.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/locale/LocaleFunc.cpp -o $@ $(INCLUDE_PATH)



# math
	
MathFunc.o: ./../src/math/MathFunc.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/math/MathFunc.cpp -o $@ $(INCLUDE_PATH)



# os

DirNotFoundException.o: ./../src/os/DirNotFoundException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/os/DirNotFoundException.cpp -o $@ $(INCLUDE_PATH)

FileExistsException.o: ./../src/os/FileExistsException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/os/FileExistsException.cpp -o $@ $(INCLUDE_PATH)

FileNotFoundException.o: ./../src/os/FileNotFoundException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/os/FileNotFoundException.cpp -o $@ $(INCLUDE_PATH)

FileNotOpenException.o: ./../src/os/FileNotOpenException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/os/FileNotOpenException.cpp -o $@ $(INCLUDE_PATH)

FileNotRemovedException.o: ./../src/os/FileNotRemovedException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/os/FileNotRemovedException.cpp -o $@ $(INCLUDE_PATH)

InvalidFileUrlException.o: ./../src/os/InvalidFileUrlException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/os/InvalidFileUrlException.cpp -o $@ $(INCLUDE_PATH)

OsException.o: ./../src/os/OsException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/os/OsException.cpp -o $@ $(INCLUDE_PATH)

OsFunc.o: ./../src/os/OsFunc.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/os/OsFunc.cpp -o $@ $(INCLUDE_PATH)

PathMissingExtException.o: ./../src/os/PathMissingExtException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/os/PathMissingExtException.cpp -o $@ $(INCLUDE_PATH)

PathMissingFileException.o: ./../src/os/PathMissingFileException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/os/PathMissingFileException.cpp -o $@ $(INCLUDE_PATH)

UnsupportedFileTypeException.o: ./../src/os/UnsupportedFileTypeException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/os/UnsupportedFileTypeException.cpp -o $@ $(INCLUDE_PATH)
	


# parser

ParserException.o: ./../src/parser/ParserException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/parser/ParserException.cpp -o $@ $(INCLUDE_PATH)

ParserFunc.o: ./../src/parser/ParserFunc.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/parser/ParserFunc.cpp -o $@ $(INCLUDE_PATH)

UnclosedBracketException.o: ./../src/parser/UnclosedBracketException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/parser/UnclosedBracketException.cpp -o $@ $(INCLUDE_PATH)

UnexpectedBracketException.o: ./../src/parser/UnexpectedBracketException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/parser/UnexpectedBracketException.cpp -o $@ $(INCLUDE_PATH)

UnmatchedQuoteException.o: ./../src/parser/UnmatchedQuoteException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/parser/UnmatchedQuoteException.cpp -o $@ $(INCLUDE_PATH)



# str

StrEntries.o: ./../src/str/StrEntries.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/str/StrEntries.cpp -o $@ $(INCLUDE_PATH)

StrFunc.o: ./../src/str/StrFunc.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/str/StrFunc.cpp -o $@ $(INCLUDE_PATH)



# time

TimeFunc.o: ./../src/time/TimeFunc.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/time/TimeFunc.cpp -o $@ $(INCLUDE_PATH)



# unix

UnixFunc.o: ./../src/unix/UnixFunc.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/unix/UnixFunc.cpp -o $@ $(INCLUDE_PATH)



# util

ArgTypeMismatchException.o: ./../src/util/ArgTypeMismatchException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/util/ArgTypeMismatchException.cpp -o $@ $(INCLUDE_PATH)

DocxFactoryException.o: ./../src/util/DocxFactoryException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/util/DocxFactoryException.cpp -o $@ $(INCLUDE_PATH)

DocxFactoryTimer.o: ./../src/util/DocxFactoryTimer.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/util/DocxFactoryTimer.cpp -o $@ $(INCLUDE_PATH)

GeneralException.o: ./../src/util/GeneralException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/util/GeneralException.cpp -o $@ $(INCLUDE_PATH)

OsNotCompatibleException.o: ./../src/util/OsNotCompatibleException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/util/OsNotCompatibleException.cpp -o $@ $(INCLUDE_PATH)

UnknownException.o: ./../src/util/UnknownException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/util/UnknownException.cpp -o $@ $(INCLUDE_PATH)



# xml

AsciiToXMLCh.o: ./../src/xml/AsciiToXMLCh.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/xml/AsciiToXMLCh.cpp -o $@ $(INCLUDE_PATH)

DomException.o: ./../src/xml/DomException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/xml/DomException.cpp -o $@ $(INCLUDE_PATH)

InvalidXmlPathException.o: ./../src/xml/InvalidXmlPathException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/xml/InvalidXmlPathException.cpp -o $@ $(INCLUDE_PATH)

NodeNotFoundException.o: ./../src/xml/NodeNotFoundException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/xml/NodeNotFoundException.cpp -o $@ $(INCLUDE_PATH)

ParseXmlException.o: ./../src/xml/ParseXmlException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/xml/ParseXmlException.cpp -o $@ $(INCLUDE_PATH)

ParseXmlFileException.o: ./../src/xml/ParseXmlFileException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/xml/ParseXmlFileException.cpp -o $@ $(INCLUDE_PATH)

StrToXMLCh.o: ./../src/xml/StrToXMLCh.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/xml/StrToXMLCh.cpp -o $@ $(INCLUDE_PATH)

Utf8ToXMLCh.o: ./../src/xml/Utf8ToXMLCh.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/xml/Utf8ToXMLCh.cpp -o $@ $(INCLUDE_PATH)

XmlException.o: ./../src/xml/XmlException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/xml/XmlException.cpp -o $@ $(INCLUDE_PATH)

XmlFunc.o: ./../src/xml/XmlFunc.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/xml/XmlFunc.cpp -o $@ $(INCLUDE_PATH)

XmlInitializer.o: ./../src/xml/XmlInitializer.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/xml/XmlInitializer.cpp -o $@ $(INCLUDE_PATH)

XmlTreeDriller.o: ./../src/xml/XmlTreeDriller.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/xml/XmlTreeDriller.cpp -o $@ $(INCLUDE_PATH)



# zip
	
FileInfo.o: ./../src/zip/FileInfo.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/zip/FileInfo.cpp -o $@ $(INCLUDE_PATH)

MinizipException.o: ./../src/zip/MinizipException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/zip/MinizipException.cpp -o $@ $(INCLUDE_PATH)

UnzipFile.o: ./../src/zip/UnzipFile.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/zip/UnzipFile.cpp -o $@ $(INCLUDE_PATH)

ZipException.o: ./../src/zip/ZipException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/zip/ZipException.cpp -o $@ $(INCLUDE_PATH)

ZipFile.o: ./../src/zip/ZipFile.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/zip/ZipFile.cpp -o $@ $(INCLUDE_PATH)

ZipFileNotOpenException.o: ./../src/zip/ZipFileNotOpenException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/zip/ZipFileNotOpenException.cpp -o $@ $(INCLUDE_PATH)

ZipFunc.o: ./../src/zip/ZipFunc.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/zip/ZipFunc.cpp -o $@ $(INCLUDE_PATH)

ZipStreamNotOpenException.o: ./../src/zip/ZipStreamNotOpenException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/zip/ZipStreamNotOpenException.cpp -o $@ $(INCLUDE_PATH)

ZipStreamOpenException.o: ./../src/zip/ZipStreamOpenException.cpp
	g++ $(FLAGS) -std=c++0x -c -fpic ./../src/zip/ZipStreamOpenException.cpp -o $@ $(INCLUDE_PATH)
	


# external libs

libuno_cppu.so.3: ./../../uno/lib/Linux/$(PLATFORM)/$@
	cp -f ./../../uno/lib/Linux/$(PLATFORM)/$@ .

libuno_cppuhelpergcc3.so.3: ./../../uno/lib/Linux/$(PLATFORM)/$@
	cp -f ./../../uno/lib/Linux/$(PLATFORM)/$@ .

libuno_sal.so.3: ./../../uno/lib/Linux/$(PLATFORM)/$@
	cp -f ./../../uno/lib/Linux/$(PLATFORM)/$@ .

libuno_salhelpergcc3.so.3: ./../../uno/lib/Linux/$(PLATFORM)/$@
	cp -f ./../../uno/lib/Linux/$(PLATFORM)/$@ .

libzint.so: ./../../zint/lib/Linux/$(PLATFORM)/$@
	cp -f ./../../zint/lib/Linux/$(PLATFORM)/$@ .

libicudata.so.49: ./../../icu/lib/Linux/$(PLATFORM)/$@
	cp -f ./../../icu/lib/Linux/$(PLATFORM)/$@ .

libicui18n.so.49: ./../../icu/lib/Linux/$(PLATFORM)/$@
	cp -f ./../../icu/lib/Linux/$(PLATFORM)/$@ .

libicuio.so.49: ./../../icu/lib/Linux/$(PLATFORM)/$@
	cp -f ./../../icu/lib/Linux/$(PLATFORM)/$@ .

libicule.so.49: ./../../icu/lib/Linux/$(PLATFORM)/$@
	cp -f ./../../icu/lib/Linux/$(PLATFORM)/$@ .

libiculx.so.49: ./../../icu/lib/Linux/$(PLATFORM)/$@
	cp -f ./../../icu/lib/Linux/$(PLATFORM)/$@ .

libicutu.so.49: ./../../icu/lib/Linux/$(PLATFORM)/$@
	cp -f ./../../icu/lib/Linux/$(PLATFORM)/$@ .

libicuuc.so.49: ./../../icu/lib/Linux/$(PLATFORM)/$@
	cp -f ./../../icu/lib/Linux/$(PLATFORM)/$@ .

libMagick++-6.Q16.so.6: ./../../imagemagick/lib/Linux/$(PLATFORM)/$@
	cp -f ./../../imagemagick/lib/Linux/$(PLATFORM)/$@ .

libMagickCore-6.Q16.so.2: ./../../imagemagick/lib/Linux/$(PLATFORM)/$@
	cp -f ./../../imagemagick/lib/Linux/$(PLATFORM)/$@ .

libMagickWand-6.Q16.so.2: ./../../imagemagick/lib/Linux/$(PLATFORM)/$@
	cp -f ./../../imagemagick/lib/Linux/$(PLATFORM)/$@ .

libxerces-c-3.1.so: ./../../xercesc/lib/Linux/$(PLATFORM)/$@
	cp -f ./../../xercesc/lib/Linux/$(PLATFORM)/$@ .

libzlib.so: ./../../zlib/lib/Linux/$(PLATFORM)/$@
	cp -f ./../../zlib/lib/Linux/$(PLATFORM)/$@ .
