
/**
 * slibxlsxprop.i -
 *
 * (c) Copyright ABC Alon Blich Consulting Tech Ltd.
 *
 * Contact Information
 * Email: alonblich@gmail.com
 * Phone: +972-54-218-8086
 */



define temp-table xlsx_ttMeta no-undo

    field cObject   as char
    field cName     as char
    field cParam    as char.



&global xlsx_xRowLimit                  1048576
&global xlsx_xColLimit                  16384
&global xlsx_xPageBreakLimit            1024

&global xlsx_xRowDefaultHeight          14.4
&global xlsx_xColDefaultWidth           9.109375
