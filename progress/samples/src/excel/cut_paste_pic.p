
/**
 * cut_paste_pic.p
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

define var iRow         as int no-undo.



/* Catch and handle any thrown exceptions */

{slib/err_try}:

    /* 1. Every file starts out as a template */

    run xlsx_copyTemplate(

        input "stXlsx",                                 /* Stream name */
        input "samples/templates/cut_paste_pic.xlsx" ). /* Template file */



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

        input "stXlsx",                                 /* Stream name */
        input "Item1" ).                                /* Clipboard item name */

    run xlsx_cut( "stXlsx", "Item2" ).

    /* 3. Optionally force clipboard item fields datatypes, if not the utility will try to guess the datatype based on the cell formats. */

    /***
    run xlsx_setClipboardDataType(

        input "stXlsx",                                 /* Stream name */
        input "Item1",                                  /* Clipboard item name */
        input "CatDescription1",                        /* Field name */
        input "text" ).                                 /* Text, date or number datatype (defaults to text if not set) */

    run xlsx_setClipboardDataType( "stXlsx", "Item1", "ItemPrice1",         "number" ).
    run xlsx_setClipboardDataType( "stXlsx", "Item2", "CatDescription2",    "text" ).
    run xlsx_setClipboardDataType( "stXlsx", "Item2", "ItemPrice2",         "number" ).
    ***/



    iRow = 0.

    for each item no-lock:

        iRow = iRow + 1.

        if iRow mod 2 = 1 then do:

            /* 4. Change clipboard item pictures and field values */

            run xlsx_setClipboardValue(

                input "stXlsx",                 /* Stream name */
                input "Item1",                  /* Clipboard item name */
                input "ItemPic1",               /* Picture name */
                input "samples/jpgs/cat" + string( item.itemnum, "99999" ) + ".jpg" ).

                    /* Picture filename (can also be a relative path filename) */

            run xlsx_setClipboardValue(

                input "stXlsx",                 /* Stream name */
                input "Item1",                  /* Clipboard item name */
                input "CatDescription1",        /* Field name */
                input item.catdescription ).    /* Field value */

            run xlsx_setClipboardValue( "stXlsx", "Item1", "ItemPrice1", item.price ).

            /* 5. Paste clipboard item with the set pictures and values */

            run xlsx_paste(

                input "stXlsx",                 /* Stream name */
                input "Item1",                  /* Clipboard item name */
                input ? ).                      /* Cursor cell reference. passing ? defaults to the current cursor position. */

        end. /* mod 2 = 1 */

        else
        if iRow mod 2 = 0 then do:

            run xlsx_setClipboardValue( "stXlsx", "Item2", "ItemPic2",          "samples/jpgs/cat" + string( item.itemnum, "99999" ) + ".jpg" ).
            run xlsx_setClipboardValue( "stXlsx", "Item2", "CatDescription2",   item.catdescription ).
            run xlsx_setClipboardValue( "stXlsx", "Item2", "ItemPrice2",        item.price ).

            run xlsx_paste( "stXlsx", "Item2", ? ).

        end. /* else */

    end. /* each item */



    /* 6. Save file */

    run xlsx_save( 

        input "stXlsx",
        input os_getNextFile( session:temp-dir + "cut_paste_pic_new.xlsx" ) ).

           /* os_getNextFile( ) adds a counter to the file name incase the file already exists */

{slib/err_catch cError cErrorMsg cStackTrace}:

    message
        cErrorMsg
        skip(1)
        cStackTrace
    view-as alert-box.

{slib/err_end}.
