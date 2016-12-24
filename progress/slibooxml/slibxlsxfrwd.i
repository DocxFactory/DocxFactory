
/**
 * slibxlsxfrwd.i -
 *
 * (c) Copyright ABC Alon Blich Consulting Tech Ltd.
 *
 * Contact Information
 * Email: alonblich@gmail.com
 * Phone: +972-54-218-8086
 */



function xlsx_getNumSheets          returns int     ( pcStream as char ) {1}.
function xlsx_getSheetName          returns char    ( pcStream as char, piSheetNum as int ) {1}.

function xlsx_getCellValue          returns char    ( pcStream as char, pcSheet as char, pcRef as char ) {1}.

function xlsx_getDefinedNameList    returns char    ( pcStream as char ) {1}.
function xlsx_getDefinedName        returns char    ( pcStream as char, pcDefinedName as char ) {1}.

function xlsx_getRange              returns char    ( pcSheetName as char, piFromRow as int, plFromRowAbs as log, piFromCol as int, plFromColAbs as log, piToRow as int, plToRowAbs as log, piToCol as int, plToColAbs as log ) {1}.
function xlsx_getRef                returns char    ( piRow as int, plRowAbs as log, piCol as int, plColAbs as log ) {1}.
function xlsx_getColRef             returns char    ( piCol as int ) {1}.

function xlsx_getCol                returns int     ( pcCol as char ) {1}.
function xlsx_getRefRow             returns int     ( pcRef as char ) {1}.
function xlsx_getRefCol             returns int     ( pcRef as char ) {1}.
