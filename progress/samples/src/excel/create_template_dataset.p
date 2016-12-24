
/**
 * create_template_dataset.p -
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

define dataset dsOrder

    for ttCustomer, ttOrder, ttItem

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
        input os_getNextFile( session:temp-dir + "create_template_dataset_new.xlsx" ) ).

{slib/err_catch cError cErrorMsg cStackTrace}:

    message
        cErrorMsg
        skip(1)
        cStackTrace
    view-as alert-box.

{slib/err_end}.
