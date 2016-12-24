
/**
 * sliberr.p -
 *
 * (c) Copyright ABC Alon Blich Consulting Tech, Ltd.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Contact information
 *  Email: alonblich@gmail.com
 *  Phone: +972-54-218-8086
 */



define temp-table ttErrorMsgFile no-undo

    field iFileId   as int
    field cFileName as char

    index iFileId is primary unique 
          iFileId

    index cFileName is unique 
          cFileName.

define temp-table ttErrorMsg no-undo

    field iFileId   as int
    field cError    as char
    field cErrorMsg as char

    index cError is primary unique iFileId
          cError.



define var hLogProc         as handle no-undo.
define var cLogStream       as char no-undo.

define var iFileIdSeq       as int no-undo.
define var cError           as char no-undo.
define var cErrorMsg        as char no-undo. 
define var cStackTrace      as char no-undo.

define var lQuit            as log no-undo.
define var lStop            as log no-undo.
define var cReturn          as char no-undo.
define var lReturn          as log no-undo.
define var lReturnError     as log no-undo.
define var lReturnNoApply   as log no-undo.



function getErrorMsg    returns char private ( pcError as char, pcParam as char, pcFileName as char ) forward.

function getStackTrace  returns char private ( piStartLevel as int, plHideDlcProc as log ) forward.



on "close" of this-procedure do:

    delete procedure this-procedure.

end. /* on close */

procedure initializeProc:

    assign
        hLogProc    = ?
        cLogStream  = ?

        iFileIdSeq  = 0.

    run err_loadErrorMsgFile( ? ).

    run clearAll.

end procedure. /* initializeProc */



procedure err_directErrors:

    define input param phLogProc    as handle no-undo.
    define input param pcLogStream  as char no-undo.

    if phLogProc    = ?
    or pcLogStream  = ? then

        run err_closeDirectErrors.

    else 

    assign
        hLogProc    = phLogProc
        cLogStream  = pcLogStream.

end procedure. /* err_directErrors */

procedure err_closeDirectErrors:

    assign
        hLogProc    = ?
        cLogStream  = ?.

end procedure. /* err_closeDirectErrors */



