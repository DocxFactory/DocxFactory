
/**
 * cut_paste_basic.p
 *
 * (c) Copyright ABC Alon Blich Consulting Tech Ltd.
 *
 * Contact Information
 * Email: alonblich@gmail.com
 * Phone: +972-54-218-8086
 */

{slibooxml/slibxlsx.i}

{slib/slibos.i}

{slib/sliberr.i}

define var cError       as char no-undo.
define var cErrorMsg    as char no-undo.
define var cStackTrace  as char no-undo.

define var iPage        as int no-undo.
define var iRow         as int no-undo.



/* Catch and handle any thrown exceptions */

{slib/err_try}:

    /* 1. Every file starts out as a template */

    run xlsx_copyTemplate(

        input "stXlsx",                                     /* Stream name */
        input "samples/templates/cut_paste_basic.xlsx" ).   /* Template file */



    /* The field columns location and ranges are defined in Excel, using named ranges. 
       You can create an Excel named range quickly by typing in the Excel name box (to the left of the formula bar). 

        1. Select the cell(s) to be named 
        2. Click in the Excel name box (to the left of the formula bar)
        3. Type a one-word name for the list, for example: ItemNum
        4. Press the Enter key.
       
       Another way to create and also edit and delete Excel named ranges is to use the Excel define name dialog box.
       In the Excel menu bar - Insert > Name > Define. */



    /* 2. Cut range into clipboard (you may work with as many clipboard items as you like) */

    run xlsx_cut(

        input "stXlsx",                                     /* Stream name */
        input "Page" ).                                     /* Clipboard item name */

    /* 3. Optionally force clipboard item fields datatypes, if not the utility will try to guess the datatype based on the cell formats. */

    /***
    run xlsx_setClipboardDataType(

        input "stXlsx",                                     /* Stream name */
        input "Page",                                       /* Clipboard item name */
        input "Title",                                      /* Field name */
        input "text" ).                                     /* Text, date or number datatype (defaults to text if not set) */

    run xlsx_setClipboardDataType( "stXlsx", "Page", "Date_*",      "date" ).
    run xlsx_setClipboardDataType( "stXlsx", "Page", "Number_*",    "number" ).
    ***/

    do iPage = 1 to 3:

        /* 4. Change clipboard item field values */

        run xlsx_setClipboardValue(

            input "stXlsx",                                 /* Stream name */
            input "Page",                                   /* Clipboard item name */
            input "Title",                                  /* Field name */
            input "Hello World" ).                          /* Field value */

        do iRow = 1 to 5:

            run xlsx_setClipboardValue( "stXlsx", "Page", "Date_"   + string( iRow ), today + random( 1, 100 ) ).
            run xlsx_setClipboardValue( "stXlsx", "Page", "Number_" + string( iRow ), random( 1, 1000 ) ).

        end. /* do iRow */

        /* 5. Paste clipboard item with the set values */

        run xlsx_paste(

            input "stXlsx",                                 /* Stream name */
            input "Page",                                   /* Clipboard item name */
            input ? ).                                      /* Cursor cell reference. passing ? defaults to the current cursor position. */

        run xlsx_insertPageBreak(

            input "stXlsx",                                 /* Stream name */
            input 1,                                        /* Sheet name or number */
            input ? ).                                      /* Row number. passing ? defaults to the current cursor position. */

    end. /* do iPage */

    /* 6. Save file */

    run xlsx_save( 

        input "stXlsx",
        input os_getNextFile( session:temp-dir + "cut_paste_basic_new.xlsx" ) ).

           /* os_getNextFile( ) adds a counter to the file name incase the file already exists */

{slib/err_catch cError cErrorMsg cStackTrace}:

    message
        cErrorMsg
        skip(1)
        cStackTrace
    view-as alert-box.

{slib/err_end}.
