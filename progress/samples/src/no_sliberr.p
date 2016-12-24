
/**
 * no_sliberr.p -
 *
 * (c) Copyright ABC Alon Blich Consulting Tech Ltd.
 *
 * Contact Information
 * Email: alonblich@gmail.com
 * Phone: +972-54-218-8086
 */

{slibooxml/slibdocx.i}



do on error undo, leave:

    run docx_copyTemplate(
        input "stXlsx",
        input "error error error" ).

end. /* do on error */

message "Error:" skip return-value view-as alert-box.