function err_isProcUsesSlibErr returns log

    ( input phProc as handle ):

    define var lOk  as log no-undo.
    define var hndl as handle no-undo.
    define var str  as char no-undo.
    define var i    as int no-undo.
    define var j    as int no-undo.

    if not valid-handle( phProc ) or phProc:type <> "procedure" then

        return no.



    assign
       lOk  = no
       j    = num-entries( phProc:super-procedures ).

    do i = 1 to j:

        assign
           hndl = widget-handle( entry( i, phProc:super-procedures ) )
           str  = replace( trim( hndl:file-name ), "~\", "/" ).

        if str = "sliberr.r"
        or str = "sliberr.p"
        or str matches "*/sliberr.r"
        or str matches "*/sliberr.p" then do:

            lOk = yes.
            leave.

        end. /* str = "sliberr" */

    end. /* 1 to j */

    return lOk.

end function. /* err_isProcUsesSlibErr */



procedure err_try:

    run clearAll.

end procedure. /* err_try */

procedure clearAll private:

    assign
        lQuit           = no
        lStop           = no
        cReturn         = ""
        lReturn         = no
        lReturnError    = no
        lReturnNoApply  = no

        cError          = ""
        cErrorMsg       = ""
        cStackTrace     = ""

            no-error. /* clears the error-status object */

end procedure. /* clearAll */



procedure err_NoError:

    define input param pcReturnValue as char no-undo.

    if  not error-status:num-messages > 0
    and not error-status:error then
        return.

    run fetchProErrors( pcReturnValue ).

    if  hLogProc    <> ?
    and cLogStream  <> ? then

        run log_directError in hLogProc ( cLogStream, cErrorMsg ).

    quit.

end procedure. /* err_NoError */

procedure err_NoErrorFlag:

    define input param pcReturnValue as char no-undo.

    if not error-status:error then
        return.

    run fetchProErrors( pcReturnValue ).

    if  hLogProc    <> ?
    and cLogStream  <> ? then

        run log_directError in hLogProc ( cLogStream, cErrorMsg ).

    quit.

end procedure. /* err_NoErrorFlag */

procedure fetchProErrors private:

    define input param pcReturnValue as char no-undo.

    define var i as int no-undo.
    define var j as int no-undo.

    assign
        cError      = ""
        cErrorMsg   = ""
        cStackTrace = ""

    j = error-status:num-messages. 

    if j = 0 then do:

        assign
            cError      = "error"
            cErrorMsg   = ( if pcReturnValue <> "" and pcReturnValue <> ? then pcReturnValue else "" ) + " (error)"
            cStackTrace = getStackTrace( 3, yes ).

                /* i did not use any specific message when return-value is blank, for example, "Error returned."
                   because error messages are language specific. */

    end. /* j = 0 */

    else

    do i = 1 to j:

        assign
            cError      = cError
                + ( if cError <> "" then "," else "" )
                + string( error-status:get-number(i) )

            cErrorMsg   = cErrorMsg
                + ( if cErrorMsg <> "" then chr(1) else "" )
                + error-status:get-message(i)

            cStackTrace = getStackTrace( 3, yes ).

    end. /* 1 to j */

    assign no-error. /* clears the error-status object */

end procedure. /* fetchProErrors */



procedure err_OsError:

    define var iError as int no-undo.

    iError = os-error. if iError <> 0 then do:

        assign
            cError      = "OS" + string( iError )
            cErrorMsg   = "OS Error (OS" + string( iError ) + ")"
            cStackTrace = getStackTrace( 2, yes ).

        if  hLogProc    <> ?
        and cLogStream  <> ? then

            run log_directError in hLogProc ( cLogStream, cErrorMsg ).

        quit.

    end. /* iError <> 0 */

end procedure. /* err_OsError */



&if "{&opsys}" begins "win" &then

&global FORMAT_MESSAGE_FROM_SYSTEM      4096
&global FORMAT_MESSAGE_IGNORE_INSERTS    512

procedure err_WinError:

    define var iError   as int no-undo.

    define var str      as char no-undo.
    define var i        as int no-undo.

    run GetLastError( output iError ). if iError <> 0 then do:

        str = fill( " ", 300 ).

        run FormatMessageA (
            input   {&FORMAT_MESSAGE_FROM_SYSTEM} + {&FORMAT_MESSAGE_IGNORE_INSERTS},
            input   0,
            input   iError,
            input   0,
            output  str,
            input   length( str ),
            input   0,
            output  i ).

        str = substr( str, 1, i ).

        if substr( str, i - 2, 3 ) = ".~r~n" then
           substr( str, i - 2, 3 ) = "".

        assign
            cError      = "WIN" + string( iError )
            cErrorMsg   = str + ". (" + cError + ")"
            cStackTrace = getStackTrace( 2, yes ).

        if  hLogProc    <> ?
        and cLogStream  <> ? then

            run log_directError in hLogProc ( cLogStream, cErrorMsg ).

        quit.

    end. /* iError <> 0 */

end procedure. /* err_WinError */

procedure FormatMessageA external "kernel32.dll":

    define input    param dwFlags       as long.
    define input    param lpSource      as long.
    define input    param dwMessageID   as long.
    define input    param dwLanguageID  as long.
    define output   param lpBuffer      as char.
    define input    param nSize         as long.
    define input    param lpArguments   as long.
    define return   param nTextlength   as long.

end procedure.

procedure GetLastError external "kernel32.dll":

    define return param dwMessageID as long.

end procedure.

&endif /* opsys begins "win" */



function err_catch returns log

    ( output pcError        as char,
      output pcErrorMsg     as char,
      output pcStackTrace   as char ):

    if cError <> "" then do:

        assign
            pcError         = cError
            pcErrorMsg      = cErrorMsg
            pcStackTrace    = cStackTrace

            cError          = ""
            cErrorMsg       = ""
            cStackTrace     = "".

        return yes.

    end. /* cError <> "" */

    else do:

        assign
            pcError         = ?
            pcErrorMsg      = ?
            pcStackTrace    = ?.

        return no.

    end. /* else */

end function. /* err_catch */

function err_throw returns log

    ( pcError       as char,
      pcParam       as char,
      pcFileName    as char ):

    assign
        cError      = pcError
        cErrorMsg   = getErrorMsg( pcError, pcParam, pcFileName )
        cStackTrace = getStackTrace( 2, yes ).

    if  hLogProc    <> ?
    and cLogStream  <> ?

  and ( cError      <> "error"
     or cErrorMsg   <> "" ) then

        run log_directError in hLogProc ( cLogStream, cErrorMsg ).

    quit.

end function. /* err_throw */

function err_throwExact returns log 

    ( pcError       as char,
      pcErrorMsg    as char,
      pcStackTrace  as char ):

    assign
        cError      = pcError
        cErrorMsg   = pcErrorMsg
        cStackTrace = pcStackTrace.

    quit.

end function. /* err_throwExact */



procedure err_loadErrorMsgFile:

    define input param pcFileName as char no-undo.

    define var cFileName    as char no-undo.
    define var str1         as char no-undo.
    define var str2         as char no-undo.

    if pcFileName = ? then
       pcFileName = "slib/errormsg".

    if can-find( 
        first ttErrorMsgFile
        where ttErrorMsgFile.cFileName = pcFileName ) then return.



    cFileName = search( pcFileName ).

    /***
    if cFileName = ? then
       cFileName = search( "slib/" + pcFileName ).

    if cFileName = ? then
       cFileName = search( "xxsrc/" + pcFileName ).

    if cFileName = ? then
       cFileName = search( "us/xxsrc/" + pcFileName ).

    if cFileName = ? then
       cFileName = search( "xx/" + pcFileName ).

    if cFileName = ? then
       cFileName = search( "us/xx/" + pcFileName ).
    ***/

    if cFileName = ? then do:

        message
            'Error messages file "' + pcFileName + '" not found.'
        view-as alert-box error.

        return.

    end. /* cFileName = ? */



    iFileIdSeq = iFileIdSeq + 1.

    create ttErrorMsgFile.
    assign
        ttErrorMsgFile.iFileId      = iFileIdSeq
        ttErrorMsgFile.cFileName    = pcFileName.

    input from value( cFileName ).

    repeat:

        assign
            str1 = ""
            str2 = "".

        import
            str1
            str2.

        assign
            str1 = trim( str1 )
            str2 = trim( str2 ).

        if  str1 <> "" and str2 <> "" and not str1 begins "#"

        and not can-find(
            first ttErrorMsg
            where ttErrorMsg.iFileId    = ttErrorMsgFile.iFileId
              and ttErrorMsg.cError     = str1 ) then do:

            if substr( str2, length( str2 ), 1 ) <> "." then
                str2 = str2 + ".".

            create ttErrorMsg.
            assign
                ttErrorMsg.iFileId      = ttErrorMsgFile.iFileId
                ttErrorMsg.cError       = str1
                ttErrorMsg.cErrorMsg    = str2.

        end. /* str1 <> "" */

    end. /* repeat */

    input close.

end procedure. /* err_loadErrorMsgFile */

function getErrorMsg returns char private

    ( pcError       as char,
      pcParam       as char,
      pcFileName    as char ):

    define var str  as char no-undo.
    define var i    as int no-undo.
    define var j    as int no-undo.

    if pcParam = "" then
       pcParam = " ".

    if pcFileName = ? then
       pcFileName = "slib/errormsg".

    str = ?.

    for first ttErrorMsgFile
        where ttErrorMsgFile.cFileName = pcFileName,

        first ttErrorMsg of ttErrorMsgFile
        where ttErrorMsg.cError = pcError:

        str = ttErrorMsg.cErrorMsg.

    end. /* for each */

    if str = ? and pcFileName <> "slib/errormsg" then

    for first ttErrorMsgFile
        where ttErrorMsgFile.cFileName = "slib/errormsg",

        first ttErrorMsg of ttErrorMsgFile
        where ttErrorMsg.cError = pcError:

        str = ttErrorMsg.cErrorMsg.

    end. /* for each */


        
    if str <> ? then do:

        if pcParam <> ? then do:

            /* we don't use substitute( ) because the number of parameters is dynamic */

            j = min( num-entries( pcParam, chr(1) ), 9 ). do i = 1 to j:

                str = replace( str, "&" + string(i), entry( i, pcParam, chr(1) ) ).

            end.

        end. /* pcParam <> "" */

        str = str + " (" + pcError + ")". /* matches progress error message formats */

    end. /* str <> ? */

    else
    if pcError = "error" then do:

        str = pcParam.

    end. /* pcError = "error" */

    else do:

        str = pcParam + " (" + pcError + ")".

    end. /* else */

    return str.

end function. /* getErrorMsg */



function err_catchQuit returns log:

    if lQuit then do:

        lQuit = no.

        return yes.

    end. /* lReturn */

    return no.

end function. /* err_catchQuit */

function err_catchStop returns log:

    if lStop then do:

        lStop = no.

        return yes.

    end. /* lReturn */

    return no.

end function. /* err_catchStop */

function err_quit returns log:

    lQuit = yes.

    quit.

end function. /* err_quit */

function err_stop returns log:

    lStop = yes.

    quit.

end function. /* err_stop */



function err_catchReturn returns log

    ( output pcReturn as char ):

    if lReturn then do:

        assign
            pcReturn    = cReturn

            cReturn     = ""
            lReturn     = no.

        return yes.

    end. /* lReturn */

    else do:

        pcReturn = ?.

        return no.

    end. /* else */

end function. /* err_catchReturn */

function err_catchReturnError returns log

    ( output pcReturn as char ):

    if lReturnError then do:

        assign
            pcReturn        = cReturn

            cReturn         = ""
            lReturnError    = no.

        return yes.

    end. /* lReturnError */

    else do:

        pcReturn = ?.

        return no.

    end. /* else */

end function. /* err_catchReturnError */

function err_catchReturnNoApply returns log:

    if lReturnNoApply then do:

        lReturnNoApply = no.

        return yes.

    end. /* lReturnNoApply */

    else return no.

end function. /* err_catchReturnNoApply */



function err_return returns log

    ( pcReturn as char ):

    assign
        cReturn = pcReturn
        lReturn = yes.

    quit.

end function. /* err_return */

function err_returnDyn returns char

    ( pcReturn as char ):

    return pcReturn.

end function. /* err_returnDyn */

function err_returnError returns log

    ( pcReturn as char ):

    assign
        cReturn         = pcReturn
        lReturnError    = yes.

    quit.

end function. /* err_returnError */

function err_returnNoApply returns log:

    lReturnNoApply = yes.

    quit.

end function. /* err_returnNoApply */



function getStackTrace returns char private ( piStartLevel as int, plHideDlcProc as log ):

    define var cProgName    as char no-undo.
    define var iLevel       as int no-undo.
    define var RetVal       as char no-undo.

    if piStartLevel = ? then
       piStartLevel = 1.

    if plHideDlcProc = ? then
       plHideDlcProc = yes.



    assign
        RetVal  = ""

        iLevel  = piStartLevel + 1.

    repeat:

        cProgName = program-name( iLevel ).

        if cProgName = ? then
            leave.

        if plHideDlcProc
        and ( cProgName matches "*_runcode~~.*"
           or cProgName matches "*_proedit~~.*"
           or cProgName matches "*_edit~~.*" ) then
            leave.

        assign
            RetVal = program-name( iLevel )
                   + ( if RetVal <> "" then "~n" else "" )
                   + RetVal

            iLevel = iLevel + 1.

    end. /* repeat */

    return RetVal.

end function. /* getStackTrace */
