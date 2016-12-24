
/**
 * slibos.p -
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

{slib/slibosfrwd.i forward}

{slib/slibosprop.i}

&if "{&opsys}" begins "win" &then

    {slib/slibwin.i}

&else

    {slib/slibunix.i}

&endif

{slib/slibpro.i}

{slib/slibpro.i}

{slib/sliberr.i}



on "close" of this-procedure do:

    delete procedure this-procedure.

end. /* close */

procedure initializeProc:

end procedure. /* initializeProc */



function os_getHostName returns char:

    &if "{&opsys}" begins "win" &then
        return  win_getHostName( ).
    &else
        return unix_getHostName( ).
    &endif

end function. /* os_getHostName */

function os_getUserName returns char:

    &if "{&opsys}" begins "win" &then
        return  win_getUserName( ).
    &else
        return unix_getUserName( ).
    &endif

end function. /* os_getUserName */



procedure os_deleteTempFiles:

    define input param pcDir        as char no-undo.
    define input param pcTempFiles  as char no-undo.

    &if "{&opsys}" begins "win" &then
        run  win_deleteTempFiles( pcDir, pcTempFiles ).
    &else
        run unix_deleteTempFiles( pcDir, pcTempFiles ).
    &endif

end procedure. /* os_deleteTempFiles */



function os_isFileExists returns log ( pcPath as char ):

    if source-procedure <> this-procedure then
        pcPath = os_normalizePath( pcPath ).

    file-info:file-name = pcPath.
    if file-info:full-pathname <> ? then

         return yes.
    else return no.

end function. /* os_isFileExists */

function os_isDirExists returns log ( pcPath as char ):

    if source-procedure <> this-procedure then
        pcPath = os_normalizePath( pcPath ).

    file-info:file-name = pcPath.

    if  file-info:full-pathname <> ?
    and index( file-info:file-type, "d" ) > 0 then

         return yes.
    else return no.

end function. /* os_isDirExists */

procedure os_createDir:

    define input param pcDir as char no-undo.

    &if "{&opsys}" begins "win" &then
        run  win_createDir( pcDir ).
    &else
        run unix_createDir( pcDir ).            
    &endif

end procedure. /* os_createDir */

procedure os_deleteDir:

    define input param pcDir as char no-undo.

    &if "{&opsys}" begins "win" &then
        run  win_deleteDir( pcDir ).
    &else
        run unix_deleteDir( pcDir ).            
    &endif

end procedure. /* os_deleteDir */

procedure os_deleteDirNoWait:

    define input param pcDir as char no-undo.

    &if "{&opsys}" begins "win" &then
        run  win_deleteDirNoWait( pcDir ).
    &else
        run unix_deleteDirNoWait( pcDir ).            
    &endif

end procedure. /* os_deleteDirNoWait */



procedure os_tail:

    define input param pcSourceFile as char no-undo.
    define input param pcTargetFile as char no-undo.
    define input param piLines      as int no-undo.

    &if "{&opsys}" begins "win" &then
        run  win_tail( pcSourceFile, pcTargetFile, piLines ).
    &else
        run unix_tail( pcSourceFile, pcTargetFile, piLines ).            
    &endif

end procedure. /* os_tail */



procedure os_fillFile:

    define input    param pcDir         as char no-undo.
    define input    param pcFileList    as char no-undo.
    define output   param table for os_ttFile.

    define var cFileName    as char no-undo.
    define var cFullPath    as char no-undo.
    define var cAttrList    as char no-undo.

    empty temp-table os_ttFile.

    if source-procedure <> this-procedure then
       pcDir = os_normalizePath( pcDir ).

    if os_isRelativePath( pcDir ) then
       pcDir = os_normalizePath( pro_cTempFullDir + pcDir ).

    if pcDir = "" then
       pcDir = ?.

    if pcDir = ? then
        return.

    if pcFileList = "" then
       pcFileList = ?.

    if pcFileList = ? then
       pcFileList = "*".

    if not os_isDirExists( pcDir ) then
        return.



    input from os-dir( pcDir ).

        repeat:

            import
               cFileName
               cFullPath
               cAttrList.

            if cFileName = "."
            or cFileName = ".." then
                next.

            if can-do( pcFileList, cFileName ) then do:

                create os_ttFile.
                assign
                    os_ttFile.cFileName = cFileName
                    os_ttFile.cFullPath = cFullPath
                    os_ttFile.cAttrList = cAttrList.

            end. /* can-do( pcFileList ) */

        end. /* repeat */

    input close.

