
/**
 * slibwin.p - win api library main procedure
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



/* the preprocessor below ensures there will be no errors compiling on unix/linux */

&if "{&opsys}" begins "win" &then

{slib/slibwinfrwd.i forward}

{slib/slibwinprop.i}

{slib/slibmath.i}

{slib/slibpro.i}

{slib/sliberr.i}

define var cUtilTail as char no-undo.



on "close" of this-procedure do:

    delete procedure this-procedure.

end. /* on close */

procedure initializeProc:

    run getVersionInfo.

    file-info:file-name = "slib/bin/tail.exe".
    if file-info:full-pathname = ? then do:

        message
            'Tail util "slib/bin/tail.exe" not found.'
        view-as alert-box.

        quit.

    end. /* full-pathname = ? */

    cUtilTail = file-info:full-pathname.

end procedure. /* initializeProc */



procedure getVersionInfo private:

    define var pVersionInfoEx   as memptr no-undo.
    define var retval           as int no-undo.

    {slib/err_try}:

        set-size( pVersionInfoEx )     = 156.
        put-long( pVersionInfoEx, 1 )  = 156.

        run GetVersionExA ( 
            input   get-pointer-value( pVersionInfoEx ), 
            output  retval ). if retval = 0 then {slib/err_win-error-flag}.

        win_iPlatformId = get-long( pVersionInfoEx, 17 ).

        case win_iPlatformId:

            when 0 then /* win 3.1 */

            assign
                win_iMajorVersion       = 0
                win_iMinorVersion       = 0
                win_iBuildNumber        = 0
                win_cCSDVersion         = ""
                win_iServicePackMajor   = 0
                win_iServicePackMajor   = 0
                win_iSuiteMask          = 0
                win_iProductType        = 0
                win_iReserved           = 0.

            when 1 then /* 95/98/me */

            assign
                win_iMinorVersion       = get-byte( pVersionInfoEx, 15 )
                win_iMajorVersion       = get-byte( pVersionInfoEx, 16 )
                win_iBuildNumber        = 0
                win_cCSDVersion         = ""
                win_iServicePackMajor   = 0
                win_iServicePackMajor   = 0
                win_iSuiteMask          = 0
                win_iProductType        = 0
                win_iReserved           = 0.

            otherwise /* xp/2000 ... */

            assign
                win_iMajorVersion       = get-long  ( pVersionInfoEx, 5 )
                win_iMinorVersion       = get-long  ( pVersionInfoEx, 9 )
                win_iBuildNumber        = get-long  ( pVersionInfoEx, 13 )
                win_cCSDVersion         = get-string( pVersionInfoEx, 21 )
                win_iServicePackMajor   = get-short ( pVersionInfoEx, 149 )
                win_iServicePackMajor   = get-short ( pVersionInfoEx, 151 )
                win_iSuiteMask          = get-short ( pVersionInfoEx, 153 )
                win_iProductType        = get-byte  ( pVersionInfoEx, 155 )
                win_iReserved           = get-byte  ( pVersionInfoEx, 156 ).

        end case. /* win_iPlatformId */



        case win_iPlatformId:

            when 0 then
            win_cVersionName = "3.1".

            when 1 then
            win_cVersionName = ( if win_iMinorVersion =  0 then "95" else
                                 if win_iMinorVersion = 10 then "98" else 
                                 if win_iMinorVersion = 90 then "ME" else ? ).

            when 2 then
            win_cVersionName = ( if win_iMajorVersion =  3 then

                                    ( if win_iMinorVersion =  0 then "NT3"       else
                                      if win_iMinorVersion =  1 then "NT3.1"     else
                                      if win_iMinorVersion =  5 then "NT3.5"     else
                                      if win_iMinorVersion = 51 then "NT3.51"    else ? ) else

                                 if win_iMajorVersion =  4 then "NT4" else
                                 if win_iMajorVersion =  5 then 

                                    ( if win_iMinorVersion = 0 then "2000"      else 
                                      if win_iMinorVersion = 1 then "XP"        else
                                      if win_iMinorVersion = 2 then
    
                                            ( if win_iProductType = {&VER_NT_WORKSTATION} then
                                                   "XP 64-bit"
                                              else "2003" ) else ? ) else
    
                                 if win_iMajorVersion =  6 then
    
                                    ( if win_iMinorVersion = 0 then "Vista"     else
                                      if win_iMinorVersion = 1 then "Windows 7" else ? ) else ? ).

            otherwise
            win_cVersionName = ?.

        end case. /* iPlatformId */

        win_cVersionStr = "P" + string( win_iPlatformId, "9" ) + " " 

            + string( win_iMajorVersion, "99" ) + "." + string( win_iMinorVersion, "99" )

                + ( if win_iPlatformId = 2 then " SP" + string( win_iServicePackMajor, "99" ) + "." + string( win_iServicePackMinor, "99" ) else "" ).

    {slib/err_catch}:

        assign
            win_cVersionStr         = ?
            win_cVersionName        = ?
            win_iMajorVersion       = ?
            win_iMinorVersion       = ?
            win_iBuildNumber        = ?
            win_iPlatformId         = ?
            win_cCSDVersion         = ?
            win_iServicePackMajor   = ?
            win_iServicePackMajor   = ?
            win_iSuiteMask          = ?
            win_iProductType        = ?
            win_iReserved           = ?.

    {slib/err_finally}:

        set-size( pVersionInfoEx ) = 0.

    {slib/err_end}.

end procedure. /* getVersionInfo */

procedure GetVersionExA external "kernel32.dll":

    define input    param lpVersionInfoEx   as long.
    define return   param retval            as long.

end procedure.



function win_getHostName returns char:

    /* the function does not throw exceptions. if an error ocurred the functions returns a null value. */

    &scoped xWsaReqVer      257 /* version 1.1. word datatype where the high and low order bytes represent the major and minor version numbers */
    &scoped xWsaDataLen     403
    &scoped xHostNameLen    100

    define var cHostName    as char no-undo.
    define var pWsaData     as memptr no-undo.
    define var iLen         as int no-undo.
    define var retval       as int no-undo.

    {slib/err_try}:

        set-size( pWsaData ) = {&xWsaDataLen}.

        run WSAStartup (
            input   {&xWsaReqVer},
            input   get-pointer-value( pWsaData ),
            output  retval ). if retval <> 0 then {slib/err_win-error-flag}.



        cHostName = fill( " ", {&xHostNameLen} ).

        run gethostname (
            output  cHostName,
            input   {&xHostNameLen},
            output  retval ). if retval <> 0 then {slib/err_win-error-flag}.

    {slib/err_catch}:

        cHostName = ?.

    {slib/err_finally}:

        set-size( pWsaData ) = 0.

        run WSACleanup ( output retval ).

    {slib/err_end}.

    return cHostName.

end function. /* win_getHostName */

procedure gethostname external "wsock32.dll" :

    define output   param lpHostname    as char.
    define input    param iLength       as long.
    define return   param retval        as long.

end procedure.

procedure WSAStartup external "wsock32.dll":

    define input    param wVersionRequested as short.
    define input    param lpWSAData         as long.
    define return   param retval            as long.

end procedure.
 
procedure WSACleanup external "wsock32.dll":

    define return param retval as long.

end procedure.



function win_getUserName returns char:

    define var cName    as char no-undo.
    define var iLen     as int no-undo.
    define var retval   as int no-undo.

    assign
        iLen    = 100
        cName   = fill( " ", iLen ).

    run GetUserNameA (
        input-output    cName,
        input-output    iLen,
        output          retval ).

    if iLen > 1 then
         cName = substr( cName, 1, iLen - 1 ).
    else cName = "".

    return cName.

end function. /* win_getUserName */

procedure GetUserNameA external "advapi32.dll":

    define input-output param lpBuffer  as char.
    define input-output param nSize     as long.
    define return       param retval    as long.

end procedure.



function win_isFileLocked returns logical ( pcFileName as char ):

    /* the function does not throw exceptions. if an error ocurred the functions returns a null value. */

    define var hFile    as int no-undo.
    define var cError   as char no-undo.

    {slib/err_try}:

        run win_lockFile( pcFileName, output hFile ). /* throws an exceptions if an error occured.
                                                         only returns a valid handle if succeeds. */
        run win_closeHandle( hFile ).

        {slib/err_return no}.

    {slib/err_catch cError}:

        if cError = "win32" then /* inuse by another process */
             {slib/err_return yes}.
        else {slib/err_return ?}. /* some other error like file not found */

    {slib/err_end}.

end function. /* win_isFileLocked */

procedure win_lockFile:

    define input    param pcFileName    as char no-undo.
    define output   param phFileHandle  as int no-undo.

    file-info:file-name = pcFileName.

    if file-info:full-pathname = ? then

        {slib/err_throw "'file_not_found'" pcFileName}.

    pcFileName = file-info:full-pathname.



    {slib/err_try}:

        run CreateFileA(
            input   pcFileName,
            input   {&GENERIC_WRITE},
            input   0,
            input   0,
            input   {&OPEN_EXISTING},
            input   {&FILE_ATTRIBUTE_NORMAL},
            input   0,
            output  phFileHandle ) {slib/err_win-error}.

    {slib/err_catch}:

        if phFileHandle <> 0 and phFileHandle <> -1 then

            run win_closeHandle( phFileHandle ).

        {slib/err_throw last}.

    {slib/err_end}.

end procedure. /* win_lockFile */

procedure CreateFileA external "kernel32.dll":

    define input    param lpFileName            as char.
    define input    param dwDesiredAccess       as long.
    define input    param dwShareMode           as long.
    define input    param lpSecurityAttributes  as long.
    define input    param dwCreationDisposition as long.
    define input    param dwFlagsAndAttributes  as long.
    define input    param hTemplateFile         as long.
    define return   param retval                as long.

end procedure.



