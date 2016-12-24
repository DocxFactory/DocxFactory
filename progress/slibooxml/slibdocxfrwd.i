
/**
 * slibdocxfrwd.i -
 *
 * (c) Copyright DocxFactory Ltd.
 *
 * docxfactory.com
 * alonb@docxfactory.com
 */



function docx_getFields             returns char {1}.
function docx_getItems              returns char {1}.
function docx_getItemParent         returns char ( pcItemName as char ) {1}.
function docx_getItemFields         returns char ( pcItemName as char ) {1}.

function docx_getUpdateTocMethod    returns int {1}.

function docx_getCodePage           returns char {1}.
function docx_getNumFracSep         returns char {1}.
function docx_getNumThSep           returns char {1}.
function docx_getDateFormat         returns char {1}.
function docx_getYearOffset         returns int {1}.
function docx_getFirstWeekDay       returns int {1}.
function docx_getWeekDayFullNames   returns char {1}.
function docx_getWeekDayShortNames  returns char {1}.
function docx_getMonthFullNames     returns char {1}.
function docx_getMonthShortNames    returns char {1}.

function docx_getTempDir            returns char {1}.
function docx_getWorkDir            returns char {1}.

function docx_getLastErrorDesc      returns char {1}.
function docx_getLastErrorFlag      returns log {1}.

