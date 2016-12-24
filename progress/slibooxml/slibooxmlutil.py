
from sys import argv, exit
from optparse import OptionParser
from os.path import abspath, isfile, splitext

import uno
from com.sun.star.beans import PropertyValue
from com.sun.star.task import ErrorCodeIOException
from com.sun.star.connection import NoConnectException



DEFAULT_OPENOFFICE_PORT = 8100

FILTER_MAP = {

    "Spreadsheet": {
        "xls": {"FilterName": "MS Excel 97"},
        "xlt": {"FilterName": "MS Excel 97 Vorlage/Template"},
        # "xml": {"FilterName": "MS Excel 2003 XML"},

        "ods": {"FilterName": "calc8"},
        "sdc": {"FilterName": "StarCalc 5.0"},
        "sxc": {"FilterName": "StarOffice XML (Calc)"},
        "uos": {"FilterName": "UOF spreadsheet"},

        "pdf": {"FilterName": "calc_pdf_Export"},
        "htm": {"FilterName": "HTML (StarCalc)"},
        "html": {"FilterName": "HTML (StarCalc)"},
        "xhtml": {"FilterName": "XHTML Calc File"},

        "dbf": {"FilterName": "dBase"},
        "dif": {"FilterName": "DIF"},
        "slk": {"FilterName": "SYLK"},
        "csv": {"FilterName": "Text - txt - csv (StarCalc)", "FilterOptions": "44,34,0"},
        "tsv": {"FilterName": "Text - txt - csv (StarCalc)", "FilterOptions": "9,34,0"}
    },

    "Text": {
        "doc": {"FilterName": "MS Word 97"},
        "dot": {"FilterName": "MS Word 97"},
        # "xml": {"MS Word 2003 XML"},

        "odt": {"FilterName": "writer8"},
        "sdw": {"FilterName": "StarWriter 5.0"},
        "sxw": {"FilterName": "StarOffice XML (Writer)"},
        "uot": {"FilterName": "UOF text"},

        "pdf": {"FilterName": "writer_pdf_Export"},
        "htm": {"FilterName": "HTML (StarWriter)"},
        "html": {"FilterName": "HTML (StarWriter)"},
        "xhtml": {"FilterName": "XHTML Writer File"},

        "ltx": {"FilterName": "LaTeX_Writer"},
        "bib": {"FilterName": "BibTeX_Writer"},
        "rtf": {"FilterName": "Rich Text Format"},
        "txt": {"FilterName": "Text"}
    },

    "Presentation": {
        "ppt": {"FilterName": "MS PowerPoint 97"},

        "odp": {"FilterName": "impress8"},
        "sdp": {"FilterName": "StarImpress 5.0"},
        "sxi": {"FilterName": "StarOffice XML (Impress)"},
        "uop": {"FilterName": "UOF presentation"},

        "pdf": {"FilterName": "impress_pdf_Export"},
        "swf": {"FilterName": "impress_flash_Export"},
        "svg": {"FilterName": "impress_svg_Export"},
        "htm": {"FilterName": "impress_html_Export"},
        "html": {"FilterName": "impress_html_Export"},
        "xhtml": {"FilterName": "XHTML Impress File"},

        "bmp": {"FilterName": "impress_bmp_Export"},
        "gif": {"FilterName": "impress_gif_Export"},
        "jpg": {"FilterName": "impress_jpg_Export"},
        "jpeg": {"FilterName": "impress_jpg_Export"},
        "png": {"FilterName": "impress_png_Export"},
        "tif": {"FilterName": "impress_tif_Export"},
        "tiff": {"FilterName": "impress_tif_Export"},

        "xpm": {"FilterName": "impress_xpm_Export"},
        "wmf": {"FilterName": "impress_wmf_Export"},
        "svm": {"FilterName": "impress_svm_Export"},
        "emf": {"FilterName": "impress_emf_Export"},
        "eps": {"FilterName": "impress_eps_Export"},
        "met": {"FilterName": "impress_met_Export"},
        "pbm": {"FilterName": "impress_pbm_Export"},
        "pct": {"FilterName": "impress_pct_Export"},
        "pgm": {"FilterName": "impress_pgm_Export"},
        "ppm": {"FilterName": "impress_ppm_Export"},
        "ras": {"FilterName": "impress_ras_Export"},
        "pwp": {"FilterName": "placeware_Export"}
    },

    "Drawing": {
        "odg": {"FilterName": "draw8"},
        "sdd": {"FilterName": "StarDraw 5.0"},
        "sda": {"FilterName": "StarDraw 5.0"},
        "sxd": {"FilterName": "StarOffice XML (Draw)"},

        "pdf": {"FilterName": "draw_pdf_Export"},
        "swf": {"FilterName": "draw_flash_Export"},
        "svg": {"FilterName": "draw_svg_Export"},
        "htm": {"FilterName": "draw_html_Export"},
        "html": {"FilterName": "draw_html_Export"},
        "xhtml": {"FilterName": "XHTML Draw File"},

        "bmp": {"FilterName": "draw_bmp_Export"},
        "gif": {"FilterName": "draw_gif_Export"},
        "jpg": {"FilterName": "draw_jpg_Export"},
        "jpeg": {"FilterName": "draw_jpg_Export"},
        "png": {"FilterName": "draw_png_Export"},
        "tif": {"FilterName": "draw_tif_Export"},
        "tiff": {"FilterName": "draw_tif_Export"},

        "ppm": {"FilterName": "draw_ppm_Export"},
        "wmf": {"FilterName": "draw_wmf_Export"},
        "pgm": {"FilterName": "draw_pgm_Export"},
        "pbm": {"FilterName": "draw_pbm_Export"},
        "emf": {"FilterName": "draw_emf_Export"},
        "xpm": {"FilterName": "draw_xpm_Export"},
        "ras": {"FilterName": "draw_ras_Export"},
        "eps": {"FilterName": "draw_eps_Export"},
        "svm": {"FilterName": "draw_svm_Export"}
    }
}