procedure win_deleteTempFiles:

    define input param pcDir        as char no-undo.
    define input param pcTempFiles  as char no-undo.

    define var cFileName as char no-undo.
    define var cFullPath as char no-undo.
    define var cAttrList as char no-undo.

    if win_tDeleteTempFiles = today then
        return.


    if pcDir = "" then
       pcDir = ?.

    if pcDir = ? then
       pcDir = pro_cTempFullDir.

    if pcTempFiles = "" then
       pcTempFiles = ?.

&if {&pro_xProversion} >= "10.1C" &then

    if pcTempFiles = ? then
       pcTempFiles = "srt*~~.,dbi*~~.,lbi*~~.,pge*~~.,trp*~~.,rpf*~~.,protrace~~.*,p.....*~~.ped,p.....tmp~~.,p.....tmp~~.*,*~~.zip~~.tmp*,................-Content-*~~.txt".

&else

    if pcTempFiles = ? then
       pcTempFiles = "srt*~~.,dbi*~~.,lbi*~~.,rcd*~~.,pge*~~.,trp*~~.,rpf*~~.,protrace~~.*,p.....*~~.ped,p.....tmp~~.,p.....tmp~~.*,*~~.zip~~.tmp*,................-Content-*~~.txt".

&endif

    input from os-dir( pcDir ).

    repeat:

        import
            cFileName
            cFullPath
            cAttrList.

        if cFileName = "."
        or cFileName = ".." then
            next.

        if can-do( pcTempFiles, ( if r-index( cFileName, "." ) = 0 then cFileName + "." else cFileName ) ) then do:

            /***
            /* if not win_isFileLocked( cFullPath ) then *** may get an access denied error when running as a task and user is not logged in.
                but even if the file is locked os-delete will not delete the file and no harm will be done. */

            if can-do( "srt*,dbi*,lbi*,rcd*", cFileName ) then

                os-delete value( cFullPath ).

            /* temp files types that aren't locked are deleted after 3 days. incase they are locked os-delete won't delete the file and no harm will be done. */

            else do:
            ***/

                file-info:file-name = cFullPath.
                if file-info:file-mod-date <= today - 3 then do:

                    if index( cAttrList, "d" ) > 0 then
                    run win_deleteDirNoWait( cFullPath ).

                    else
                    os-delete value( cFullPath ).

                end. /* file-mod-date <= today - 3 */

            /***
            end. /* not can-do */
            ***/

        end. /* can-do( cFileName ) */

    end. /* repeat */

    input close. /* os-dir */



    win_tDeleteTempFiles = today. 

end procedure. /* win_deleteTempFiles */

