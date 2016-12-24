
/**
 * slibdocx.p -
 *
 * (c) Copyright DocxFactory Ltd.
 *
 * docxfactory.com
 * alonb@docxfactory.com
 */

{slibooxml/slibdocxfrwd.i "forward"}

{slibooxml/slibdocxprop.i}

{slib/slibos.i}

{slib/slibpro.i}

function getErrorFlagFromDfw    returns log forward:
function getErrorFlagFromDfwc   returns log forward:

define var gcErrorWhat  as char no-undo.
define var glErrorFlag  as log no-undo.



on "close" of this-procedure do:

    delete procedure this-procedure.

end. /* on close */

procedure initializeProc:
    
    run docx_setTempDir     ( session:temp-dir ) no-error.
    run docx_setDateFormat  ( session:date-format ).
    run docx_setYearOffset  ( session:year-offset ).
    run docx_setNumFracSep  ( session:numeric-decimal-point ) .
    run docx_setNumThSep    ( session:numeric-separator ).
    run docx_setCodePage    ( session:cpinternal ) no-error.
    
    if error-status:error then
        run docx_setCodePage( "" ) no-error.

    run resetErrors.

end procedure. /* initializeProc */



procedure docx_compile:

    define input param pcSrcFileName    as char no-undo.
    define input param pcDstFileName    as char no-undo.

    run resetErrors.

    run dfwc_compile(
        pcSrcFileName,
        pcDstFileName ).

    if getErrorFlagFromDfwc() then do:

        run setErrorsFromDfwc.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_compile */

procedure docx_copyTemplate: /* added for backwards compatibility */

    define input param pcFileName as char no-undo.

    define var cDir     as char no-undo.
    define var cFile    as char no-undo.
    define var cExt     as char no-undo.
    
    define var tDate    as date no-undo.
    define var iTime    as int no-undo.

    pcFileName = os_normalizePath( pcFileName ).

    if os_isRelativePath( pcFileName ) then
        pcFileName = os_normalizePath( pro_cWorkDir + "/" + pcFileName ).

    run os_breakPath(
        input   pcFileName,
        output  cDir,
        output  cFile,
        output  cExt ).
        
    if cExt = ".docx" then do:
    
        file-info:file-name = cDir + cFile + ".dfw".
        if file-info:full-pathname = ? then do:
        
            run docx_compile(
                pcFileName,
                input cDir + cFile + ".dfw" ) no-error.

            if error-status:error then
                return error return-value.

        end. /* full-pathname = ? */

        else do:
        
            assign
                tDate = file-info:file-mod-date.
                iTime = file-info:file-mod-time.
                
            file-info:file-name = pcFileName.
            if file-info:full-pathname = ? then

                return error "File not found " + pcFileName.

            if  file-info:file-mod-date > tDate
             or file-info:file-mod-date = tDate
            and file-info:file-mod-time > iTime then do:
            
                run docx_compile(
                    pcFileName,
                    input cDir + cFile + ".dfw" ) no-error.

                if error-status:error then
                    return error return-value.

            end. /* file-mod-date > tDate */

        end. /* else */
    
        pcFileName = cDir + cFile + ".dfw".

    end. /* cExt = ".docx" */



    run docx_load( pcFileName ) no-error.
    
    if error-status:error then
        return error return-value.

end procedure. /* docx_copyTemplate */

