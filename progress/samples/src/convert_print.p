
/**
 * convert_print.p
 *
 * (c) Copyright ABC Alon Blich Consulting Tech Ltd.
 *
 * Contact Information
 * Email: alonblich@gmail.com
 * Phone: +972-54-218-8086
 */

{slibooxml/slibooxml.i}

{slib/slibos.i}

{slib/sliberr.i}



define temp-table ttOption no-undo

    like ooxml_ttOption.

define var cError       as char no-undo.
define var cErrorMsg    as char no-undo.
define var cStackTrace  as char no-undo.



/* Catch and handle any thrown exceptions */

{slib/err_try}:

    /* Conversion sample */

    /* Word */

    run ooxml_convert(

        input "samples/templates/sale_order.docx",                      /* Source file */
        input os_getNextFile( session:temp-dir + "sale_order.pdf" ),    /* Target file */
        input ? ).                                                      /* Optional options temp-table handle */

    /*
    run ooxml_convert( "samples/templates/sale_order.docx",     os_getNextFile( session:temp-dir + "sale_order.odt" ), ? ).
    run ooxml_convert( "samples/templates/sale_order.docx",     os_getNextFile( session:temp-dir + "sale_order.doc" ), ? ).
    run ooxml_convert( "samples/templates/sale_order.docx",     os_getNextFile( session:temp-dir + "sale_order.html" ), ? ).
    run ooxml_convert( "samples/templates/sale_order.docx",     os_getNextFile( session:temp-dir + "sale_order.txt" ), ? ).
    */

    /* Excel */

    /*
    run ooxml_convert( "samples/templates/replace_pivot.xlsx",  os_getNextFile( session:temp-dir + "replace_pivot.pdf" ), ? ).
    run ooxml_convert( "samples/templates/replace_pivot.xlsx",  os_getNextFile( session:temp-dir + "replace_pivot.ods" ), ? ).
    run ooxml_convert( "samples/templates/replace_pivot.xlsx",  os_getNextFile( session:temp-dir + "replace_pivot.xls" ), ? ).
    run ooxml_convert( "samples/templates/replace_pivot.xlsx",  os_getNextFile( session:temp-dir + "replace_pivot.html" ), ? ).
    run ooxml_convert( "samples/templates/replace_pivot.xlsx",  os_getNextFile( session:temp-dir + "replace_pivot.csv" ), ? ).
    */



    /* Printing sample */

    /*
    run ooxml_print(
        input "samples/templates/sale_order.docx",  /* File name */
        input "Bullzip PDF Printer",                /* Printer */
        input ?,                                    /* Optional number of copies */
        input ? ).                                  /* Optional options temp-table */
    */



    /* Text sample */

    /*
    create ttOption.
    assign
        ttOption.cOption    = "PaperOrientation"
        ttOption.cValue     = "Landscape". /* Landscape */

    run ooxml_convert(
        input "samples/templates/text.txt",
        input os_getNextFile( session:temp-dir + "text.docx" ),
        input temp-table ttOption:handle ).

    run ooxml_print(
        input "samples/templates/text.txt",
        input "Bullzip PDF Printer",
        input ?,
        input temp-table ttOption:handle ).
    */

{slib/err_catch cError cErrorMsg cStackTrace}:

    message
        cErrorMsg
        skip(1)
        cStackTrace
    view-as alert-box.

{slib/err_end}.