procedure win_createDir:

    define input param pcDir as char no-undo.

    define var cDir as char no-undo.
    define var str  as char no-undo.
    define var i    as int no-undo.

    if source-procedure <> this-procedure then
       pcDir = win_normalizePath( pcDir ).

    if win_isRelativePath( pcDir ) then
       pcDir = win_normalizePath( pro_cWorkDir + "~\" + pcDir ).

    if pcDir = "" then
       pcDir = ?.

    if pcDir = ? then
        return.

    assign
        cDir    = win_getSubPath( pcDir, "protocol", "drive" )
        pcDir   = win_getSubPath( pcDir, "dir", "ext" ).

    if cDir <> "" then do:

        if cDir matches "*:" then
             file-info:file-name = cDir + "~\".
        else file-info:file-name = cDir.

        if file-info:full-pathname = ?
        or index( file-info:file-type, "d" ) = 0 then

            {slib/err_throw "'dir_not_found'" cDir}.

    end. /* cDir <> "" */

    do i = 1 to num-entries( pcDir, "~\" ):

        str = entry( i, pcDir, "~\" ).

        if str = "" then
            next.

        cDir = win_normalizePath( cDir + "~\" + str ).

        file-info:file-name = cDir.

        if file-info:full-pathname = ? then do:

            os-create-dir value( cDir ) {slib/err_os-error}.

        end. /* file-info:full-pathname = ? */

    end. /* 1 to num-entries */

end procedure. /* win_createDir */

procedure win_deleteDir:
    
    define input param pcDir as char no-undo.

    if source-procedure <> this-procedure then
       pcDir = win_normalizePath( pcDir ).

    if win_isRelativePath( pcDir ) then
       pcDir = win_normalizePath( pro_cWorkDir + "~\" + pcDir ).

    if pcDir = "" then
       pcDir = ?.

    if pcDir = ? then
        return.

    file-info:file-name = pcDir.

    if file-info:full-pathname = ?
    or index( file-info:file-type, "d" ) = 0 then

        return.



    run win_batch(
        input 'rmdir "' + pcDir + '" /S /Q',
        input 'silent,wait' ).

    file-info:file-name = pcDir.

    if  file-info:full-pathname <> ?
    and index( file-info:file-type, "d" ) > 0 then

        {slib/err_throw "'dir_delete_failed'" pcDir}.

end procedure. /* win_deleteDir */

procedure win_deleteDirNoWait:
    
    define input param pcDir as char no-undo.

    run win_batch(
        'rmdir "' + pcDir + '" /S /Q',
        'silent' ). 
        
end procedure. /* win_deleteDirNoWait */



procedure win_tail:

    define input param pcSourceFile as char no-undo.
    define input param pcTargetFile as char no-undo.
    define input param piLines      as int no-undo.

    define var cCmd as char no-undo.

    if source-procedure <> this-procedure then
    assign
        pcSourceFile = win_normalizePath( pcSourceFile )
        pcTargetFile = win_normalizePath( pcTargetFile ).

    file-info:file-name = pcSourceFile.
    if file-info:full-pathname = ? then

        {slib/err_throw "'file_not_found'" pcSourceFile}.

    pcSourceFile = file-info:full-pathname.



    cCmd = '"' + cUtilTail + '" -' + string( piLines ) + ' "' + pcSourceFile + '" > "' + pcTargetFile + '"'.

    run win_batch(
        input cCmd,
        input 'silent,wait' ). 

    if win_iErrorLevel <> 0 then
        {slib/err_throw "'os_util_error'" cCmd "string( win_iErrorLevel )"}.

end procedure. /* win_tail */



function win_isPathOnLocalHardDisk returns log ( pcPath as char ):

    /* the function only makes use of the path's drive and can also be used with files or directories that have not been created yet. */



    /* assumes that if the call is from another procedure or function with in this library then the path has already been normalized. */

    if source-procedure <> this-procedure then
        pcPath = win_normalizePath( pcPath ).

    if win_isRelativePath( pcPath ) then
        pcPath = win_normalizePath( pro_cWorkDir + "~\" + pcPath ).



    if pcPath begins "~\~\" then
         return no.

    if win_getDriveType( substr( pcPath, 1, 3 ) ) = "FIXED" then
         return yes.
    else return no.

end function. /* win_isPathOnLocalHardDisk */

function win_getDriveType returns char ( pcDrive as char ):

    /* note that the function and so does GetDriveType win api (byitself) do not differentiate
       between mounted hard disks and hard disks connected through usb (not usb thumb drive). */

    define var retval as int no-undo.

    run GetDriveTypeA (
        input   pcDrive,
        output  retval ).

    case retval:

        when {&DRIVE_UNKNOWN}       then return ?.
        when {&DRIVE_NO_ROOT_DIR}   then return ?.
        when {&DRIVE_REMOVABLE}     then return "REMOVABLE".
        when {&DRIVE_FIXED}         then return "FIXED".
        when {&DRIVE_REMOTE}        then return "REMOTE".
        when {&DRIVE_CDROM}         then return "CDROM".
        when {&DRIVE_RAMDISK}       then return "RAMDISK".

    end case. /* retval */

end function. /* win_getDriveType */

procedure GetDriveTypeA external "kernel32.dll":

    define input    param lpRootPathName    as char.
    define return   param retval            as long.

end procedure.



function win_getSpecialFolder returns char ( piCSIDL as int ):

    define var pidl     as int no-undo.
    define var cPath    as char no-undo.
    define var retval   as int no-undo.
 
    run SHGetSpecialFolderLocation(
        input   0,
        input   piCSIDL,
        output  pidl,
        output  retval ). if retval <> 0 then {slib/err_win-error-flag}.

    cPath = fill( " ", 260 ).

    run SHGetPathFromIDListA(
        input   pidl,
        output  cPath,
        output  retval ).

    run CoTaskMemFree( pidl ).

    return trim( cPath ).
 
end function. /* win_getSpecialFolder */

procedure SHGetSpecialFolderLocation external "shell32.dll":

    define input    param hwndOwner as long.
    define input    param nFolder   as long.
    define output   param pidl      as long.
    define return   param retval    as long.

end procedure.

procedure SHGetPathFromIDListA external "shell32.dll":

    define input    param pidl      as long.
    define output   param pszPath   as char.
    define return   param retval    as long.

end procedure.

procedure CoTaskMemFree external "ole32.dll":

    define input param lpPidl as long.

end procedure.



function win_getShortPathName returns char ( pcLongPathName as char ):

    define var str      as char no-undo.
    define var retval   as int no-undo.

    str = fill( " ", 68 ).
 
    run GetShortPathNameA (
        input   pcLongPathName,
        output  str,
        input   length( str ),
        output  retval ). if retval = 0 or retval > 68 then {slib/err_win-error-flag}.

    return substr( str, 1, retval ).

end function. /* win_getShortPathName */

procedure GetShortPathNameA external "kernel32.dll":

    define input    param lpszLongPath  as char.
    define output   param lpszShortPath as char.
    define input    param cchBuffer     as long.
    define return   param retval        as long.

end procedure.



function win_getBigFileSize returns dec ( pcFileName as char ):

    define var cTempFile    as char no-undo.
    define var cFileSize    as char no-undo.
    define var dFileSize    as dec no-undo.

    if source-procedure <> this-procedure then
        pcFileName = win_normalizePath( pcFileName ).

    if win_isRelativePath( pcFileName ) then
        pcFileName = win_normalizePath( pro_cWorkDir + "~\" + pcFileName ).



    cTempFile = win_getTempFile( "", ".out" ).

    run win_batch(
        input 'for %%A in ("' + pcFileName + '") do echo %%~~zA > "' + cTempFile + '"',
        input 'silent,wait' ).

    input from value( cTempFile ).
    import unformatted cFileSize.
    input close.

    os-delete value( cTempFile ).

    assign
        dFileSize = ?
        dFileSize = decimal( cFileSize ) no-error.

    return dFileSize.

end function. /* win_getBigFileSize */

procedure win_getDiskFreeSpace:

    define input    param pcRootPath    as char no-undo.
    define output   param pdTotal       as dec no-undo.
    define output   param pdFree        as dec no-undo.
    define output   param pdUsed        as dec no-undo.

    define var ptr1     as memptr no-undo.
    define var ptr2     as memptr no-undo.
    define var ptr3     as memptr no-undo.
    define var retval   as int no-undo.

    assign
        pdTotal = ? 
        pdFree  = ?
        pdUsed  = ?.

    pcRootPath = replace( pcRootPath, "/", "~\" ).

    do while index( pcRootPath, "~\~\", 2 ) <> 0:
        substr( pcRootPath, 2, length( pcRootPath ) - 1 ) = replace( substr( pcRootPath, 2, length( pcRootPath ) - 1 ), "~\~\", "~\" ).
    end.

    if substr( pcRootPath, length( pcRootPath ), 1 ) <> "~\" then
        pcRootPath = pcRootPath + "~\".

    {slib/err_try}:

        set-size( ptr1 ) = 8.
        set-size( ptr2 ) = 8.
        set-size( ptr3 ) = 8.

        run GetDiskFreeSpaceExA (
            input   pcRootPath + chr(0),
            output  ptr1,
            output  ptr2,
            output  ptr3,
            output  retval ). if retval <> 1 then {slib/err_win-error-flag}.

        assign
            pdTotal = math_Int642Dec( ptr2 )
            pdFree  = math_Int642Dec( ptr3 )
            pdUsed  = pdTotal - pdFree.

    {slib/err_catch}:

        {slib/err_throw last}.

    {slib/err_finally}:

        set-size( ptr1 ) = 0.
        set-size( ptr2 ) = 0.
        set-size( ptr3 ) = 0.

    {slib/err_end}.

end procedure. /* win_getDiskFreeSpace */

procedure GetDiskFreeSpaceExA external "kernel32.dll":

    define input    param lpDirectoryName               as char.
    define output   param lpFreeBytesAvailableToCaller  as memptr.
    define output   param lpTotalNumberOfBytes          as memptr.
    define output   param lpTotalNumberOfFreeBytes      as memptr.
    define return   param retval                        as long.

end procedure.



procedure win_breakPath:

    define input    param pcPath            as char no-undo.
    define output   param pcProtocol        as char no-undo.
    define output   param pcComputerName    as char no-undo.
    define output   param pcSharedFolder    as char no-undo.
    define output   param pcDrive           as char no-undo.
    define output   param pcDir             as char no-undo.
    define output   param pcFile            as char no-undo.
    define output   param pcExt             as char no-undo.

    define var i as int no-undo.

    assign
        pcProtocol      = ""
        pcComputerName  = ""
        pcSharedFolder  = ""
        pcDrive         = ""
        pcDir           = ""
        pcFile          = ""
        pcExt           = "".

    /* assumes that if the call is from another procedure or function with in this library then the path has already been normalized. */

    if pcPath = ? then
        return.

    if source-procedure <> this-procedure then
       pcPath = win_normalizePath( pcPath ).

    if pcPath begins "~\~\" then do:

        assign
            pcProtocol = substr( pcPath, 1, 2 )
            substr( pcPath, 1, 2 ) = "".

        i = index( pcPath, "~\", 3 ).
        if i = 0 then i = length( pcPath ) + 1.

        assign
            pcComputerName = substr( pcPath, 1, i - 1 )
            substr( pcPath, 1, i - 1 ) = "".

        i = index( pcPath, "~\", 2 ).
        if i = 0 then i = length( pcPath ) + 1.

        assign
            pcSharedFolder = substr( pcPath, 1, i - 1 )
            substr( pcPath, 1, i - 1 ) = "".

    end. /* pcPath begins "\\" */

    else
    if  substr( pcPath, 1, 1 ) >= "a"
    and substr( pcPath, 1, 1 ) <= "z"
    and substr( pcPath, 2, 1 )  = ":" then do:

        assign
            pcDrive = substr( pcPath, 1, 2 )
            substr( pcPath, 1, 2 ) = "".

    end. /* else */

    i = r-index( pcPath, "~\" ). 
    if i > 0 then

    assign
        pcDir = substr( pcPath, 1, i )
        substr( pcPath, 1, i ) = "".

    i = r-index( pcPath, "." ).
    if i > 0 then

    assign
        pcExt = substr( pcPath, i )
        substr( pcPath, i, length( pcExt ) ) = "".

    pcFile = pcPath.

end procedure. /* win_breakPath */

function win_getSubPath returns char ( pcPath as char, pcFrom as char, pcTo as char ):

    &global xList 'protocol,computername,sharedfolder,drive,dir,file,ext'

    define var iFrom            as int no-undo.
    define var iTo              as int no-undo.

    define var cProtocol        as char no-undo.
    define var cComputerName    as char no-undo.
    define var cSharedFolder    as char no-undo.
    define var cDrive           as char no-undo.
    define var cDir             as char no-undo.
    define var cFile            as char no-undo.
    define var cExt             as char no-undo.

    define var str              as char no-undo.
    define var i                as int no-undo.

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

    if source-procedure <> this-procedure then
       pcPath = win_normalizePath( pcPath ).



    run win_breakPath(
        input   pcPath,
        output  cProtocol,
        output  cComputerName,
        output  cSharedFolder,
        output  cDrive,
        output  cDir,
        output  cFile,
        output  cExt ).

    str = "".

    do i = iFrom to iTo:

        case entry( i, {&xList} ):

            when "protocol"     then str = str + cProtocol.
            when "computername" then str = str + cComputerName.
            when "sharedfolder" then str = str + cSharedFolder.
            when "drive"        then str = str + cDrive.
            when "dir"          then str = str + cDir.
            when "file"         then str = str + cFile.
            when "ext"          then str = str + cExt.

        end case. /* entry */

    end. /* iFrom to iTo */

    return str.

    &undefine xList

end function. /* win_getSubPath */

function win_getRelativePath returns char ( pcSourceDir as char, pcPath as char ):

    define var cPath        as char no-undo.
    define var iSourceLen   as int no-undo.
    define var iLen         as int no-undo.
    define var i            as int no-undo.
    define var j            as int no-undo.

    if source-procedure <> this-procedure then
    assign
        pcSourceDir = win_normalizePath( pcSourceDir )
        pcPath      = win_normalizePath( pcPath ).

    if substr( pcSourceDir, length( pcSourceDir ), 1 ) = "~\" then
       substr( pcSourceDir, length( pcSourceDir ), 1 ) = "".

    assign
        iSourceLen  = num-entries( pcSourceDir, "~\" )
        iLen        = num-entries( pcPath, "~\" ).

    i = 0.

    if substr( pcSourceDir, 1, 1 )  = "~\" and substr( pcPath, 1, 1 )  = "~\"
    or substr( pcSourceDir, 1, 1 ) <> "~\" and substr( pcPath, 1, 1 ) <> "~\" then do:   

        do while i + 1 <= iSourceLen
             and i + 1 <= iLen 
             and entry( i + 1, pcSourceDir, "~\" ) = entry( i + 1, pcPath, "~\" ):

            i = i + 1.

        end. /* do while */

    end. /* substr( ) = "~\" */



    if i = 0 then

        cPath = pcPath.

    else do:

        cPath = "".

        do j = i + 1 to iSourceLen:

            cPath = cPath
                 + ( if cPath <> "" then "~\" else "" )
                 + "..".

        end. /* i + 1 to iLen */

        do j = i + 1 to iLen:

            cPath = cPath
                 + ( if cPath <> "" then "~\" else "" )
                 + entry( j, pcPath, "~\" ).

        end. /* i + 1 to iLen */

    end. /* else */

    return cPath.

end function. /* win_getRelativePath */

/* works for non existent files and dirs possibly prior to been created. */

function win_isRelativePath returns log ( pcPath as char ):

    if pcPath = ? then
        return no.

    /* assumes that if the call is from another procedure or function with in this library then the path has already been normalized. */

    if source-procedure <> this-procedure then
        pcPath = win_normalizePath( pcPath ).

    if substr( pcPath, 1, 1 ) >= "a" and substr( pcPath, 1, 1 ) <= "z" and substr( pcPath, 2, 2 ) = ":~\"
    or substr( pcPath, 1, 2 ) = "~\~\" then

         return no.
    else return yes.

end function. /* win_isRelativePath */

/* works for non existent files and dirs possibly prior to been created. */

function win_isUncPath returns log ( pcPath as char ):

    /* assumes that if the call is from another procedure or function with in this library then the path has already been normalized. */

    if pcPath = ? then
         return no.

    if source-procedure <> this-procedure then
        pcPath = win_normalizePath( pcPath ).

    if substr( pcPath, 1, 2 ) = "~\~\" then
         return yes.
    else return no.

end function. /* win_isUncPath */

function win_normalizePath returns char ( pcPath as char ):

    define var cPath    as char no-undo.
    define var cRoot    as char no-undo.
    define var cDir     as char no-undo.
    define var iDir     as int no-undo.

    define var str      as char no-undo.
    define var i        as int no-undo.

    pcPath = trim( pcPath ).

    if pcPath = ""
    or pcPath = ? then
        return pcPath.

    pcPath = replace( pcPath, "/", "~\" ).

    do while index( pcPath, "~\~\", 2 ) <> 0:
        substr( pcPath, 2, length( pcPath ) - 1 ) = replace( substr( pcPath, 2, length( pcPath ) - 1 ), "~\~\", "~\" ).
    end.

    do while index( pcPath, "::" ) <> 0:
        pcPath = replace( pcPath, "::", ":" ).
    end.

    if lookup( ".", pcPath, "~\" ) > 0 or lookup( "..", pcPath, "~\" ) > 0 then do:

        assign
            cRoot = ""
            cPath = "".

        if pcPath begins "~\~\" then do:

            i = index( pcPath, "~\", 3 ).
            if i = 0 then i = length( pcPath ).

            assign
                cRoot = substr( pcPath, 1, i )
                substr( pcPath, 1, i ) = "".

            i = index( pcPath, "~\" ). 
            if i > 0 then

            assign
                cRoot = cRoot + substr( pcPath, 1, i )
                substr( pcPath, 1, i ) = "".

        end. /* pcPath begins "\\" */

        else
        if  substr( pcPath, 1, 1 ) >= "a"
        and substr( pcPath, 1, 1 ) <= "z"
        and substr( pcPath, 2, 1 )  = ":" then do:

            assign
               cRoot = substr( pcPath, 1, 2 )
               substr( pcPath, 1, 2 ) = "".

            if substr( pcPath, 1, 1 ) = "~\" then
            assign
               cRoot = cRoot + substr( pcPath, 1, 1 )
               substr( pcPath, 1, 1 ) = "".

        end. /* substr = ":" */



        do iDir = 1 to num-entries( pcPath, "~\" ):

            cDir = entry( iDir, pcPath, "~\" ).

            if cDir = "." then do:

                if cPath <> "" or cRoot <> "" then
                    next.

                else
                cPath = cPath
                      + ( if cPath <> "" then "~\" else "" )
                      + cDir.

            end. /* cDir = "." */

            else
            if cDir = ".." then do:

                if cPath <> "" and entry( num-entries( cPath, "~\" ), cPath, "~\" ) <> ".." then do:

                    str = "".

                    do i = 1 to num-entries( cPath, "~\" ) - 1:

                        str = str
                            + ( if str <> "" then "~\" else "" )
                            + entry( i, cPath, "~\" ).

                    end. /* 1 to num-entries */

                    cPath = str.

                end. /* else */

                else
                cPath = cPath
                      + ( if cPath <> "" then "~\" else "" )
                      + cDir.

            end. /* cDir = ".." */

            else
            cPath = cPath
                  + ( if cPath <> "" then "~\" else "" )
                  + cDir.

        end. /* 1 to num-entries */

        pcPath = cPath.

        if cRoot <> "" then
            pcPath = cRoot + pcPath.

    end. /* lookup( ".." ) > 0 */

    return pcPath.

end function. /* win_normalizePath */



function win_getTempFile returns char ( pcUserChars as char, pcExt as char ): /* based on adecomm/_tmpfile.p */

    return win_getTempFileByDir( pro_cTempFullDir, pcUserChars, pcExt ).

end function. /* win_getTempFile */

function win_getTempFileByDir returns char ( pcDir as char, pcUserChars as char, pcExt as char ): /* added after win_getTempFile with the option to specify dir */

    define var cFileName as char no-undo.

    if source-procedure <> this-procedure then
       pcDir = win_normalizePath( pcDir ).

    if pcDir = ? then
       pcDir = pro_cTempFullDir.

    if pcUserChars = ? then
       pcUserChars = "".

    if pcExt = ? then
       pcExt = "".



    if not pcDir matches "*~\" then
       pcDir = pcDir + "~\".
        
    if pcUserChars <> "" and not pcUserChars begins "_" then
       pcUserChars = "_" + pcUserChars.

    if pcExt <> "" and not pcExt begins "." then
       pcExt = "." + pcExt.

    repeat:

        cFileName = pcDir + "p" + string( random( 00000, 99999 ), "99999" ) + "tmp" + pcUserChars + pcExt.

        file-info:file-name = cFileName.
        if file-info:full-pathname = ? then

            leave.

    end. /* repeat */

    return cFileName.

end function. /* win_getTempFileByDir */



function win_getProcessName returns char ( piPid as int ):

    &scoped xMaxPath 260

    define var hProcess     as int no-undo.
    define var iBytesNeeded as int no-undo.
    define var phModule     as memptr no-undo.
    define var cPName       as char no-undo.
    define var retval       as int no-undo.

    if win_cVersionStr < "p2" then

        {slib/err_throw "'win_not_supported'" "'win_getProcessName'"}.

    {slib/err_try}:

        run OpenProcess ( 
            input   {slib/math_or {&PROCESS_QUERY_INFORMATION} {&PROCESS_VM_READ}},
            input   0,
            input   piPid,
            output  hProcess ). if hProcess = 0 then {slib/err_win-error-flag}.

        cPName = fill( " ", {&xMaxPath} ).

        set-size( phModule ) = 4.

        run EnumProcessModules (
            input   hProcess,
            input   get-pointer-value( phModule ),
            input   get-size( phModule ),
            output  iBytesNeeded,
            output  retval ). if retval = 0 then {slib/err_win-error-flag}.

        run GetModuleBaseNameA (
            input   hProcess,
            input   get-long( phModule, 1 ),
            output  cPName,
            input   length( cPName ),
            output  retval ). if retval = 0 then {slib/err_win-error-flag}.

        cPName = substr( cPName, 1, retval ).

        {slib/err_return cPName}.

    {slib/err_catch}:

        {slib/err_return "'[unknown]'"}.

    {slib/err_finally}:

        if get-size( phModule ) > 0 then
           set-size( phModule ) = 0.

        run win_closeHandle( hProcess ).

    {slib/err_end}.

end function. /* win_getProcessName */

procedure EnumProcessModules external "psapi.dll":

    define input    param hProcess      as long.
    define input    param lphModule     as long.
    define input    param cb            as long.
    define output   param cbNeeded      as long.
    define return   param retval        as long.

end procedure.
 
procedure GetModuleBaseNameA external "psapi.dll":

    define input    param hProcess      as long.
    define input    param hModule       as long.
    define output   param lpBaseName    as char.
    define input    param nSize         as long.
    define return   param retval        as long.

end procedure.
 
procedure OpenProcess external "kernel32.dll":

    define input    param dwDesiredAccess   as long.
    define input    param bInheritHandle    as long.
    define input    param dwProcessId       as long.
    define return   param retval            as long.

end procedure.



procedure win_terminateProcess:

    define input param piPid as int no-undo.

    define var hProcess     as int no-undo.
    define var retval       as int no-undo.
    define var lTaskKill    as log no-undo.

    lTaskKill = no.

    if search( win_getSpecialFolder( {&CSIDL_SYSTEM} ) + "\taskkill.exe" ) <> ? then

        lTaskKill = yes.

    /***
    if win_cVersionStr begins "P2 05.01" then do:

        if {slib/math_and {&VER_SUITE_PERSONAL} win_iSuiteMask} = 0 then
           lTaskKill = yes.

    end. /* win XP */

    else
    if win_cVersionStr >= "P2 05.02" then
       lTaskKill = yes.
    ***/



    if lTaskKill then do:

        run win_batch(
            input 'taskkill.exe /PID ' + string( piPid ) + ' /F',
            input 'silent,wait' ).

    end. /* lTaskKill */

    else do:

        {slib/err_try}:
     
            run OpenProcess (
                input   {&PROCESS_TERMINATE}, 
                input   0,
                input   piPid,
                output  hProcess ). if hProcess = 0 then {slib/err_win-error-flag}.
    
            run TerminateProcess (
                input   hProcess,
                input   0,
                output  retval ). if retval = 0 then {slib/err_win-error-flag}.
    
        {slib/err_catch}:
    
            {slib/err_throw last}.
    
        {slib/err_finally}:
    
            run win_closeHandle( hProcess ).
    
        {slib/err_end}.

    end. /* else */

end procedure. /* win_terminateProcess */

procedure TerminateProcess external "kernel32.dll":

    define input    param hProcess  as long.
    define input    param uExitCode as long.
    define return   param retval    as long.

end procedure.



function win_isProcessRunning returns logical ( phProcess as int ):

    define var iExitCode    as int no-undo.
    define var retval       as int no-undo.

    if phProcess = 0 or phProcess = ? then
        return no.

    run Sleep(0).
 
    run GetExitCodeProcess(
        input   phProcess,
        output  iExitCode,
        output  retval ).

    if retval = 0 then
        return no.

    return iExitCode = {&STILL_ACTIVE}.

end function. /* win_isProcessRunning */

procedure GetExitCodeProcess external "kernel32.dll":

    define input    param hProcess  as long.
    define output   param ExitCode  as long.
    define return   param retval    as long.

end procedure.

procedure Sleep external "kernel32.dll":

    define input param dwMilliseconds as long.

end procedure.



procedure win_getProcessList:

    define output param table for win_ttProcess.

    define buffer win_Process for win_ttProcess.
 
    define var pProcessEntry    as memptr no-undo.
    define var hSnapShot        as int no-undo.
    define var pProcessIds      as memptr no-undo.
    define var iBytesReturned   as int no-undo.
    define var iPid             as int no-undo.
    define var cPName           as char no-undo.
 
    define var retval           as int no-undo.
    define var i                as int no-undo.
    define var j                as int no-undo.

    empty temp-table win_Process.

    if win_cVersionStr >= "p2" then do:

        {slib/err_try}:

            set-size( pProcessIds ) = 4000. /* room for 1000 pid's */
 
            run EnumProcesses (
                input   get-pointer-value( pProcessIds ),
                input   get-size( pProcessIds ),
                output  iBytesReturned,
                output  retval ). if retval = 0 then {slib/err_win-error-flag}.
 
            j = iBytesReturned / 4. do i = 1 to j:

                assign
                    iPid    = get-long( pProcessIds, 4 * ( i - 1 ) + 1 )
                    cPName  = win_getProcessName( iPid ).

                create win_Process.
                assign
                    win_Process.iPid    = iPid
                    win_Process.cPName  = cPName. 

            end. /* 1 to j */

        {slib/err_catch}:

            {slib/err_throw last}.

        {slib/err_finally}:

            set-size( pProcessIds ) = 0.

        {slib/err_end}.

    end. /* version >= p2 */



    else do:

        {slib/err_try}:

            run CreateToolhelp32Snapshot(
                input   {&TH32CS_SNAPPROCESS}, 
                input   0, 
                output  hSnapShot ). if hSnapShot = -1 then {slib/err_win-error-flag}.
 
            set-size( pProcessEntry )   = 336.
            put-long( pProcessEntry, 1) = get-size( pProcessEntry ).

            run Process32First(
                input   hSnapShot, 
                input   pProcessEntry,
                output  retval ).

            repeat while retval <> 0:
 
                create win_Process.
                assign
                    win_Process.iPid    = get-long  ( pProcessEntry, 9 )
                    win_Process.cPName  = get-string( pProcessEntry, 37 ). 
 
                run Process32Next(
                    input   hSnapShot, 
                    input   pProcessEntry,
                    output  retval ).

            end. /* repeat */

        {slib/err_catch}:

            {slib/err_throw last}.

        {slib/err_finally}:
 
            if get-size( pProcessEntry ) > 0 then
               set-size( pProcessEntry ) = 0.

            run win_closeHandle( hSnapShot ).

        {slib/err_end}.

    end. /* else */

end procedure. /* getProcessList */

procedure EnumProcesses external "psapi.dll":

    define input    param lpIdProcess   as long.
    define input    param cb            as long.
    define output   param cbNeeded      as long.
    define return   param retval        as long.

end procedure.

procedure CreateToolhelp32Snapshot external "kernel32.dll":

    define input    param dwFlags       as long.
    define input    param th32ProcessId as long.
    define return   param retval        as long.

end procedure.
 
procedure Process32First external "kernel32.dll":

    define input    param hSnapShot         as long.
    define input    param lpProcessEntry32  as memptr.
    define return   param retval            as long.

end procedure.
 
procedure Process32Next external "kernel32.dll":

    define input    param hSnapShot         as long.
    define input    param lpProcessEntry32  as memptr.
    define return   param retval            as long.

end procedure.



procedure win_doShellExecuteEx:

    define input    param pcVerb        as char no-undo.
    define input    param pcFileName    as char no-undo.
    define input    param pcParam       as char no-undo.
    define input    param pcDir         as char no-undo.
    define input    param piShowFlag    as int no-undo.
    define input    param piMaskFlag    as int no-undo.
    define output   param phInstance    as int no-undo.
    define output   param phProcess     as int no-undo.
    define output   param retval        as int no-undo.

    define var pExecInfo    as memptr no-undo.
    define var pVerb        as memptr no-undo.
    define var pFileName    as memptr no-undo.
    define var pParam       as memptr no-undo.
    define var pDir         as memptr no-undo.

    if pcDir = ? then
       pcDir = pro_cWorkDir.

    if piShowFlag = ? then
       piShowFlag = {&SW_SHOWNORMAL}.

    if piMaskFlag = ? then
       piMaskFlag = {slib/math_or {&SEE_MASK_FLAG_NO_UI} {&SEE_MASK_FLAG_NOCLOSEPROCESS}}.

    {slib/err_try}:

        set-size( pVerb )           = length( pcVerb ) + 1.
        put-string( pVerb, 1 )      = pcVerb.
 
        set-size( pFileName )       = length( pcFileName ) + 1.
        put-string( pFileName, 1 )  = pcFileName.

        set-size( pParam )          = length( pcParam ) + 1.
        put-string( pParam, 1 )     = pcParam.

        set-size( pDir )            = length( pcDir ) + 1.
        put-string( pDir, 1 )       = pcDir.

        set-size( pExecInfo )       = 60.
        put-long( pExecInfo,  1 )   = get-size( pExecInfo ).

        put-long( pExecInfo,  5 )   = piMaskFlag.
        put-long( pExecInfo,  9 )   = 0.
        put-long( pExecInfo, 13 )   = get-pointer-value( pVerb ).
        put-long( pExecInfo, 17 )   = get-pointer-value( pFileName ).
        put-long( pExecInfo, 21 )   = get-pointer-value( pParam ).
        put-long( pExecInfo, 25 )   = get-pointer-value( pDir ).
        put-long( pExecInfo, 29 )   = piShowFlag.

        run ShellExecuteExA (
            input   get-pointer-value( pExecInfo ),
            output  retval ). if retval = 0 then {slib/err_win-error-flag}.

        assign
            phInstance  = get-long( pExecInfo, 33 )
            phProcess   = get-long( pExecInfo, 57 ).

    {slib/err_catch}:

        {slib/err_throw last}.

    {slib/err_finally}:

        set-size( pExecInfo )   = 0.
        set-size( pVerb )       = 0.
        set-size( pFileName )   = 0.
        set-size( pParam )      = 0.
        set-size( pDir )        = 0.

    {slib/err_end}.

end procedure. /* win_doShellExecuteEx */

procedure win_doShellExecute:

    define input param pcVerb       as char no-undo.
    define input param pcFileName   as char no-undo.
    define input param pcParam      as char no-undo.
    define input param pcDir        as char no-undo.
    define input param piShowFlag   as int no-undo.

    define var retval as int no-undo.

    if pcDir = ? then
       pcDir = pro_cWorkDir.

    if piShowFlag = ? then
       piShowFlag = {&SW_SHOWNORMAL}.

    run ShellExecuteA(
        input   0,
        input   pcVerb,
        input   pcFileName,
        input   pcParam,
        input   pcDir,
        input   piShowFlag,
        output  retval ). 

    if retval = {&SE_ERR_NOASSOC}
    or retval = {&SE_ERR_ASSOCINCOMPLETE} then do:

        run ShellExecuteA (
            input   0,
            input   "open",
            input   "rundll32.exe",
            input   "shell32.dll,OpenAs_RunDLL " + pcFileName,
            input   pcDir,
            input   piShowFlag,
            output  retval ). if retval <= 32 then {slib/err_win-error-flag}.

    end. /* retval <= err */

    else 
    if retval <= 32 then {slib/err_win-error-flag}.

end procedure. /* win_doShellExecute */

procedure ShellExecuteExA external "shell32.dll":

    define input    param lpExecInfo    as long.
    define return   param retval        as long.

end procedure.

procedure ShellExecuteA external "shell32.dll":

    define input    param hwnd          as long.
    define input    param lpOperation   as char.
    define input    param lpFile        as char.
    define input    param lpParameters  as char.
    define input    param lpDirectory   as char.
    define input    param nShowCmd      as long.
    define return   param retval        as long.

end procedure.



procedure win_batch:

    define input param pcBatch      as char no-undo.
    define input param pcOptions    as char no-undo.

    define var cBatFile     as char no-undo.
    define var cErrFile     as char no-undo.

    define var hInstance    as int no-undo.
    define var hProcess     as int no-undo.

    define var retval       as int no-undo.
    define var i            as int no-undo.

    win_iErrorLevel = 0.

    pcBatch = replace( replace( pcBatch, 
        "~r~n", "~n" ), 
            "~r", "~n" ).

    do i = 1 to num-entries( pcBatch, "~n" ):

        entry( i, pcBatch, "~n" ) = trim( entry( i, pcBatch, "~n" ) ).

        if entry( i, pcBatch, "~n" ) = "proenv" then
           entry( i, pcBatch, "~n" ) = 

                  'set DLC="' + pro_cDlc + '"~n'
                + 'set WRKDIR="' + pro_cWorkDir + '"~n'
                + 'set PATH=%DLC%\bin;' + os-getenv( "PATH" ) + '~n'
                + 'set LIB=%DLC%\LIB;%LIB%'.
             /* + 'cd /D %WRKDIR%'. *** proenv on windows does not do cd to workdir */

    end. /* do i */

    do i = 1 to num-entries( pcOptions ):
        entry( i, pcOptions ) = trim( entry( i, pcOptions ) ).
    end.

    if win_cVersionStr >= "P2 06" then do:

        if lookup( "wait", pcOptions ) = 0 then
            pcOptions = pcOptions + ",wait".
            
        i = lookup( "wait-for-input-idle", pcOptions ).
        if i <> 0 then

        assign
            entry( i, pcOptions ) = ""
            pcOptions = trim( replace( pcOptions, ",,", "," ), "," ).

    end. /* win_cVersionStr >= "p2 06" */

    if  lookup( "pause", pcBatch, "~n" ) > 0
    and lookup( "silent", pcOptions ) > 0 then
 
        {slib/err_throw "'win_silent_pause'" pcBatch}. 



    assign
        cBatFile = win_getTempFile( "", ".bat" )
        cErrFile = win_getTempFile( "", ".err" ).

    output to value( cBatFile ).

        put unformatted
            pcBatch skip.

        if lookup( "wait", pcOptions ) > 0 then
        put unformatted
            'echo %errorlevel% > "' + cErrFile + '"' skip.

        put unformatted
            'del %0' skip.

    output close.



    /* because of the windows uac (started in windows vista) and the security authorizations popups we no longer use shellExecuteEx.
       using only os-command does not support running in the background without the wait option. the wait option is ignored. */
       
    if win_cVersionStr >= "P2 06" then do:

        if lookup( "silent", pcOptions ) > 0 then
        os-command silent value( cBatFile /*** + ">nil 2>&1" *** may cause the batch not to run ***/ ).

        else
        os-command value( cBatFile ).

    end. /* win_cVersionStr >= "p2 06 */

    else do:

        /***
        silent option does not work with win_execute/CreateProcess in console applications.
        for this reason and because you could also insert multiple commands
        win_batch should be used as the main replacement for os-command (for windows).

        run win_execute(
            input cFileName,
            input pro_cWorkDir,
            input pcOptions ).
        ***/

        run win_doShellExecuteEx(
            input "open",
            input cBatFile,
            input "",
            input pro_cWorkDir,
            input ( if lookup( "silent", pcOptions ) > 0 then {&SW_HIDE} else {&SW_SHOWNORMAL} ),
            input ?,
            output hInstance,
            output hProcess,
            output retval ).

        if lookup( "wait-for-input-idle", pcOptions ) > 0 then
            run WaitForInputIdle ( hProcess, -1, output retval ).

        else
        if lookup( "wait", pcOptions ) > 0 then
            run WaitForSingleObject ( hProcess, -1, output retval ).

        run win_closeHandle( hProcess ).

    end. /* else */



    if lookup( "wait", pcOptions ) > 0 then do:

        input from value( cErrFile ).

        do on error undo, leave:
            import win_iErrorLevel.
        end.

        input close.

        os-delete value( cErrFile ).

    end. /* wait */

end procedure. /* win_batch */

procedure win_execute:

    define input param pcCommandLine    as char no-undo.
    define input param pcWorkDir        as char no-undo.
    define input param pcOptions        as char no-undo. /* silent,wait,wait-for-input-idle */

    define var hProcess     as int no-undo.
    define var retval       as int no-undo.

    hProcess = win_createProcess(
        input pcCommandLine,
        input pcWorkDir,
        input ( if lookup( "silent", pcOptions ) > 0 then {&SW_HIDE} else {&SW_SHOWNORMAL} ) ).

    if lookup( "wait-for-input-idle", pcOptions ) > 0 then
        run WaitForInputIdle ( hProcess, -1, output retval ).

    else
    if lookup( "wait", pcOptions ) > 0 then
        run WaitForSingleObject ( hProcess, -1, output retval ).

    run win_closeHandle( hProcess ).

end procedure. /* win_execute */

function win_createProcess returns int

    ( pcCommandLine as char,
      pcWorkDir     as char,
      piShowWindow  as int ):

    define var pWorkDir     as memptr no-undo.
    define var pProcessInfo as memptr no-undo.
    define var pStartupInfo as memptr no-undo.

    define var hProcess     as int no-undo.
    define var hThread      as int no-undo.
    define var retval       as int no-undo.

    if piShowWindow = ? then
       piShowWindow = {&SW_SHOWNORMAL}.

    {slib/err_try}:

        set-size( pProcessInfo )        = 16.
        set-size( pStartupInfo )        = 68.
        put-long( pStartupInfo, 1 )     = 68.

        put-long( pStartupInfo, 45 )    = {&STARTF_USESHOWWINDOW}.
        put-short( pStartupInfo, 49 )   = piShowWindow. 

        if pcWorkDir <> "" then do:

            set-size( pWorkDir )        = 256.
            put-string( pWorkDir, 1 )   = pcWorkDir.

        end. /* pcWorkDir */

        run CreateProcessA (
            input   0,
            input   pcCommandLine,
            input   0,
            input   0,
            input   0,
            input   0,
            input   0,
            input   ( if pcWorkDir = "" then 0 else 
                    get-pointer-value( pWorkDir) ),
            input   get-pointer-value( pStartupInfo ),
            input   get-pointer-value( pProcessInfo ),
            output  retval ). if retval = 0 then {slib/err_win-error-flag}.

        assign
            hProcess = get-long( pProcessInfo, 1 )
            hThread  = get-long( pProcessInfo, 5 ).

        run win_closeHandle( hThread ).

        {slib/err_return hProcess}.

    {slib/err_catch}:

        {slib/err_throw last}.

    {slib/err_finally}:

        set-size( pProcessInfo )    = 0.
        set-size( pStartupInfo )    = 0.
        set-size( pWorkDir )        = 0.

    {slib/err_end}.

end function. /* win_createProcess */

procedure CreateProcessA external "kernel32.dll":

    define input    param lpApplicationName     as long.
    define input    param lpCommandline         as char.
    define input    param lpProcessAttributes   as long.
    define input    param lpThreadAttributes    as long.
    define input    param bInheritHandles       as long.
    define input    param dCreationFlags        as long.
    define input    param lpEnvironment         as long.
    define input    param lpCurrentDirectory    as long.
    define input    param lpStartupInfo         as long.
    define input    param lpProcessInformation  as long.
    define return   param retval                as long.

end procedure.

procedure WaitForSingleObject external "kernel32.dll":

    define input    param hObject   as long.
    define input    param dwTimeout as long.
    define return   param retval    as long.

end procedure.

procedure WaitForInputIdle external "user32.dll":

    define input    param hProcess          as long.
    define input    param dwMilliseconds    as long.
    define return   param retval            as long.

end procedure.



function win_isMutexExists returns log ( plGlobal as log, pcMutex as char ):

    /* the function does not throw exceptions. if an error ocurred the functions returns a null value. */

    define var hMutex as int no-undo.
    define var cError as char no-undo.

    {slib/err_try}:

        run win_createMutex(
            input   plGlobal,
            input   pcMutex,
            output  hMutex ).

        run win_closeHandle( hMutex ).

        {slib/err_return no}.

    {slib/err_catch cError}:

        if cError = "win183" then /* already exists */
             {slib/err_return yes}.
        else {slib/err_return ?}.

    {slib/err_end}.

end function. /* win_isMutexExists */

procedure win_createMutex:

    define input    param plGlobal  as log no-undo.
    define input    param pcMutex   as char no-undo.
    define output   param phMutex   as int no-undo.

    if plGlobal then
        pcMutex = "Global~\" + pcMutex.

    phMutex = ?.

    {slib/err_try}:

        run CreateMutexA (
            input   0,
            input   0,
            input   pcMutex,
            output  phMutex ) {slib/err_win-error}.

    {slib/err_catch}:

        run win_closeHandle( phMutex ).

        {slib/err_throw last}.

    {slib/err_end}.

end procedure. /* win_createMutex */

procedure CreateMutexA external "kernel32.dll":

    define input    param lpMutexAttributes as long.
    define input    param bInitialOwner     as long.
    define input    param lpName            as char.
    define return   param hMutex            as long.

end procedure.



procedure win_pressKey:

    define input param piVirtKey as int no-undo.

    define buffer win_ttInput for win_ttInput.

    if win_cVersionStr >= "p2" then do:

        empty temp-table win_ttInput.

        create win_ttInput.
        assign
            win_ttInput.iSeq        = 1
            win_ttInput.dwType      = {&INPUT_KEYBOARD}
            win_ttInput.wVk         = piVirtKey
            win_ttInput.wScan       = 0
            win_ttInput.dwFlags     = 0
            win_ttInput.dwTime      = 0
            win_ttInput.lpExtraInfo = 0.

        create win_ttInput.
        assign
            win_ttInput.iSeq        = 2
            win_ttInput.dwType      = {&INPUT_KEYBOARD}
            win_ttInput.wVk         = piVirtKey
            win_ttInput.wScan       = 0
            win_ttInput.dwFlags     = {&KEYEVENTF_KEYUP}
            win_ttInput.dwTime      = 0
            win_ttInput.lpExtraInfo = 0.

        run win_sendInput( table win_ttInput &if {&pro_xProversion} >= "10.1A" &then by-reference &endif ). /* if an exception was thrown in win_sendInput( ) it will be passed higher up */

    end. /* version >= p2 */

    else do:

        /*** trying to synthesize mouse and keyboard events from a minimized remote desktop session is denied.
            throws an access denied exception (win5). */

        {slib/err_try}:

            run keybd_event(
                input piVirtKey, 
                input 0,
                input 0,
                input 0 ) {slib/err_win-error}.

            run keybd_event(
                input piVirtKey,
                input 0,
                input {&KEYEVENTF_KEYUP},
                input 0 ) {slib/err_win-error}.

        {slib/err_end}.

    end. /* else */

end procedure. /* win_pressKey */

procedure win_pressKeyDown:

    define input param piVirtKey as int no-undo.

    define buffer win_ttInput for win_ttInput.

    if win_cVersionStr >= "p2" then do:

        empty temp-table win_ttInput.

        create win_ttInput.
        assign
            win_ttInput.iSeq        = 1
            win_ttInput.dwType      = {&INPUT_KEYBOARD}
            win_ttInput.wVk         = piVirtKey
            win_ttInput.wScan       = 0
            win_ttInput.dwFlags     = 0
            win_ttInput.dwTime      = 0
            win_ttInput.lpExtraInfo = 0.

        run win_sendInput( table win_ttInput &if {&pro_xProversion} >= "10.1A" &then by-reference &endif ). /* if an exception was thrown in win_sendInput( ) it will be passed higher up */

    end. /* version >= p2 */

    else do:

        /*** trying to synthesize mouse and keyboard events from a minimized remote desktop session is denied.
            throws an access denied exception (win5). */

        {slib/err_try}:

            run keybd_event(
                input piVirtKey, 
                input 0,
                input 0,
                input 0 ) {slib/err_win-error}.

        {slib/err_end}.

    end. /* else */

end procedure. /* win_pressKeyDown */

procedure win_pressKeyUp:

    define input param piVirtKey as int no-undo.

    define buffer win_ttInput for win_ttInput.

    if win_cVersionStr >= "p2" then do:

        empty temp-table win_ttInput.

        create win_ttInput.
        assign
            win_ttInput.iSeq        = 2
            win_ttInput.dwType      = {&INPUT_KEYBOARD}
            win_ttInput.wVk         = piVirtKey
            win_ttInput.wScan       = 0
            win_ttInput.dwFlags     = {&KEYEVENTF_KEYUP}
            win_ttInput.dwTime      = 0
            win_ttInput.lpExtraInfo = 0.

        run win_sendInput( table win_ttInput &if {&pro_xProversion} >= "10.1A" &then by-reference &endif ). /* if an exception was thrown in win_sendInput( ) it will be passed higher up */

    end. /* version >= p2 */

    else do:

        /*** trying to synthesize mouse and keyboard events from a minimized remote desktop session is denied.
            throws an access denied exception (win5). */

        {slib/err_try}:

            run keybd_event(
                input piVirtKey,
                input 0,
                input {&KEYEVENTF_KEYUP},
                input 0 ) {slib/err_win-error}.

        {slib/err_end}.

    end. /* else */

end procedure. /* win_pressKeyUp */

function win_isKeyPressed returns log ( piVirtKey as int ):

    define var retval as int no-undo.

    /*** trying to synthesize mouse and keyboard events from a minimized remote desktop session is denied.
        throws an access denied exception (win5). */

    {slib/err_try}:

        run GetKeyState( piVirtKey, output retval ) {slib/err_win-error}.

        {slib/err_return "( get-bits( retval, 8 /* high-order bit */, 1 ) = 1 )"}.

    {slib/err_catch}:

        {slib/err_return ?}.

    {slib/err_end}.

end function. /* win_isKeyPressed */

procedure win_setKeyToggleState:

    define input param piVirtKey    as int no-undo.
    define input param plToggle     as log no-undo.

    /* microsoft recommended approach using sendinput */

    if win_isKeyToggled( piVirtKey ) <> plToggle then

       run win_pressKey( piVirtKey ). /* if an exception was thrown it will be passed higher up */

end procedure. /* win_setKeyToggleState */

function win_isKeyToggled returns log ( piVirtKey as int ):

    /* note that with shift, ctrl and alt keys, in most cases you would want to check if they
       are been pressed and not if they have been toggled or not. */

    define var retval as int no-undo.

    /*** trying to synthesize mouse and keyboard events from a minimized remote desktop session is denied.
        throws an access denied exception (win5). */

    {slib/err_try}:

        run GetKeyState( piVirtKey, output retval ) {slib/err_win-error}.

        {slib/err_return "( get-bits( retval, 1 /* low-order bit */, 1 ) = 1 )"}.

    {slib/err_catch}:

        {slib/err_return ?}.

    {slib/err_end}.

end function. /* win_isKeyToggled */

procedure keybd_event external "user32.dll":

    define input    param bVk          as byte.
    define input    param bScan        as byte.
    define input    param dwFlags      as long.
    define input    param dwExtraInfo  as long.    

end procedure.

procedure GetKeyState external "user32.dll":

    define input    param nVirtKey  as long.
    define return   param retval    as short.

end procedure.



procedure win_moveMouse:

    define input param piX as int no-undo.
    define input param piY as int no-undo.

    define var i as int no-undo.



    empty temp-table win_ttInput.

    i = {slib/math_or {&MOUSEEVENTF_MOVE} {&MOUSEEVENTF_ABSOLUTE}}. /* to avoid an assign with both an index field and a usewr-defined function */

    create win_ttInput.
    assign
        win_ttInput.iSeq        = 1
        win_ttInput.dwType      = {&INPUT_MOUSE}
        win_ttInput.dX          = piX
        win_ttInput.dY          = piY
        win_ttInput.dwMouseData = 0
        win_ttInput.dwFlags     = i
        win_ttInput.dwTime      = 0
        win_ttInput.lpExtraInfo = 0.

    run win_sendInput( table win_ttInput &if {&pro_xProversion} >= "10.1A" &then by-reference &endif ). /* if an exception was thrown in win_sendInput( ) it will be passed higher up */

end procedure. /* win_moveMouse */

procedure win_clickMouse:

    define input param pcButton as char no-undo. /* left/right/middle/x */
    define input param piX      as int no-undo.
    define input param piY      as int no-undo.

    define var iButtonDown  as int no-undo.
    define var iButtonUp    as int no-undo.
    define var i            as int no-undo.
 
    case pcButton:

        when "left" then
        assign
            iButtonDown = {&MOUSEEVENTF_LEFTDOWN}
            iButtonUp   = {&MOUSEEVENTF_LEFTUP}.

        when "right" then
        assign
            iButtonDown = {&MOUSEEVENTF_RIGHTDOWN}
            iButtonUp   = {&MOUSEEVENTF_RIGHTUP}.

        when "middle" then
        assign
            iButtonDown = {&MOUSEEVENTF_MIDDLEDOWN}
            iButtonUp   = {&MOUSEEVENTF_MIDDLEUP}.

        when "x" then /* Win2000 or higher */
        assign
            iButtonDown = {&MOUSEEVENTF_XDOWN}
            iButtonUp   = {&MOUSEEVENTF_XUP}.

    end case. /* pcButton */



    empty temp-table win_ttInput.

    i = {slib/math_or {&MOUSEEVENTF_MOVE} {&MOUSEEVENTF_ABSOLUTE}}. /* to avoid an assign with both an index field and a usewr-defined function */

    create win_ttInput.
    assign
        win_ttInput.iSeq        = 1
        win_ttInput.dwType      = {&INPUT_MOUSE}
        win_ttInput.dX          = piX
        win_ttInput.dY          = piY
        win_ttInput.dwMouseData = 0
        win_ttInput.dwFlags     = i
        win_ttInput.dwTime      = 0
        win_ttInput.lpExtraInfo = 0.

    create win_ttInput.
    assign
        win_ttInput.iSeq        = 2
        win_ttInput.dwType      = {&INPUT_MOUSE}
        win_ttInput.dX          = 0
        win_ttInput.dY          = 0
        win_ttInput.dwMouseData = 0
        win_ttInput.dwFlags     = iButtonDown
        win_ttInput.dwTime      = 0
        win_ttInput.lpExtraInfo = 0.

    create win_ttInput.
    assign
        win_ttInput.iSeq        = 3
        win_ttInput.dwType      = {&INPUT_MOUSE}
        win_ttInput.dX          = 0
        win_ttInput.dY          = 0
        win_ttInput.dwMouseData = 0
        win_ttInput.dwFlags     = iButtonUp
        win_ttInput.dwTime      = 0
        win_ttInput.lpExtraInfo = 0.

    run win_sendInput( table win_ttInput &if {&pro_xProversion} >= "10.1A" &then by-reference &endif ). /* if an exception was thrown in win_sendInput( ) it will be passed higher up */

end procedure. /* win_clickMouse */

procedure win_dblclickMouse:

    define input param pcButton as char no-undo. /* left/right/middle/x */
    define input param piX      as int no-undo.
    define input param piY      as int no-undo.

    define var iButtonDown  as int no-undo.
    define var iButtonUp    as int no-undo.
    define var i            as int no-undo.
 
    case pcButton:

        when "left" then
        assign
            iButtonDown = {&MOUSEEVENTF_LEFTDOWN}
            iButtonUp   = {&MOUSEEVENTF_LEFTUP}.

        when "right" then
        assign
            iButtonDown = {&MOUSEEVENTF_RIGHTDOWN}
            iButtonUp   = {&MOUSEEVENTF_RIGHTUP}.

        when "middle" then
        assign
            iButtonDown = {&MOUSEEVENTF_MIDDLEDOWN}
            iButtonUp   = {&MOUSEEVENTF_MIDDLEUP}.

        when "x" then /* Win2000 or higher */
        assign
            iButtonDown = {&MOUSEEVENTF_XDOWN}
            iButtonUp   = {&MOUSEEVENTF_XUP}.

    end case. /* pcButton */



    empty temp-table win_ttInput.

    i = {slib/math_or {&MOUSEEVENTF_MOVE} {&MOUSEEVENTF_ABSOLUTE}}. /* to avoid an assign with both an index field and a usewr-defined function */

    create win_ttInput.
    assign
        win_ttInput.iSeq        = 1
        win_ttInput.dwType      = {&INPUT_MOUSE}
        win_ttInput.dX          = piX
        win_ttInput.dY          = piY
        win_ttInput.dwMouseData = 0
        win_ttInput.dwFlags     = i
        win_ttInput.dwTime      = 0
        win_ttInput.lpExtraInfo = 0.

    create win_ttInput.
    assign
        win_ttInput.iSeq        = 2
        win_ttInput.dwType      = {&INPUT_MOUSE}
        win_ttInput.dX          = 0
        win_ttInput.dY          = 0
        win_ttInput.dwMouseData = 0
        win_ttInput.dwFlags     = iButtonDown
        win_ttInput.dwTime      = 0
        win_ttInput.lpExtraInfo = 0.

    create win_ttInput.
    assign
        win_ttInput.iSeq        = 3
        win_ttInput.dwType      = {&INPUT_MOUSE}
        win_ttInput.dX          = 0
        win_ttInput.dY          = 0
        win_ttInput.dwMouseData = 0
        win_ttInput.dwFlags     = iButtonUp
        win_ttInput.dwTime      = 0
        win_ttInput.lpExtraInfo = 0.

    create win_ttInput.
    assign
        win_ttInput.iSeq        = 4
        win_ttInput.dwType      = {&INPUT_MOUSE}
        win_ttInput.dX          = 0
        win_ttInput.dY          = 0
        win_ttInput.dwMouseData = 0
        win_ttInput.dwFlags     = iButtonDown
        win_ttInput.dwTime      = 0
        win_ttInput.lpExtraInfo = 0.

    create win_ttInput.
    assign
        win_ttInput.iSeq        = 5
        win_ttInput.dwType      = {&INPUT_MOUSE}
        win_ttInput.dX          = 0
        win_ttInput.dY          = 0
        win_ttInput.dwMouseData = 0
        win_ttInput.dwFlags     = iButtonUp
        win_ttInput.dwTime      = 0
        win_ttInput.lpExtraInfo = 0.

    run win_sendInput( table win_ttInput &if {&pro_xProversion} >= "10.1A" &then by-reference &endif ). /* if an exception was thrown in win_sendInput( ) it will be passed higher up */

end procedure. /* win_clickMouse */

procedure win_sendInput:

    define input param table for win_ttInput.

    define var pInputs      as memptr no-undo.
    define var nInputs      as int no-undo.

    define var dPixelWidth  as dec no-undo.
    define var dPixelHeight as dec no-undo.

    define var retval       as int no-undo.
    define var i            as int no-undo.
    define var j            as int no-undo.

    if win_cVersionStr < "p2" then

        {slib/err_throw "'win_not_supported'" "'win_sendInput'"}.



    {slib/err_try}:

        nInputs = 0.

        for each win_ttInput:
            nInputs = nInputs + 1.
        end.

        set-size( pInputs ) = nInputs * 28.



        assign
            dPixelWidth     = ?
            dPixelHeight    = ?

            i               = 0.

        for each win_ttInput:

            assign
                i = i + 1
                j = 28 * ( i - 1 ).

            case win_ttInput.dwType:

                when {&INPUT_KEYBOARD} then do:

                    put-long( pInputs, j + 1 )              = win_ttInput.dwType.
                    put-unsigned-short( pInputs, j + 5 )    = win_ttInput.wVk.
                    put-unsigned-short( pInputs, j + 7 )    = win_ttInput.wScan.
                    put-long( pInputs, j + 9 )              = win_ttInput.dwFlags.
                    put-long( pInputs, j + 13 )             = win_ttInput.dwTime.
                    put-long( pInputs, j + 17 )             = win_ttInput.lpExtraInfo.

                    /* clear empty space */

                    put-long( pInputs, j + 21 )             = 0.
                    put-long( pInputs, j + 25 )             = 0.

                end. /* input_keyboard */

                when {&INPUT_MOUSE} then do:

                    if dPixelWidth  = ? then

                    assign
                       dPixelWidth  = 65536 / ( win_getSystemMetrics( {&SM_CXSCREEN} ) )
                       dPixelHeight = 65536 / ( win_getSystemMetrics( {&SM_CYSCREEN} ) ).

                    put-long( pInputs, j + 1 )              = win_ttInput.dwType.
                    put-long( pInputs, j + 5 )              = int( ( win_ttInput.dX - 1 ) * dPixelWidth ).
                    put-long( pInputs, j + 9 )              = int( ( win_ttInput.dY - 1 ) * dPixelHeight ).
                    put-long( pInputs, j + 13 )             = win_ttInput.dwMouseData.
                    put-long( pInputs, j + 17 )             = win_ttInput.dwFlags.
                    put-long( pInputs, j + 21 )             = win_ttInput.dwTime.
                    put-long( pInputs, j + 25 )             = win_ttInput.lpExtraInfo.

                end. /* input_mouse */

                when {&INPUT_HARDWARE} then do:

                    put-long( pInputs, j + 1 )              = win_ttInput.dwType.
                    put-long( pInputs, j + 5 )              = win_ttInput.uMsg.
                    put-unsigned-short( pInputs, j + 9 )    = win_ttInput.wParamL.
                    put-unsigned-short( pInputs, j + 11 )   = win_ttInput.wParamH.

                    /* clear empty space */

                    put-long( pInputs, j + 13 )             = 0.
                    put-long( pInputs, j + 17 )             = 0.
                    put-long( pInputs, j + 21 )             = 0.
                    put-long( pInputs, j + 25 )             = 0.

                end. /* input_hardware */

            end case. /* dwType */

        end. /* for each */

        run SendInput(
            input   nInputs,
            input   get-pointer-value( pInputs ),
            input   28,
            output  retval ). if retval = 0 then {slib/err_win-error}.

    /*** trying to synthesize mouse and keyboard events from a minimized remote desktop session is denied.
        throws an access denied exception (win5).

    {slib/err_catch}:

        {slib/err_throw last}. */

    {slib/err_finally}:

        set-size( pInputs ) = 0.

    {slib/err_end}.

end procedure. /* win_sendKeybdInput */

procedure SendInput external "user32.dll":

    define input    param nInputs   as long.
    define input    param pInputs   as long.
    define input    param cbSize    as long.
    define return   param retval    as long.

end procedure.



function win_getSystemMetrics returns int ( nIndex as int ):

    define var retval as int no-undo.

    run GetSystemMetrics( nIndex, output retval ).

    return retval.

end function. /* win_getSystemMetrics */

procedure GetSystemMetrics external "user32.dll":

    define input    param nIndex as long.
    define return   param retval as long.

end procedure.



function win_queryPerfCounter returns dec:

    define var ptr      as memptr no-undo.
    define var retval   as int no-undo.

    {slib/err_try}:

        set-size( ptr ) = 8.

        run QueryPerformanceCounter(
            output ptr,
            output retval ). if retval = 0 then {slib/err_win-error-flag}.

        {slib/err_return "math_Int642Dec( ptr )"}.

    {slib/err_finally}:

        set-size( ptr ) = 0.

    {slib/err_end}.

end function. /* win_queryPerCounter */

function win_queryPerfFrequency returns dec:

    define var ptr      as memptr no-undo.
    define var retval   as int no-undo.

    {slib/err_try}:

        set-size( ptr ) = 8.

        run QueryPerformanceFrequency(
            output ptr,
            output retval ). if retval = 0 then {slib/err_win-error-flag}.

        {slib/err_return "math_Int642Dec( ptr )"}.

    {slib/err_finally}:

        set-size( ptr ) = 0.

    {slib/err_end}.

end function. /* win_queryPerfFrequency */

procedure QueryPerformanceFrequency external "kernel32.dll":

    define output param lpFrequency as memptr.
    define return param retval      as long.

end procedure.

procedure QueryPerformanceCounter external "kernel32.dll":

    define output param lpPerformanceCount  as memptr.
    define return param retval              as long.

end procedure.



procedure win_lockWindowUpdate:

    define input param phWidget as widget no-undo.
    
    define var retval as int no-undo.

    if phWidget <> ? then do:

        if phWidget:hwnd = ? then
           phWidget:visible = yes.

        run LockWindowUpdate( phWidget:hwnd, output retval ) {&err_win-error}.

    end. /* phWidget <> ? */

    else do:

        run LockWindowUpdate( 0, output retval ) {&err_win-error}.

    end. /* else */

end procedure. /* win_lockWindowUpdate */

procedure LockWindowUpdate external "user32.dll":

    define input  param hWndLock as long.
    define return param IsLocked as long.

end procedure.



procedure win_setWidgetRTL:

    define input param phWidget as widget no-undo.
    define input param plRTL    as log no-undo.

    define var iStyle   as int no-undo.
    define var i        as int no-undo.

    /* the widget must be realized (the windows control has been created for the widget) before win api's can be used on the widget.
       also note that rtl language support is required, except for vista and higher, apparently. */

    if phWidget:hwnd = ? then
       phWidget:visible = yes. /* realize widget */



    run GetWindowLongA ( 
        input   phWidget:hwnd,
        input   {&GWL_EXSTYLE},
        output  iStyle ).

    assign
        i       = {slib/math_or {&WS_EX_LEFT} {&WS_EX_LTRREADING} {&WS_EX_RIGHTSCROLLBAR}
                           {&WS_EX_RIGHT} {&WS_EX_RTLREADING} {&WS_EX_LEFTSCROLLBAR}
                           {&WS_EX_LAYOUTRTL} {&WS_EX_NOINHERITLAYOUT}}

        i       = {slib/math_not i}
        iStyle  = {slib/math_and iStyle i}.

    if plRTL then
    assign
        iStyle  = 

           ( if  lookup( phWidget:type, "fill-in,editor" ) > 0
             and phWidget:sensitive then

                  {slib/math_or iStyle {&WS_EX_RIGHT} {&WS_EX_RTLREADING} {&WS_EX_LEFTSCROLLBAR}}
             else {slib/math_or iStyle {&WS_EX_RTLREADING} {&WS_EX_LAYOUTRTL} {&WS_EX_NOINHERITLAYOUT}} ).

    else
    assign
        iStyle  = {slib/math_or iStyle {&WS_EX_LEFT} {&WS_EX_LTRREADING} {&WS_EX_RIGHTSCROLLBAR}}.

    run SetWindowLongA (
        input phWidget:hwnd,
        input {&GWL_EXSTYLE},
        input iStyle ).

    run InvalidateRect(
        input   phWidget:hwnd,
        input   0,
        input   1,
        output  i ).

end procedure. /* win_setWidgetRTL */

procedure SetWindowLongA external "user32.dll":

    define input param hWnd         as long.
    define input param nIndex       as long.
    define input param dwNewLong    as long.

end procedure.

procedure GetWindowLongA external "user32.dll":

    define input    param hWnd          as long.
    define input    param nIndex        as long.
    define return   param dwCurrLong    as long.

end procedure.

procedure InvalidateRect external "user32.dll":

    define input    param hWnd      as long.
    define input    param lpRect    as long.
    define input    param bErase    as long.
    define return   param RetVal    as long.

end procedure.



procedure win_bringWindowToTop:

    define input param phWnd as int no-undo.

    run BringWindowToTop( phWnd ).

end procedure. /* win_bringWindowToTop */

procedure BringWindowToTop external "user32.dll":

    define input param hWnd as long.

end procedure.



/* unfortunately this function does not work for console applications, probably, because it requires a message loop. */

function win_getLangId returns char:

    &scoped KL_NAMELENGTH 9

    define var cLangId  as char no-undo.
    define var retval   as int no-undo.

    cLangId = fill( " ", {&KL_NAMELENGTH} ).

    run GetKeyboardLayoutNameA (
        output cLangId,
        output retval ).

    if retval = 0 then return ?.

    cLangId = entry( 1, cLangId, chr(0) ).

    return cLangId.

end function. /* win_getKeyboardLayoutName */

procedure GetKeyboardLayoutNameA external "user32.dll":

    define output param pwszKLID  as char.
    define return param retval    as long.

end procedure.



procedure win_closeHandle:

    /* the procedure like most cleanup operations does not throw exceptions even if the operation fails. */

    define input param hObject as int no-undo.

    define var retval as int no-undo.

    if hObject = 0 or hObject = -1 then
        return.

    run CloseHandle( hObject, output retval ).

end procedure. /* win_closeHandle */

procedure CloseHandle external "kernel32.dll":

    define input    param hObject   as long.
    define return   param retval    as long.

end procedure.

&endif /* opsys begins "win" */