class DocumentException(Exception):

    def __init__(self, errmsg):

        self.args = (errmsg)
        self.errmsg = errmsg

    def __str__(self):

        return self.message

class Document:

    def __init__(self, fileName, port):

        localContext = uno.getComponentContext()
        resolver = localContext.ServiceManager.createInstanceWithContext("com.sun.star.bridge.UnoUrlResolver", localContext)

        try:
            context = resolver.resolve("uno:socket,host=localhost,port=%s;urp;StarOffice.ComponentContext" % port)
        except NoConnectException:
            raise DocumentException, "failed to connect to OpenOffice.org on port %s" % port

        desktop = context.ServiceManager.createInstanceWithContext("com.sun.star.frame.Desktop", context)

        self.fileName = fileName
        self.fileUrl = self._toFileUrl(self.fileName)
        self.document = desktop.loadComponentFromURL(self.fileUrl, "_blank", 0, self._toProperties(Hidden=True))

        try:
            self.document.refresh()
        except AttributeError:
            pass

    def __del__(self):

        self.document.close(True)



    def convert(self, outputFile):

        outputUrl = self._toFileUrl(outputFile)
        outputExt = self._getFileExt(outputFile)

        filterProps = self._filterProps(self.document, outputExt)
        filterProps["Overwrite"] = True

        self.document.storeToURL(outputUrl, self._toProperties(**filterProps))

    def printout(self, printerName, paperFormat, paperOrientation, copyCount):

        printerProps = {}
        printerProps["Name"] = printerName

        printProps = {}
        printProps["Wait"] = True

        if paperFormat != None:
            printerProps["PaperFormat"] = uno.getConstantByName("com.sun.star.view.PaperFormat." + paperFormat.upper())

        if paperOrientation != None:
            printerProps["PaperOrientation"] = uno.getConstantByName("com.sun.star.view.PaperOrientation." + paperOrientation.upper())

        if copyCount != None and copyCount > 0:
            printProps["CopyCount"] = copyCount

        self.document.Printer = self._toProperties(**printerProps)
        uno.invoke(self.document, "print", (self._toProperties(**printProps),))



    def _detectFamily(self, document):

        if document.supportsService("com.sun.star.text.GenericTextDocument"):
            return "Text"
        if document.supportsService("com.sun.star.sheet.SpreadsheetDocument"):
            return "Spreadsheet"
        if document.supportsService("com.sun.star.presentation.PresentationDocument"):
            return "Presentation"
        if document.supportsService("com.sun.star.drawing.DrawingDocument"):
            return "Drawing"

        raise DocumentException, "unknown document family: %s" % document

    def _filterProps(self, document, outputExt):

        family = self._detectFamily(document)
        filterByFamily = FILTER_MAP[family]

        try:
            filter = filterByFamily[outputExt]
        except KeyError:
            raise DocumentException, "unsupported conversion: to '%s'" % outputExt

        return filter

    def _toProperties(self, **args):

        props = []

        for key in args:
            prop = PropertyValue()
            prop.Name = key
            prop.Value = args[key]
            props.append(prop)

        return tuple(props)

    def _toFileUrl(self, path):

        return uno.systemPathToFileUrl( abspath( path ) )

    def _getFileExt(self, path):

        ext = splitext(path)[1]

        if ext is not None:
            return ext[1:].lower()



class InvalidUsageException(Exception):

    def __init__(self, usage):

        errmsg = "USAGE: convert <input-file> <output-file> [--port <port>] \n" + \
                 "       print   <file-name> <printer-name> [--paper-format <paper-format>] [--paper-orientation <paper-orientation>] [--copy-count <copy-count>] [--port <port>]"

        self.args = (errmsg)
        self.errmsg = errmsg

    def __str__(self):

        return self.errmsg

try:

    parser = OptionParser()

    parser.add_option("--port", dest="port", default=DEFAULT_OPENOFFICE_PORT)
    parser.add_option("--copy-count", dest="copyCount", type="int")
    parser.add_option("--paper-format", dest="paperFormat")
    parser.add_option("--paper-orientation", dest="paperOrientation")

    (opts, args) = parser.parse_args()

    if opts.port == "":
       opts.port = None

    if opts.paperFormat == "":
       opts.paperFormat = None

    if opts.paperOrientation == "":
       opts.paperOrientation = None

    if opts.copyCount <= 0:
       opts.copyCount = None



    if len(args) < 1:
        raise InvalidUsageException()

    if args[0] == "convert":

        if len(args) < 3:
            raise InvalidUsageException()

        if not isfile(args[1]):
            raise IOError("no such input file: %s" % args[1])

        try:
            document = Document(args[1], opts.port)
            document.convert(args[2])
        finally:
            del document

    elif args[0] == "print":

        if len(args) < 3:
            raise InvalidUsageException()

        if not isfile(args[1]):
            raise IOError("no such input file: %s" % args[1])

        try:
            document = Document(args[1], opts.port)    
            document.printout(args[2], opts.paperFormat, opts.paperOrientation, opts.copyCount)
        finally:        
            del document

    else:

        raise InvalidUsageException()

except InvalidUsageException as e:

    print e
    exit(255)

except Exception as e:

    print e
    exit(1)
