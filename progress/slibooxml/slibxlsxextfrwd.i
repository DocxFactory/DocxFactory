
/**
 * slibxlsxextfrwd.i -
 *
 * (c) Copyright ABC Alon Blich Consulting Tech Ltd.
 *
 * Contact Information
 * Email: alonblich@gmail.com
 * Phone: +972-54-218-8086
 */

 

procedure xlsx_replaceLongRange:

    define input param pcStream         as char no-undo.
    define input param pcDataSource     as char no-undo.
    define input param pcFieldMapping   as char no-undo.
    define input param pcBufferCanDo    as char no-undo.
    define input param pcFieldCanDo     as char no-undo.

    run super(
        input pcStream,
        input pcDataSource,
        input pcFieldMapping,
        input pcBufferCanDo,
        input pcFieldCanDo ).

end procedure. /* xlsx_replaceLongRange */



procedure xlsx_cut:

    define input param pcStream     as char no-undo.
    define input param pcItemName   as char no-undo.

    run super(
        input pcStream,
        input pcItemName ).

end procedure. /* xlsx_cut */

procedure xlsx_paste:

    define input param pcStream     as char no-undo.
    define input param pcItemName   as char no-undo.
    define input param pcRef        as char no-undo.

    run super(
        input pcStream,
        input pcItemName,
        input pcRef ).

end procedure. /* xlsx_paste */

procedure xlsx_setClipboardDataType:

    define input param pcStream     as char no-undo.
    define input param pcItemName   as char no-undo.
    define input param pcFieldName  as char no-undo.
    define input param pcDataType   as char no-undo.

    run super(
        input pcStream,
        input pcItemName,
        input pcFieldName,
        input pcDataType ).

end procedure. /* xlsx_setClipboardDataType */

procedure xlsx_setClipboardValue:

    define input param pcStream     as char no-undo.
    define input param pcItemName   as char no-undo.
    define input param pcFieldName  as char no-undo.
    define input param pcValue      as char no-undo.

    run super(
        input pcStream,
        input pcItemName,
        input pcFieldName,
        input pcValue ).

end procedure. /* xlsx_setClipboardValue */

procedure xlsx_setClipboardPic:

    define input param pcStream     as char no-undo.
    define input param pcItemName   as char no-undo.
    define input param pcPicName    as char no-undo.
    define input param pcFileName   as char no-undo.

    run super(
        input pcStream,
        input pcItemName,
        input pcPicName,
        input pcFileName ).

end procedure. /* xlsx_setClipboardValue */

procedure xlsx_insertPageBreak:

    define input param pcStream as char no-undo.
    define input param pcSheet  as char no-undo.
    define input param piRow    as int no-undo.

    run super(
        input pcStream,
        input pcSheet,
        input piRow ).

end procedure. /* xlsx_insertPageBreak */