procedure docx_load:

    define input param pcFileName as char no-undo.

    run resetErrors.

    run dfw_load( pcFileName ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_load */

procedure docx_save:

    define input param pcFileName as char no-undo.

    run resetErrors.

    run dfw_save( pcFileName ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_save */

procedure docx_print:

    define input param pcPrinterName    as char no-undo.
    define input param piCopyCnt        as int no-undo.

    run resetErrors.

    run dfw_print(
        pcPrinterName,
        piCopyCnt ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_print */

procedure docx_close:

    run resetErrors.

    run dfw_close.

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_close */

procedure docx_merge:

    define input param pcData as char no-undo.

    if pcData = ? then
	    return.

    run resetErrors.

    run dfw_merge( pcData ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_merge */

procedure docx_setChartValue:

    define input param pcItemName    as char no-undo.
    define input param pcFieldName   as char no-undo.
    define input param pcSeries      as char no-undo.
    define input param pcCategory    as char no-undo.
    define input param pdValue       as dec no-undo.

    if pcSeries = ? then
       pcSeries = "".

    if pcCategory = ? then
       pcCategory = "".

    if pdValue = ? then
        return.

    run resetErrors.

    run dfw_setChartValueByStrStr(
        pcItemName,
        pcFieldName,
        pcSeries,
        pcCategory,
        pdValue ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_setChartValue */

procedure docx_setClipboardValue:

    define input param pcItemName    as char no-undo.
    define input param pcFieldName   as char no-undo.
    define input param pcValue       as char no-undo.

    if pcValue = ? then
	    return.

    run resetErrors.

    run dfw_setClipboardValueByStr(
        pcItemName,
        pcFieldName,
        pcValue ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_setClipboardValue */

procedure docx_setClipboardValueByStr:

    define input param pcItemName    as char no-undo.
    define input param pcFieldName   as char no-undo.
    define input param pcValue       as char no-undo.

    if pcValue = ? then
	    return.

    run resetErrors.

    run dfw_setClipboardValueByStr(
        pcItemName,
        pcFieldName,
        pcValue ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_setClipboardValueByStr */

procedure docx_setClipboardValueByDec:

    define input param pcItemName    as char no-undo.
    define input param pcFieldName   as char no-undo.
    define input param pdValue       as dec no-undo.

    if pdValue = ? then
	    return.

    run resetErrors.

    run dfw_setClipboardValueByDouble(
        pcItemName,
        pcFieldName,
        pdValue ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_setClipboardValueByDec */

procedure docx_paste:

    define input param pcItemName as char no-undo.

    run resetErrors.

    run dfw_paste( pcItemName ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_paste */

function docx_getFields returns char:

    define var retval   as memptr no-undo.
    define var str      as char no-undo.

    run resetErrors.

    run dfw_getFields( output retval ).
    str = get-string( retval, 1 ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error ?.

    end. /* getErrorFlagFromDfw() */

    return str.

end function. /* docx_getFields */

function docx_getItems returns char:

    define var retval   as memptr no-undo.
    define var str      as char no-undo.

    run resetErrors.

    run dfw_getItems( output retval ).
    str = get-string( retval, 1 ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error ?.

    end. /* getErrorFlagFromDfw() */

    return str.

end function. /* docx_getItems */

function docx_getItemParent returns char ( pcItemName as char ):

    define var retval   as memptr no-undo.
    define var str      as char no-undo.

    run resetErrors.

    run dfw_getItemParent( pcItemName, output retval ).
    str = get-string( retval, 1 ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error ?.

    end. /* getErrorFlagFromDfw() */

    return str.

end function. /* docx_getItemParent */

function docx_getItemFields returns char ( pcItemName as char ):

    define var retval   as memptr no-undo.
    define var str      as char no-undo.

    run resetErrors.

    run dfw_getItemFields( pcItemName, output retval ).
    str = get-string( retval, 1 ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error ?.

    end. /* getErrorFlagFromDfw() */

    return str.

end function. /* docx_getItemFields */



procedure docx_setUpdateTocMethod:

    define input param piMethod as int no-undo.

    if piMethod = ? then
	    return.

    run resetErrors.

    run dfw_setUpdateTocMethod( piMethod ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_setUpdateTocMethod */

function docx_getUpdateTocMethod returns int:

    define var retval as int no-undo.

    run resetErrors.

    run dfw_getUpdateTocMethod( output retval ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error ?.

    end. /* getErrorFlagFromDfw() */

    return retval.

end function. /* docx_getUpdateTocMethod */



procedure docx_setCodePage:

    define input param pcCodePage as char no-undo.

    case pcCodePage:
    
        when "undefined"    then pcCodePage = "utf-8".
        when "1250"         then pcCodePage = "windows-1250".
        when "1251"         then pcCodePage = "windows-1251".
        when "1252"         then pcCodePage = "windows-1252".
        when "1253"         then pcCodePage = "windows-1253".
        when "1254"         then pcCodePage = "windows-1254".
        when "1255"         then pcCodePage = "windows-1255".
        when "1256"         then pcCodePage = "windows-1256".
        when "1257"         then pcCodePage = "windows-1257".

    end case. /* pcCodePage */

    run resetErrors.

    run dfw_setCodePage( pcCodePage ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_setCodePage */

procedure docx_setNumFracSep:

    define input param pcCh as char no-undo.

    run resetErrors.

    run dfw_setNumFracSep( ( if pcCh = "" then 0 else asc( substr( pcCh, 1, 1 ) ) ) ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_setNumFracSep */

procedure docx_setNumThSep:

    define input param pcCh as char no-undo.

    run resetErrors.

    run dfw_setNumThSep( ( if pcCh = "" then 0 else asc( substr( pcCh, 1, 1 ) ) ) ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_setNumThSep */

procedure docx_setDateFormat:

    define input param pcDateFormat as char no-undo.

    run resetErrors.

    run dfw_setDateFormat( pcDateFormat ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_setDateFormat */

procedure docx_setYearOffset:

    define input param piYearOffset as int no-undo.

    run resetErrors.

    run dfw_setYearOffset( piYearOffset ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_setYearOffset */

procedure docx_setFirstWeekDay:

    define input param piWeekDay as int no-undo.

    run resetErrors.

    run dfw_setFirstWeekDay( piWeekDay ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_setFirstWeekDay */

procedure docx_setWeekDayNames0:

    run resetErrors.

    run dfw_setWeekDayNames0.

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_setWeekDayNames0 */

procedure docx_setWeekDayNames7:

    define input param pcWeekDay1   as char no-undo.
    define input param pcWeekDay2   as char no-undo.
    define input param pcWeekDay3   as char no-undo.
    define input param pcWeekDay4   as char no-undo.
    define input param pcWeekDay5   as char no-undo.
    define input param pcWeekDay6   as char no-undo.
    define input param pcWeekDay7   as char no-undo.

    run resetErrors.

    run dfw_setWeekDayNames7(
        pcWeekDay1,
        pcWeekDay2,
        pcWeekDay3,
        pcWeekDay4,
        pcWeekDay5,
        pcWeekDay6,
        pcWeekDay7 ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_setWeekDayNames7 */

procedure docx_setWeekDayNames14:

    define input param pcFullName1  as char no-undo.
    define input param pcFullName2  as char no-undo.
    define input param pcFullName3  as char no-undo.
    define input param pcFullName4  as char no-undo.
    define input param pcFullName5  as char no-undo.
    define input param pcFullName6  as char no-undo.
    define input param pcFullName7  as char no-undo.

    define input param pcShortName1 as char no-undo.
    define input param pcShortName2 as char no-undo.
    define input param pcShortName3 as char no-undo.
    define input param pcShortName4 as char no-undo.
    define input param pcShortName5 as char no-undo.
    define input param pcShortName6 as char no-undo.
    define input param pcShortName7 as char no-undo.

    run resetErrors.

    run dfw_setWeekDayNames14(
        pcFullName1,
        pcFullName2,
        pcFullName3,
        pcFullName4,
        pcFullName5,
        pcFullName6,
        pcFullName7,

        pcShortName1,
        pcShortName2,
        pcShortName3,
        pcShortName4,
        pcShortName5,
        pcShortName6,
        pcShortName7 ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_setWeekDayNames14 */

procedure docx_setMonthNames0:

    run resetErrors.

    run dfw_setMonthNames0.

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_setMonthNames0 */

procedure docx_setMonthNames12:

    define input param pcMonth1     as char no-undo.
    define input param pcMonth2     as char no-undo.
    define input param pcMonth3     as char no-undo.
    define input param pcMonth4     as char no-undo.
    define input param pcMonth5     as char no-undo.
    define input param pcMonth6     as char no-undo.
    define input param pcMonth7     as char no-undo.
    define input param pcMonth8     as char no-undo.
    define input param pcMonth9     as char no-undo.
    define input param pcMonth10    as char no-undo.
    define input param pcMonth11    as char no-undo.
    define input param pcMonth12    as char no-undo.

    run resetErrors.

    run dfw_setMonthNames12(
        pcMonth1,
        pcMonth2,
        pcMonth3,
        pcMonth4,
        pcMonth5,
        pcMonth6,
        pcMonth7,
        pcMonth8,
        pcMonth9,
        pcMonth10,
        pcMonth11,
        pcMonth12 ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_setMonthNames12 */

procedure docx_setMonthNames24:

    define input param pcFullName1      as char no-undo.
    define input param pcFullName2      as char no-undo.
    define input param pcFullName3      as char no-undo.
    define input param pcFullName4      as char no-undo.
    define input param pcFullName5      as char no-undo.
    define input param pcFullName6      as char no-undo.
    define input param pcFullName7      as char no-undo.
    define input param pcFullName8      as char no-undo.
    define input param pcFullName9      as char no-undo.
    define input param pcFullName10     as char no-undo.
    define input param pcFullName11     as char no-undo.
    define input param pcFullName12     as char no-undo.

    define input param pcShortName1     as char no-undo.
    define input param pcShortName2     as char no-undo.
    define input param pcShortName3     as char no-undo.
    define input param pcShortName4     as char no-undo.
    define input param pcShortName5     as char no-undo.
    define input param pcShortName6     as char no-undo.
    define input param pcShortName7     as char no-undo.
    define input param pcShortName8     as char no-undo.
    define input param pcShortName9     as char no-undo.
    define input param pcShortName10    as char no-undo.
    define input param pcShortName11    as char no-undo.
    define input param pcShortName12    as char no-undo.

    run resetErrors.

    run dfw_setMonthNames24(
        pcFullName1,
        pcFullName2,
        pcFullName3,
        pcFullName4,
        pcFullName5,
        pcFullName6,
        pcFullName7,
        pcFullName8,
        pcFullName9,
        pcFullName10,
        pcFullName11,
        pcFullName12,

        pcShortName1,
        pcShortName2,
        pcShortName3,
        pcShortName4,
        pcShortName5,
        pcShortName6,
        pcShortName7,
        pcShortName8,
        pcShortName9,
        pcShortName10,
        pcShortName11,
        pcShortName12 ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_setMonthNames24 */

function docx_getCodePage returns char:

    define var retval   as memptr no-undo.
    define var str      as char no-undo.

    run resetErrors.

    run dfw_getCodePage( output retval ).
    str = get-string( retval, 1 ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error ?.

    end. /* getErrorFlagFromDfw() */

    return str.

end function. /* docx_getCodePage */

function docx_getNumFracSep returns char:

    define var retval as int no-undo.

    run resetErrors.

    run dfw_getNumFracSep( output retval ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error ?.

    end. /* getErrorFlagFromDfw() */

    return chr( retval ).

end function. /* docx_getNumFracSep */

function docx_getNumThSep returns char:

    define var retval as int no-undo.

    run resetErrors.

    run dfw_getNumThSep( output retval ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error ?.

    end. /* getErrorFlagFromDfw() */

    return chr( retval ).

end function. /* docx_getNumThSep */

function docx_getDateFormat returns char:

    define var retval   as memptr no-undo.
    define var str      as char no-undo.

    run resetErrors.

    run dfw_getDateFormat( output retval ).
    str = get-string( retval, 1 ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error ?.

    end. /* getErrorFlagFromDfw() */

    return str.

end function. /* docx_getDateFormat */

function docx_getYearOffset returns int:

    define var retval as int no-undo.

    run resetErrors.

    run dfw_getYearOffset( output retval ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error ?.

    end. /* getErrorFlagFromDfw() */

    return retval.

end function. /* docx_getYearOffset */

function docx_getFirstWeekDay returns int:

    define var retval as int no-undo.

    run resetErrors.

    run dfw_getFirstWeekDay( output retval ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error ?.

    end. /* getErrorFlagFromDfw() */

    return retval.

end function. /* docx_getFirstWeekDay */

function docx_getWeekDayFullNames returns char:

    define var retval   as memptr no-undo.
    define var str      as char no-undo.

    run resetErrors.

    run dfw_getWeekDayFullNames( output retval ).
    str = get-string( retval, 1 ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error ?.

    end. /* getErrorFlagFromDfw() */

    return str.

end function. /* docx_getWeekDayFullNames */

function docx_getWeekDayShortNames returns char:

    define var retval   as memptr no-undo.
    define var str      as char no-undo.

    run resetErrors.

    run dfw_getWeekDayShortNames( output retval ).
    str = get-string( retval, 1 ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error ?.

    end. /* getErrorFlagFromDfw() */

    return str.

end function. /* docx_getWeekDayShortNames */

function docx_getMonthFullNames returns char:

    define var retval   as memptr no-undo.
    define var str      as char no-undo.

    run resetErrors.

    run dfw_getMonthFullNames( output retval ).
    str = get-string( retval, 1 ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error ?.

    end. /* getErrorFlagFromDfw() */

    return str.

end function. /* docx_getMonthFullNames */

function docx_getMonthShortNames returns char:

    define var retval   as memptr no-undo.
    define var str      as char no-undo.

    run resetErrors.

    run dfw_getMonthShortNames( output retval ).
    str = get-string( retval, 1 ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error ?.

    end. /* getErrorFlagFromDfw() */

    return str.

end function. /* docx_getMonthShortNames */



procedure docx_setTempDir:

    define input param pcDir as char no-undo.

    run resetErrors.

    run dfw_setTempDir( pcDir ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error gcErrorWhat.

    end. /* getErrorFlagFromDfw() */

end procedure. /* docx_setTempDir */

function docx_getTempDir returns char:

    define var retval   as memptr no-undo.
    define var str      as char no-undo.

    run resetErrors.

    run dfw_getTempDir( output retval ).
    str = get-string( retval, 1 ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error ?.

    end. /* getErrorFlagFromDfw() */

    return str.

end function. /* docx_getTempDir */

function docx_getWorkDir returns char:

    define var retval   as memptr no-undo.
    define var str      as char no-undo.

    run resetErrors.

    run dfw_getWorkDir( output retval ).
    str = get-string( retval, 1 ).

    if getErrorFlagFromDfw() then do:

        run setErrorsFromDfw.
        return error ?.

    end. /* getErrorFlagFromDfw() */

    return str.

end function. /* docx_getWorkDir */



function docx_getLastErrorDesc returns char:

    return gcErrorWhat.

end function. /* docx_getLastErrorDesc */

function docx_getLastErrorFlag returns log:

    return glErrorFlag.

end function. /* docx_getLastErrorFlag */

procedure resetErrors:

    assign
        gcErrorWhat = ""
        glErrorFlag = false.

end procedure. /* resetErrors */

procedure setErrorsFromDfw:

    define var retval as memptr no-undo.
    run dfw_getErrorWhat( output retval ).

    gcErrorWhat = get-string( retval, 1 ).
    glErrorFlag = true.

end procedure. /* setErrorFromDfw */

procedure setErrorsFromDfwc:

    define var retval as memptr no-undo.
    run dfwc_getErrorWhat( output retval ).

    gcErrorWhat = get-string( retval, 1 ).
    glErrorFlag = true.

end procedure. /* setErrorFromDfw */

function getErrorFlagFromDfw returns log:

    define var retval as int no-undo.

    run dfw_getErrorFlag( output retval ).
    return ( if retval = 1 then true else false ).

end function. /* getErrorFlagFromDfw() */

function getErrorFlagFromDfwc returns log:

    define var retval as int no-undo.

    run dfwc_getErrorFlag( output retval ).
    return ( if retval = 1 then true else false ).

end function. /* getErrorFlagFromDfw() */



procedure dfwc_compile external {&docx_xDocxFactoryLib} cdecl persistent:

    define input    param pcSrcFileName as char.
    define input    param pcDstFileName as char.

end procedure. /* dfw_compile */

procedure dfwc_getErrorWhat external {&docx_xDocxFactoryLib} cdecl persistent:

    define return param pcRetVal as memptr.

end procedure. /* dfw_getErrorDesc */

procedure dfwc_getErrorFlag external {&docx_xDocxFactoryLib} cdecl persistent:

    define return param piRetVal as byte.

end procedure. /* dfw_getErrorFlag */



procedure dfw_load external {&docx_xDocxFactoryLib} cdecl persistent:

    define input param pcFileName as char.

end procedure. /* dfw_load */

procedure dfw_save external {&docx_xDocxFactoryLib} cdecl persistent:

    define input param pcFileName as char.

end procedure. /* dfw_save */

procedure dfw_print external {&docx_xDocxFactoryLib} cdecl persistent:

    define input    param pcPrintName   as char.
    define input    param piCopyCnt     as unsigned-short.

end procedure. /* dfw_print */

procedure dfw_close external {&docx_xDocxFactoryLib} cdecl persistent:

end procedure. /* dfw_close */

procedure dfw_merge external {&docx_xDocxFactoryLib} cdecl persistent:

    define input param pcData as char.

end procedure. /* dfw_merge */

procedure dfw_setChartValueByStrStr external {&docx_xDocxFactoryLib} cdecl persistent:

    define input    param pcItemName    as char.
    define input    param pcFieldName   as char.
    define input    param pcSeries      as char.
    define input    param pcCategory    as char.
    define input    param pdValue       as double.

end procedure. /* dfw_setClipboardValueByStr */

procedure dfw_setClipboardValueByStr external {&docx_xDocxFactoryLib} cdecl persistent:

    define input    param pcItemName    as char.
    define input    param pcFieldName   as char.
    define input    param pcValue       as char.

end procedure. /* dfw_setClipboardValueByStr */

procedure dfw_setClipboardValueByDouble external {&docx_xDocxFactoryLib} cdecl persistent:

    define input    param pcItemName    as char.
    define input    param pcFieldName   as char.
    define input    param pdValue       as double.

end procedure. /* dfw_setClipboardValueByStr */

procedure dfw_paste external {&docx_xDocxFactoryLib} cdecl persistent:

    define input param pcItemName as char.

end procedure. /* dfw_paste */

procedure dfw_getFields external {&docx_xDocxFactoryLib} cdecl persistent:

    define return param pcRetVal as memptr.

end procedure. /* dfw_getFields */

procedure dfw_getItems external {&docx_xDocxFactoryLib} cdecl persistent:

    define return param pcRetVal as memptr.

end procedure. /* dfw_getItems */

procedure dfw_getItemParent external {&docx_xDocxFactoryLib} cdecl persistent:

    define input    param pcItemName    as char.
    define return   param pcRetVal      as memptr.

end procedure. /* dfw_getItemParent */

procedure dfw_getItemFields external {&docx_xDocxFactoryLib} cdecl persistent:

    define input    param pcItemName    as char.
    define return   param pcRetVal      as memptr.

end procedure. /* dfw_getItemFields */



procedure dfw_setUpdateTocMethod external {&docx_xDocxFactoryLib} cdecl persistent:

    define input param piMethod as byte.

end procedure. /* dfw_setUpdateTocMethod */

procedure dfw_getUpdateTocMethod external {&docx_xDocxFactoryLib} cdecl persistent:

    define return param piRetVal as byte.

end procedure. /* dfw_getUpdateTocMethod */



procedure dfw_setCodePage external {&docx_xDocxFactoryLib} cdecl persistent:

    define input param pcCodePage as char.

end procedure. /* dfw_setCodePage */

procedure dfw_setNumFracSep external {&docx_xDocxFactoryLib} cdecl persistent:

    define input param piFrac as byte.

end procedure. /* dfw_setNumFracSep */

procedure dfw_setNumThSep external {&docx_xDocxFactoryLib} cdecl persistent:

    define input param piTh as byte.

end procedure. /* dfw_setNumThSep */

procedure dfw_setDateFormat external {&docx_xDocxFactoryLib} cdecl persistent:

    define input param pcDateFormat as char.

end procedure. /* dfw_setDateFormat */

procedure dfw_setYearOffset external {&docx_xDocxFactoryLib} cdecl persistent:

    define input param piYearOffset as unsigned-short.

end procedure. /* dfw_setYearOffset */

procedure dfw_setFirstWeekDay external {&docx_xDocxFactoryLib} cdecl persistent:

    define input param piWeekday as unsigned-short.

end procedure. /* dfw_setYearOffset */

procedure dfw_setWeekDayNames0 external {&docx_xDocxFactoryLib} cdecl persistent:

end procedure. /* dfw_setWeekDayNames0 */

procedure dfw_setWeekDayNames7 external {&docx_xDocxFactoryLib} cdecl persistent:

    define input   param pcWeekDay1    as char.
    define input   param pcWeekDay2    as char.
    define input   param pcWeekDay3    as char.
    define input   param pcWeekDay4    as char.
    define input   param pcWeekDay5    as char.
    define input   param pcWeekDay6    as char.
    define input   param pcWeekDay7    as char.

end procedure. /* dfw_setWeekDayNames7 */

procedure dfw_setWeekDayNames14 external {&docx_xDocxFactoryLib} cdecl persistent:

    define input   param pcFullName1   as char.
    define input   param pcFullName2   as char.
    define input   param pcFullName3   as char.
    define input   param pcFullName4   as char.
    define input   param pcFullName5   as char.
    define input   param pcFullName6   as char.
    define input   param pcFullName7   as char.

    define input   param pcShortName1  as char.
    define input   param pcShortName2  as char.
    define input   param pcShortName3  as char.
    define input   param pcShortName4  as char.
    define input   param pcShortName5  as char.
    define input   param pcShortName6  as char.
    define input   param pcShortName7  as char.

end procedure. /* dfw_setWeekDayNames14 */

procedure dfw_setMonthNames0 external {&docx_xDocxFactoryLib} cdecl persistent:

end procedure. /* dfw_setMonthNames0 */

procedure dfw_setMonthNames12 external {&docx_xDocxFactoryLib} cdecl persistent:

    define input   param pcMonth1   as char.
    define input   param pcMonth2   as char.
    define input   param pcMonth3   as char.
    define input   param pcMonth4   as char.
    define input   param pcMonth5   as char.
    define input   param pcMonth6   as char.
    define input   param pcMonth7   as char.
    define input   param pcMonth8   as char.
    define input   param pcMonth9   as char.
    define input   param pcMonth10  as char.
    define input   param pcMonth11  as char.
    define input   param pcMonth12  as char.

end procedure. /* dfw_setMonthNames12 */

procedure dfw_setMonthNames24 external {&docx_xDocxFactoryLib} cdecl persistent:

    define input   param pcFullName1   as char.
    define input   param pcFullName2   as char.
    define input   param pcFullName3   as char.
    define input   param pcFullName4   as char.
    define input   param pcFullName5   as char.
    define input   param pcFullName6   as char.
    define input   param pcFullName7   as char.
    define input   param pcFullName8   as char.
    define input   param pcFullName9   as char.
    define input   param pcFullName10  as char.
    define input   param pcFullName11  as char.
    define input   param pcFullName12  as char.

    define input   param pcShortName1  as char.
    define input   param pcShortName2  as char.
    define input   param pcShortName3  as char.
    define input   param pcShortName4  as char.
    define input   param pcShortName5  as char.
    define input   param pcShortName6  as char.
    define input   param pcShortName7  as char.
    define input   param pcShortName8  as char.
    define input   param pcShortName9  as char.
    define input   param pcShortName10 as char.
    define input   param pcShortName11 as char.
    define input   param pcShortName12 as char.

end procedure. /* dfw_setMonthNames24 */

procedure dfw_getCodePage external {&docx_xDocxFactoryLib} cdecl persistent:

    define return param pcRetVal as memptr.

end procedure. /* dfw_getCodePage */

procedure dfw_getNumFracSep external {&docx_xDocxFactoryLib} cdecl persistent:

    define return param piRetVal as byte.

end procedure. /* dfw_getNumFracSep */

procedure dfw_getNumThSep external {&docx_xDocxFactoryLib} cdecl persistent:

    define return param piRetVal as byte.

end procedure. /* dfw_getNumThSep */

procedure dfw_getDateFormat external {&docx_xDocxFactoryLib} cdecl persistent:

    define return param pcRetVal as memptr.

end procedure. /* dfw_getDateFormat */

procedure dfw_getYearOffset external {&docx_xDocxFactoryLib} cdecl persistent:

    define return param piRetVal as unsigned-short.

end procedure. /* dfw_getYearOffset */

procedure dfw_getFirstWeekDay external {&docx_xDocxFactoryLib} cdecl persistent:

    define return param piRetVal as unsigned-short.

end procedure. /* dfw_getFirstWeekDay */

procedure dfw_getWeekDayFullNames external {&docx_xDocxFactoryLib} cdecl persistent:

    define return param pcRetVal as memptr.

end procedure. /* dfw_getWeekDayFullNames */

procedure dfw_getWeekDayShortNames external {&docx_xDocxFactoryLib} cdecl persistent:

    define return param pcRetVal as memptr.

end procedure. /* dfw_getWeekDayShortNames */

procedure dfw_getMonthFullNames external {&docx_xDocxFactoryLib} cdecl persistent:

    define return param pcRetVal as memptr.

end procedure. /* dfw_getMonthFullNames */

procedure dfw_getMonthShortNames external {&docx_xDocxFactoryLib} cdecl persistent:

    define return param pcRetVal as memptr.

end procedure. /* dfw_getMonthShortNames */



procedure dfw_setTempDir external {&docx_xDocxFactoryLib} cdecl persistent:

    define input param pcDir as char.

end procedure. /* dfw_setTempDir */

procedure dfw_getTempDir external {&docx_xDocxFactoryLib} cdecl persistent:

    define return param pcRetVal as memptr.

end procedure. /* dfw_getTempDir */

procedure dfw_getWorkDir external {&docx_xDocxFactoryLib} cdecl persistent:

    define return param pcRetVal as memptr.

end procedure. /* dfw_getWorkDir */



procedure dfw_getErrorWhat external {&docx_xDocxFactoryLib} cdecl persistent:

    define return param pcRetVal as memptr.

end procedure. /* dfw_getErrorDesc */

procedure dfw_getErrorFlag external {&docx_xDocxFactoryLib} cdecl persistent:

    define return param piRetVal as byte.

end procedure. /* dfw_getErrorFlag */
