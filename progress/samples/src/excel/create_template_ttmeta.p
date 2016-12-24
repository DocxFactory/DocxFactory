
/**
 * create_template_ttmeta.p -
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



define temp-table ttCustomer no-undo

    field CustNum           like Customer.CustNum
    field Name              like Customer.Name
    field Country           like Customer.Country
    field City              like Customer.City
    field State             like Customer.State
    field Address           like Customer.Address

    index CustNum is primary unique
          CustNum.

define temp-table ttOrder no-undo

    field CustNum           like Order.CustNum
    field OrderNum          like Order.OrderNum
    field OrderDate         like Order.OrderDate
    field Carrier           like Order.Carrier
    field tDatetime         as datetime

    index CustOrder is primary unique
          CustNum
          OrderNum.

define temp-table ttItem no-undo

    field ItemNum           like Item.ItemNum
    field ItemName          like Item.ItemName
    field CatDescription    like Item.CatDescription
    field Price             like Item.Price
    field OnHand            like Item.OnHand

    index ItemNum is primary unique
          ItemNum.

define temp-table ttMeta no-undo

    like xlsx_ttMeta.

define dataset dsOrder

    for ttCustomer, ttOrder, ttItem, ttMeta

    data-relation Order for ttCustomer, ttOrder

        relation-fields ( CustNum, CustNum ).

define var hDataSet     as handle no-undo.

define var cError       as char no-undo.
define var cErrorMsg    as char no-undo.
define var cStackTrace  as char no-undo.



/* Catch and handle any thrown exceptions */

{slib/err_try}:

    /* Data source */

    for each  Customer
        where Customer.CustNum >= 1
          and Customer.CustNum <= 50
        no-lock:

        create ttCustomer.
        buffer-copy Customer to ttCustomer.

        for each Order of Customer
            no-lock:

            create ttOrder.
            buffer-copy Order to ttOrder
                assign
                    ttOrder.tDatetime = now.

        end. /* each Order */

    end. /* each Customer */

    for each Item
        no-lock:

        create ttItem.
        buffer-copy Item to ttItem.

    end. /* each Item */

    hDataSet = dataset dsOrder:handle.



    /* ttMeta */

    create ttMeta.
    assign
        ttMeta.cObject  = "SheetName"
        ttMeta.cName    = "ttItem|ttCustomer"
        ttMeta.cParam   = "Item List|Customer List".

    /* Calculated fields -

       You can enter Excel formulas using Excel syntax and functions.
       Use 4GL buffer-field or calculated field names in your formulas (that the util converts to cell references).

       The order calculated fields are entered does not matter (the util will create them based on their dependencies)
       or whether they're created in a single or multiple "Calc" meta records. */

    create ttMeta.
    assign
        ttMeta.cObject  = "Calc"
        ttMeta.cName    = "ExtPrice"
        ttMeta.cParam   = "ttItem.Price * ttItem.OnHand".

    /* Multi-line calculated fields -

       You can enter one formula for the first line and a different one for the rest of the file by using the ; separator.

       You can reference a column previous row by adding :PrevRow option to the field name.
       You can also use the following other options :NextRow,:PrevCol,:NextCol. */

    create ttMeta.
    assign
        ttMeta.cObject  = "Calc"
        ttMeta.cName    = "AccumPrice"
        ttMeta.cParam   = "ExtPrice;AccumPrice:PrevRow + ExtPrice".



    create ttMeta.
    assign
        ttMeta.cObject  = "Label"
        ttMeta.cName    = "ttItem.ItemNum|ttItem.ItemName|ExtPrice|AccumPrice"
        ttMeta.cParam   = "Item!Number|Item!Name|Extended!Price|Accumulated!Price".

    create ttMeta.
    assign
        ttMeta.cObject  = "Format"
        ttMeta.cName    = "ttItem.OnHand|ExtPrice|AccumPrice"
        ttMeta.cParam   = "#,##0;[Red]-#,##0|#,##0.00|#,##0.00". /* Excel format */

    create ttMeta.
    assign
        ttMeta.cObject  = "Width"
        ttMeta.cName    = "ttItem.Price|ttItem.OnHand|ExtPrice|AccumPrice"
        ttMeta.cParam   = "13|13|13|13".

    create ttMeta.
    assign
        ttMeta.cObject  = "Align"
        ttMeta.cName    = "ExtPrice|AccumPrice"
        ttMeta.cParam   = "right|right".

    create ttMeta.
    assign
        ttMeta.cObject  = "WrapText"
        ttMeta.cName    = "ttItem.CatDescription"
        ttMeta.cParam   = "yes".

    create ttMeta.
    assign
        ttMeta.cObject  = "Sum"
        ttMeta.cName    = "ttItem.OnHand|ExtPrice".

    create ttMeta.
    assign
        ttMeta.cObject  = "Average"
        ttMeta.cName    = "ttItem.Price".

    create ttMeta.
    assign
        ttMeta.cObject  = "Count"
        ttMeta.cName    = "ItemNum".



    /* xlsx_createTemplate( ) instead of xlsx_copyTemplate( ), creates a new template instead of using an existing one */

    run xlsx_createTemplate(

        input "stXlsx",                         /* Stream */
        input "samples/templates/blank.xlsx",   /* Blank file */
        input hDataSet,                         /* Dataset handle or temp-table handle or query handle or buffer handle */
        input "",                               /* Buffer can-do list */
        input "" ).                             /* Field  can-do list */

    run xlsx_replaceLongRange(

        input "stXlsx",
        input hDataSet,
        input "",
        input "",
        input "" ).

    run xlsx_save(

        input "stXlsx",
        input os_getNextFile( session:temp-dir + "create_template_ttmeta_new.xlsx" ) ).

{slib/err_catch cError cErrorMsg cStackTrace}:

    message
        cErrorMsg
        skip(1)
        cStackTrace
    view-as alert-box.

{slib/err_end}.