end procedure. /* os_fillFile */

procedure os_findFile:

    define input    param pcDir         as char no-undo.
    define input    param pcFileList    as char no-undo.
    define output   param pcPathList    as char no-undo.

    if source-procedure <> this-procedure then
       pcDir = os_normalizePath( pcDir ).

    if os_isRelativePath( pcDir ) then
       pcDir = os_normalizePath( pro_cTempFullDir + pcDir ).

    if pcDir = "" then
       pcDir = ?.

    if pcDir = ? then
        return.

    if pcFileList = "" then
       pcFileList = ?.

    if pcFileList = ? then
       pcFileList = "*".
       


    pcPathList = "".

    run findFileRecurr(
        input           pcDir,
        input           pcFileList,
        input-output    pcPathList ).

    if pcPathList = "" then
       pcPathList = ?.

end procedure. /* os_findFile */

procedure findFileRecurr private:

    define input        param pcDir         as char no-undo.
    define input        param pcFileList    as char no-undo.
    define input-output param pcPathList    as char no-undo.

    define var cFileName    as char no-undo.
    define var cFullPath    as char no-undo.
    define var cAttrList    as char no-undo.

    input from os-dir( pcDir ).

        repeat:

            import
               cFileName
               cFullPath
               cAttrList.

            if cFileName = "."
            or cFileName = ".." then
                next.

            else
            if index( cAttrList, "d" ) > 0 then
            
            run findFileRecurr(
                input            cFullPath,
                input           pcFileList,
                input-output    pcPathList ).    

            else
            if can-do( pcFileList, cFileName ) then

            pcPathList = pcPathList
                + ( if pcPathList <> "" then "," else "" )
                + cFullPath.

        end. /* repeat */

    input close.

end procedure. /* findFileRecurr */



procedure os_breakPath:

    define input    param pcPath    as char no-undo.
    define output   param pcDir     as char no-undo.
    define output   param pcFile    as char no-undo.
    define output   param pcExt     as char no-undo.

    &if "{&opsys}" begins "win" &then

        define var cProtocol        as char no-undo.
        define var cComputerName    as char no-undo.
        define var cSharedFolder    as char no-undo.
        define var cDrive           as char no-undo.

        run win_breakPath(
            input   pcPath,
            output  cProtocol,
            output  cComputerName,
            output  cSharedFolder,
            output  cDrive,
            output  pcDir,
            output  pcFile,
            output  pcExt ).

        pcDir = cProtocol + cComputerName + cSharedFolder + cDrive + pcDir.

    &else /* "{&opsys}" begins "win" */

        run unix_breakPath(
            input   pcPath,
            output  pcDir,
            output  pcFile,
            output  pcExt ).

    &endif /* &else */

end procedure. /* os_breakPath */

function os_getSubPath returns char ( pcPath as char, pcFrom as char, pcTo as char ):

    &if "{&opsys}" begins "win" &then

        return win_getSubPath( pcPath,
            ( if pcFrom = ? or pcFrom = "dir" then "protocol" else pcFrom ),
            ( if pcTo = ? then ( if pcFrom = ? then "dir" else pcFrom ) else pcTo ) ).

    &else

        return unix_getSubPath( pcPath,
            ( if pcFrom = ? or pcFrom = "dir" then "dir" else pcFrom ),
            ( if pcTo = ? then ( if pcFrom = ? then "dir" else pcFrom ) else pcTo ) ).

    &endif

end function. /* os_getSubPath */

function os_getFullPath returns char ( pcPath as char ):

    if source-procedure <> this-procedure then
        pcPath = os_normalizePath( pcPath ).

    file-info:file-name = pcPath.
    return file-info:full-pathname.

end function. /* os_getFullPath */

function os_getRelativePath returns char ( pcSourceDir as char, pcPath as char ):

    &if "{&opsys}" begins "win" &then
        return  win_getRelativePath( pcSourceDir, pcPath ).
    &else
        return unix_getRelativePath( pcSourceDir, pcPath ).
    &endif

end function. /* os_getRelativePath */

function os_isRelativePath returns log ( pcPath as char ):

    &if "{&opsys}" begins "win" &then
        return  win_isRelativePath( pcPath ).
    &else
        return unix_isRelativePath( pcPath ).
    &endif

end function. /* os_isRelativePath */

