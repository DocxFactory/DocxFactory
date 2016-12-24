
/**
 * replace_filter.p -
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

    open query qCustomer

        for each Customer
            no-lock.



    /* 1. Every file starts out as a template */

    run xlsx_copyTemplate(

        input "stXlsx",                                     /* Stream name */
        input "samples/templates/replace_filter.xlsx" ).    /* Template file */



    /* The field columns location and ranges are defined in Excel, using named ranges. 
       You can create an Excel named range quickly by typing in the Excel name box (to the left of the formula bar). 

        1. Select the cell(s) to be named 
        2. Click in the Excel name box (to the left of the formula bar)
        3. Type a one-word name for the list, for example: ItemNum
        4. Press the Enter key.
       
       Another way to create and also edit and delete Excel named ranges is to use the Excel define name dialog box.
       In the Excel menu bar - Insert > Name > Define. */
    


    /* 2. Replace template data with temp-table data */

    run xlsx_replaceLongRange(

        input "stXlsx",                                     /* Stream name */
        input query qCustomer:handle,                       /* Dataset or temp-table handle or query handle or buffer handle */
        input "CustName = Customer.Name,"                   /* <named range> = <buffer field> mapping */
            + "Country  = Customer.Country,"
            + "Address  = Customer.Address,"
            + "Address2 = Customer.Address2,"
            + "City     = Customer.City,"
            + "State    = Customer.State",
        input "",                                           /* Buffer can-do list */
        input "" ).                                         /* Field  can-do list */

    /* 3. Save file */

    run xlsx_save(

        input "stXlsx",
        input os_getNextFile( session:temp-dir + "replace_filter_new.xlsx" ) ).

           /* os_getNextFile( ) adds a counter to the file name incase the file already exists */

{slib/err_catch cError cErrorMsg cStackTrace}:

    message
        cErrorMsg
        skip(1)
        cStackTrace
    view-as alert-box.

{slib/err_end}.
