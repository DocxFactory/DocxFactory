
/**
 * header_lines.p -
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



/* Catch and handle any thrown exceptions */

{slib/err_try}:

    /* Data sources */

    find first Item
         where Item.ItemNum = 1
         no-lock no-error.

    open query qOrderline

        for each  Orderline
            where Orderline.ItemNum = Item.ItemNum
            no-lock.



    /* Open template and generate Orderline template */

    run xlsx_createTemplate(

        input "stXlsx",
        input "samples/templates/header_lines.xlsx",
        input query qOrderline:handle,
        input "",
        input "" ).



    /* Header */

    run xlsx_cut(

        input "stXlsx",
        input "Header" ).

    run xlsx_setClipboardValue( "stXlsx", "Header", "Item.ItemNum",         Item.ItemNum ).
    run xlsx_setClipboardValue( "stXlsx", "Header", "Item.ItemName",        Item.ItemName ).
    run xlsx_setClipboardValue( "stXlsx", "Header", "Item.CatDescription",  Item.CatDescription ).
    run xlsx_setClipboardValue( "stXlsx", "Header", "Item.Price",           Item.Price ).
    run xlsx_setClipboardValue( "stXlsx", "Header", "Item.OnHand",          Item.OnHand ).

    run xlsx_paste(

        input "stXlsx",
        input "Header",
        input ? ).



    /* Lines */

    run xlsx_replaceLongRange(

        input "stXlsx",
        input query qOrderline:handle,
        input "",
        input "",
        input "" ).

    run xlsx_save(

        input "stXlsx",
        input os_getNextFile( session:temp-dir + "header_lines_new.xlsx" ) ).

{slib/err_catch cError cErrorMsg cStackTrace}:

    message
        cErrorMsg
        skip(1)
        cStackTrace
    view-as alert-box.

{slib/err_end}.