function os_normalizePath returns char ( pcPath as char ):

    &if "{&opsys}" begins "win" &then
        return  win_normalizePath( pcPath ).
    &else
        return unix_normalizePath( pcPath ).
    &endif

end function. /* os_normalizePath */



procedure os_breakFileUrl:

    define input    param pcFileUrl     as char no-undo.
    define output   param pcProtocol    as char no-undo.
    define output   param pcHost        as char no-undo.
    define output   param pcPath        as char no-undo.

    define var i as int no-undo.

    assign
        pcProtocol  = ""
        pcHost      = ""
        pcPath      = "".

    if source-procedure <> this-procedure then
        pcFileUrl = os_normalizeFileUrl( pcFileUrl ).

    if not pcFileUrl begins "file://" then
        {slib/err_throw "'os_invalid_fileurl'" pcFileUrl}.

    assign
        pcProtocol = substr( pcFileUrl, 1, 7 )
        substr( pcFileUrl, 1, 7 ) = "".

    i = index( pcFileUrl, "/" ).
    if i = 0 then i = length( pcFileUrl ).

    assign
        pcHost = substr( pcFileUrl, 1, i - 1 )
        substr( pcFileUrl, 1, i - 1 ) = "".

    assign
        pcPath = pcFileUrl
        pcFileUrl = "".

end procedure. /* os_breakFileUrl */

function os_getSubFileUrl returns char ( pcFileUrl as char, pcFrom as char, pcTo as char ):

    &global xList 'protocol,host,path'

    define var iFrom        as int no-undo.
    define var iTo          as int no-undo.

    define var cProtocol    as char no-undo.
    define var cHost        as char no-undo.
    define var cPath        as char no-undo.

    define var str          as char no-undo.
    define var i            as int no-undo.

    if pcFrom = "" then
       pcFrom = ?.

    if pcFrom = ? then
        return ?.

    if pcTo = "" then
       pcTo = ?.

    if pcTo = ? then
       pcTo = pcFrom.

    assign
       iFrom    = lookup( pcFrom, {&xList} )
       iTo      = lookup( pcTo, {&xList} ).

    if iFrom = 0 then return ?.
    if iTo = 0 then return ?.

    if source-procedure <> this-procedure then
        pcFileUrl = os_normalizeFileUrl( pcFileUrl ).



    run os_breakFileUrl(
        input   pcFileUrl,
        output  cProtocol,
        output  cHost,
        output  cPath ).

    str = "".

    do i = iFrom to iTo:

        case entry( i, {&xList} ):

            when "protocol" then str = str + cProtocol.
            when "host"     then str = str + cHost.
            when "path"     then str = str + cPath.

        end case. /* entry */

    end. /* iFrom to iTo */

    return str.

    &undefine xList

end function. /* os_getSubFileUrl */

function os_FileUrl2Path returns char ( pcFileUrl as char ):

    define var cProtocol    as char no-undo.
    define var cHost        as char no-undo.
    define var cPath        as char no-undo.
    define var str          as char no-undo.

    if source-procedure <> this-procedure then
        pcFileUrl = os_normalizeFileUrl( pcFileUrl ).

    run os_breakFileUrl(
        input   pcFileUrl,
        output  cProtocol,
        output  cHost,
        output  cPath ).

    cPath = replace( cPath, "%20", " " ).

    &if "{&opsys}" begins "win" &then

        if substr( cPath, 1, 1 ) = "/" then
           substr( cPath, 1, 1 ) = "".

        if cHost = "" then
           cHost = "localhost".

        if cHost <> "localhost"
        or substr( cPath, 1, 1 ) <  "a"
        or substr( cPath, 1, 1 ) >  "z"
        or substr( cPath, 2, 1 ) <> ":" then

           cPath = "~\~\" + cHost + "~\" + cPath.

        return win_normalizePath( cPath ).
    &else
        return unix_normalizePath( cPath ).
    &endif

end function. /* os_FileUrl2Path */

