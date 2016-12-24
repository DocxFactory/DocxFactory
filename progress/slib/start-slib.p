
/**
 * start-slib.p -
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



function getRunFile     returns char private    ( pcFileName as char ) forward.
function normalizePath  returns char private    ( pcPath as char ) forward.
function isRelativePath returns log private     ( pcPath as char ) forward.



define temp-table ttProc no-undo

    field cFullPath     as char
    field hProcHndl     as handle

    index cFullPath is primary unique
          cFullPath.

define temp-table ttStackTrace no-undo

    field cFullPath     as char
    field iProcLevel    as int

    index cFullPath is primary unique
          cFullPath

    index iProcLevel is unique
          iProcLevel.

define var iProcLevel as int no-undo.



on "close" of this-procedure do:

    delete procedure this-procedure.

end. /* on close */

run initializeProc.

procedure initializeProc private:

    empty temp-table ttStackTrace.

    assign
        iProcLevel = 0.

end procedure. /* initializeProc */



procedure startSLib:

    define input    param pcFileName        as char no-undo.
    define input    param phSourceProc      as handle no-undo.
    define output   param phTargetProc      as handle no-undo.
    define output   param plInitializeProc  as log no-undo.

    define buffer ttProc        for ttProc.
    define buffer ttStackTrace  for ttStackTrace.

    define var cFileName    as char no-undo.
    define var hProcHndl    as handle no-undo.

    define var str          as char no-undo.
    define var i            as int no-undo.

    assign
       phTargetProc     = ?
       plInitializeProc = no.

    if phSourceProc = ? then
       phSourceProc = source-procedure.



    cFileName = getRunFile( pcFileName ).

    /***
    if cFileName = ? then
       cFileName = getRunFile( "slib/" + pcFileName ).

    if cFileName = ? then
       cFileName = getRunFile( "xx/" + pcFileName ).

    if cFileName = ? then
       cFileName = getRunFile( "us/xx/" + pcFileName ).
    ***/

    if cFileName = ? then return error.



    find first ttStackTrace
         where ttStackTrace.cFullPath = cFileName
         no-error.

    if avail ttStackTrace then do:

        repeat:

            str = str 
                + ( if str <> "" then " calls~n" else "" )
                + ttStackTrace.cFullPath.

            find next ttStackTrace 
                 use-index iProcLevel 
                 no-error.

            if not avail ttStackTrace then
                leave.

        end. /* repeat */

        str = str 
            + ( if str <> "" then " calls~n" else "" )
            + cFileName + " again.".

        message
            str
        view-as alert-box.

        quit.

    end. /* avail */

    iProcLevel = iProcLevel + 1.

    create ttStackTrace.
    assign
        ttStackTrace.cFullPath  = cFileName
        ttStackTrace.iProcLevel = iProcLevel.



    for each ttProc:

        if not valid-handle( ttProc.hProcHndl )
        or not ttProc.hProcHndl:file-name = ttProc.cFullPath then

            delete ttProc.

    end. /* each ttProc */

    find first ttProc
         where ttProc.cFullPath = cFileName
         no-error.

    if not avail ttProc then do:

        run value( cFileName ) persistent set hProcHndl no-error.
        if error-status:error then return error.

        create ttProc.
        assign
            ttProc.cFullPath    = cFileName
            ttProc.hProcHndl    = hProcHndl.

        plInitializeProc = yes.

    end. /* not avail */



    phTargetProc = ttProc.hProcHndl.
    phSourceProc:add-super-procedure( phTargetProc ).

    iProcLevel = iProcLevel - 1.
    delete ttStackTrace.

end procedure. /* startSLib */



function getRunFile returns char private ( input pcFileName as char ):

    define var cPFileName   as char no-undo.
    define var cRFileName   as char no-undo.
    define var cFullPath    as char no-undo.

    define var cWorkDir     as char no-undo.
    define var cDir         as char no-undo.
    define var i            as int no-undo.

    if pcFileName matches "*~~.r" then do:

        file-info:file-name = pcFileName.

        return file-info:full-pathname.

    end. /* matches "*.r" */



    file-info:file-name = ".".
    
    cWorkDir = file-info:full-pathname.

    assign
        i = r-index( pcFileName, "." )

        cRFileName = ( if i > 0 then substr( pcFileName, 1, i - 1 ) else pcFileName ) + ".r"
        cPFileName = pcFileName.
    
    
    
    repeat i = 1 to num-entries( propath ):

        cDir = normalizePath( entry( i, propath ) ).

        if isRelativePath( cDir ) then
            cDir = normalizePath( cWorkDir + "/" + cDir ).



        if cDir matches "*~~.pl" then
             cFullPath = search( cDir + "<<"    + cRFileName + ">>" ).
        else cFullPath = search( cDir + "/"     + cRFileName ).

        if cFullPath = ? then do:

            if cDir matches "*~~.pl" then
                 cFullPath = search( cDir + "<<"    + cPFileName + ">>" ).
            else cFullPath = search( cDir + "/"     + cPFileName ).

        end. /* cFullPath = ? */

        if cFullPath <> ? then do:

            if cDir matches "*~~.pl" then
                 file-info:file-name = cDir.
            else file-info:file-name = cFullPath.

            return cFullPath.

        end. /* cFullPath <> ? */
        
    end. /* do */

    return ?.

end function. /* getRunFile */

function isRelativePath returns log private ( pcPath as char ):

    if source-procedure <> this-procedure then
        pcPath = normalizePath( pcPath ).
    
    &if "{&opsys}" begins "win" &then

        if  substr( pcPath, 1, 1 ) >= "a" and substr( pcPath, 1, 1 ) <= "z"
        and substr( pcPath, 2, 2 ) = ":~\" 
        or  substr( pcPath, 1, 2 ) = "~\~\" then
    
             return no.
        else return yes.
    
    &else
    
        if pcPath begins "/" then
             return no.
        else return yes.
    
    &endif
        
end function. /* isRelativePath */

function normalizePath returns char private ( pcPath as char ):
    
    &if "{&opsys}" begins "win" &then

        pcPath = replace( trim( pcPath ), "/", "~\" ).
    
        do while index( pcPath, "~\~\", 2 ) <> 0:
            substr( pcPath, 2, length( pcPath ) - 1 ) = replace( substr( pcPath, 2, length( pcPath ) - 1 ), "~\~\", "~\" ).
        end.
    
        return pcPath.

    &else

        pcPath = replace( trim( pcPath ), "~\", "/" ).
    
        do while index( pcPath, "//" ) <> 0:
            pcPath = replace( pcPath, "//", "/" ).
        end.
    
        return pcPath.

    &endif

end function. /* normalizePath */
