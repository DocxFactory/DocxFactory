
/**
 * create_template.p -
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

    /* Data source */

    open query qQuery

        for each Item /* Try other tables. Remember to delete the create_template.xlsx if you change the table. */
            no-lock.



    /* xlsx_createTemplate( ) instead of xlsx_copyTemplate( ), creates a new template instead of using an existing one */

    run xlsx_createTemplate(

        input "stXlsx",                         /* Stream */
        input "samples/templates/blank.xlsx",   /* Blank file */
        input query qQuery:handle,              /* Dataset handle or temp-table handle or query handle or buffer handle */
        input "",                               /* Buffer can-do list */
        input "" ).                             /* Field  can-do list */

    run xlsx_replaceLongRange(

        input "stXlsx",
        input query qQuery:handle,
        input "",
        input "",
        input "" ).

    run xlsx_save(

        input "stXlsx",
        input os_getNextFile( session:temp-dir + "create_template_new.xlsx" ) ).

{slib/err_catch cError cErrorMsg cStackTrace}:

    message
        cErrorMsg
        skip(1)
        cStackTrace
    view-as alert-box.

{slib/err_end}.