function os_normalizeFileUrl returns char ( pcFileUrl as char ):

    define var i as int no-undo.

    pcFileUrl = trim( pcFileUrl ).

    if pcFileUrl = "" or pcFileUrl = ? then
        return pcFileUrl.

    if not pcFileUrl begins "file://" then
        return pcFileUrl.



    substr( pcFileUrl, 1, 4 ) = lc( substr( pcFileUrl, 1, 4 ) ).

    substr( pcFileUrl, 8 /* length( "file://" ) + 1 */, length( pcFileUrl ) - 7 ) = replace( substr( pcFileUrl, 8, length( pcFileUrl ) - 7 ), "~\", "/" ).

    do while index( pcFileUrl, "//", 8 ) <> 0:
        substr( pcFileUrl, 8, length( pcFileUrl ) - 7 ) = replace( substr( pcFileUrl, 8, length( pcFileUrl ) - 7 ), "//", "/" ).
    end.

    do while index( pcFileUrl, "::" ) <> 0:
        pcFileUrl = replace( pcFileUrl, "::", ":" ).
    end.

    pcFileUrl = replace( pcFileUrl, " ", "%20" ).



    i = index( pcFileUrl, "/", 8 ).

    if  substr( pcFileUrl, i + 1, 1 ) >= "a"
    and substr( pcFileUrl, i + 1, 1 ) <= "z"
    and substr( pcFileUrl, i + 2, 1 )  = "|" then
        substr( pcFileUrl, i + 2, 1 )  = ":".

    return pcFileUrl.

end function. /* os_normalizeFileUrl */



function os_getTempFile returns char ( pcUserChars as char, pcExt as char ): /* based on adecomm/_tmpfile.p */

    &if "{&opsys}" begins "win" &then
        return  win_getTempFile( pcUserChars, pcExt ).
    &else
        return unix_getTempFile( pcUserChars, pcExt ).
    &endif

end function. /* os_getTempFile */

function os_getTempFileByDir returns char ( pcDir as char, pcUserChars as char, pcExt as char ): /* added after os_getTempFile with the option to specify dir */

    &if "{&opsys}" begins "win" &then
        return  win_getTempFileByDir( pcDir, pcUserChars, pcExt ).
    &else
        return unix_getTempFileByDir( pcDir, pcUserChars, pcExt ).
    &endif

end function. /* os_getTempFile */

function os_getNextFile returns char ( pcFileName as char ):

    define var cFileName    as char no-undo.
    define var cDir         as char no-undo.
    define var cFile        as char no-undo.
    define var cExt         as char no-undo.
    define var i            as int no-undo.

    if source-procedure <> this-procedure then
        pcFileName = os_normalizePath( pcFileName ).

    file-info:file-name = pcFileName.
    if file-info:full-pathname = ? then
        return pcFileName.

    run os_breakPath(
        input   pcFileName,
        output  cDir,
        output  cFile,
        output  cExt ).

    i = 2.

    repeat:

        cFileName = cDir + cFile + "(" + string(i) + ")" + cExt.

        file-info:file-name = cFileName.
        if file-info:full-pathname = ? then

            return cFileName.

        i = i + 1.

    end. /* repeat */

end function. /* os_getNextFile */

function os_isEmptyDir returns log ( pcDir as char ):

    define var cFileName    as char no-undo.
    define var cFullPath    as char no-undo.
    define var lEmpty       as log no-undo.

    if not os_isDirExists( pcDir ) then
        {slib/err_throw "'dir_not_found'" pcDir}.

    lEmpty = yes.

    input from os-dir( pcDir ) no-attr-list.

        _loop:

        repeat:

            import
                cFileName
                cFullPath.

            if  cFileName <> "."
            and cFileName <> ".." then do:

                lEmpty = no.
                leave _loop.

            end. /* cFileName <> "." and cFileName <> ".." */

        end. /* repeat */

    input close.

    return lEmpty.

end function. /* os_isEmptyDir */



function os_getBigFileSize returns dec ( pcFileName as char ):

    &if "{&opsys}" begins "win" &then
        return  win_getBigFileSize( pcFileName ).
    &else
        return unix_getBigFileSize( pcFileName ).
    &endif 

end function. /* os_getBigFileSize */

procedure os_getDiskFreeSpace:

    define input    param pcPath    as char no-undo.
    define output   param pdTotal   as dec no-undo.
    define output   param pdFree    as dec no-undo.
    define output   param pdUsed    as dec no-undo.
    
    &if "{&opsys}" begins "win" &then
    
        run win_getDiskFreeSpace(
            input   pcPath,
            output  pdTotal,
            output  pdFree,
            output  pdUsed ).
    
    &else
    
        run unix_getDiskFreeSpace(
            input   pcPath,
            output  pdTotal,
            output  pdFree,
            output  pdUsed ).

    &endif 
    
end procedure. /* os_getDiskFreeSpace */
