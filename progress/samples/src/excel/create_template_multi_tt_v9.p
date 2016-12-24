
/**
 * create_template_multi_tt_v9.p -
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



define temp-table ttMeta no-undo

    like xlsx_ttMeta.

define var cTableList   as char no-undo.
define var hTable       as handle no-undo.
define var hBuffer      as handle no-undo.

define var cError       as char no-undo.
define var cErrorMsg    as char no-undo.
define var cStackTrace  as char no-undo.



/**
 * The sample dynamically create and fills a list of temp-tables. The temp-tables handles are 
 * concatenated into a string and passed to the xlsx_createTemplate( ) and xlsx_replaceLongRange( ) 
 * similar to a prodataset with many temp-tables in OpenEdge 10.
 *
 * A ttMeta temp-table for additional formatting is also defined and filled and added to the string
 * list of tables.
 *
 * This sample passes a list of temp-tables but a concatenated string list of buffers, queries or even 
 * prodatasets in OpenEdge 10 can also be passed as a datasource.
 */

{slib/err_try}:

    create widget-pool.

    cTableList = "".

    empty temp-table ttMeta.

    for each  item
        where item.itemnum >= 1
          and item.itemnum <= 10
        no-lock:
    
        create temp-table hTable.

        hTable:create-like( buffer orderline:handle ).
        hTable:temp-table-prepare( "item" + string( item.itemnum, "99" ) ).

        cTableList = cTableList

            + ( if cTableList <> "" then "," else "" )
            + string( hTable ).



        hBuffer = hTable:default-buffer-handle.

        for each  orderline
            where orderline.item = item.itemnum
            no-lock:

            hBuffer:buffer-create( ).
            hBuffer:buffer-copy( buffer orderline:handle ).

        end. /* each orderline */



        create ttMeta.
        assign
            ttMeta.cObject  = "SheetName"
            ttMeta.cName    = hBuffer:name
            ttMeta.cParam   = "Item #" + string( item.itemnum, "99" ).

        create ttMeta.
        assign
            ttMeta.cObject  = "Sum"
            ttMeta.cName    = hBuffer:name + ".ExtendedPrice".

    end. /* each item */

    cTableList = cTableList

        + ( if cTableList <> "" then "," else "" )
        + string( temp-table ttMeta:handle ).



    run xlsx_createTemplate(

        input "stXlsx",
        input "samples/templates/blank.xlsx",
        input cTableList,
        input "",
        input "" ).

    run xlsx_replaceLongRange(

        input "stXlsx",
        input cTableList,
        input "",
        input "",
        input "" ).

    run xlsx_save(

        input "stXlsx",
        input os_getNextFile( session:temp-dir + "create_template_multi_tt_v9.xlsx" ) ).

{slib/err_catch cError cErrorMsg cStackTrace}:

    message
        cErrorMsg
        skip(1)
        cStackTrace
    view-as alert-box.

{slib/err_end}.
