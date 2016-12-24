
/**
 * slibunix.p -
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

{slib/slibunixfrwd.i forward}

{slib/slibunixprop.i}

{slib/slibpro.i}

{slib/sliberr.i}



on "close" of this-procedure do:

    delete procedure this-procedure.

end. /* close */

procedure initializeProc:

end procedure. /* initializeProc */



function unix_getHostName returns char:

    define var retval as char no-undo.

    input through value( "hostname -f 2> /dev/null" ). /* if this doesn't work on your platform, try uname -n */
    import unformatted retval.
    input close.

    return retval.

end function. /* unix_getHostName */

function unix_getUserName returns char:

    return os-getenv( "username" ).

end function. /* unix_getUserName */



procedure unix_deleteTempFiles:

    define input param pcDir        as char no-undo.
    define input param pcTempFiles  as char no-undo.

    define var cFileName as char no-undo.
    define var cFullPath as char no-undo.
    define var cAttrList as char no-undo.

    if unix_tDeleteTempFiles = today then
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

            file-info:file-name = cFullPath.
            if file-info:file-mod-date <= today - 3 then do:

                if index( cAttrList, "d" ) > 0 then
                run unix_deleteDirNoWait( cFullPath ).

                else
                os-delete value( cFullPath ).

            end. /* file-mod-date <= today - 3 */

        end. /* can-do( cFileName ) */

    end. /* repeat */

    input close. /* os-dir */



    unix_tDeleteTempFiles = today. 

end procedure. /* unix_deleteTempFiles */

procedure unix_createDir:

    define input param pcDir as char no-undo.

    define var cDir as char no-undo.
    define var i    as int no-undo.

    if source-procedure <> this-procedure then
       pcDir = unix_normalizePath( pcDir ).

    if unix_isRelativePath( pcDir ) then
       pcDir = unix_normalizePath( pro_cWorkDir + "/" + pcDir ).

    if pcDir = "" then
       pcDir = ?.

    if pcDir = ? then
        return.

    cDir = entry( 1, pcDir, "/" ).

    do i = 2 to num-entries( pcDir, "/" ):

        cDir = cDir
            + "/"
            + entry( i, pcDir, "/" ).

        file-info:file-name = cDir.

        if file-info:full-pathname = ? then do:

            os-create-dir value( cDir ) {slib/err_os-error}.

        end. /* file-info:full-pathname = ? */

    end. /* 2 to num-entries */

end procedure. /* unix_createDir */

procedure unix_deleteDir:

    define input param pcDir as char no-undo.

    /***
    define var cFileName as char no-undo.
    define var cFullPath as char no-undo.
    define var cAttrList as char no-undo.
    ***/

    if source-procedure <> this-procedure then
       pcDir = unix_normalizePath( pcDir ).

    if unix_isRelativePath( pcDir ) then
       pcDir = unix_normalizePath( pro_cWorkDir + "/" + pcDir ).

    if pcDir = "" then
       pcDir = ?.

    if pcDir = ?
    or pcDir = "/" then
        return.

    file-info:file-name = pcDir.

    if file-info:full-pathname = ?
    or index( file-info:file-type, "d" ) = 0 then

        return.

    run unix_shell(
        input 'rm -rf "' + pcDir + '"',
        input 'silent,wait' ).

    file-info:file-name = pcDir.

    if  file-info:full-pathname <> ?
    and index( file-info:file-type, "d" ) > 0 then

        {slib/err_throw "'dir_delete_failed'" pcDir}.

end procedure. /* unix_deleteDir */

procedure unix_deleteDirNoWait:

    define input param pcDir as char no-undo.

    /***
    define var cFileName as char no-undo.
    define var cFullPath as char no-undo.
    define var cAttrList as char no-undo.
    ***/

    if pcDir = "" then
       pcDir = ?.

    if pcDir = ? 
    or pcDir = "/" then
        return.

    run unix_shell(
        input 'rm -rf "' + pcDir + '"',
        input 'silent' ).

end procedure. /* unix_deleteDirNoWait */



procedure unix_tail:

    define input param pcSourceFile as char no-undo.
    define input param pcTargetFile as char no-undo.
    define input param piLines      as int no-undo.

    define var cCmd as char no-undo.

    if source-procedure <> this-procedure then
    assign
        pcSourceFile = unix_normalizePath( pcSourceFile )
        pcTargetFile = unix_normalizePath( pcTargetFile ).

    file-info:file-name = pcSourceFile.
    if file-info:full-pathname = ? then

        {slib/err_throw "'file_not_found'"}.

    pcSourceFile = file-info:full-pathname.



    cCmd = 'tail -' + string( piLines ) + ' "' + pcSourceFile + '" > "' + pcTargetFile + '"'.

    run unix_shell(
        input cCmd,
        input 'silent,wait' ). 

    if unix_iExitCode <> 0 then
        {slib/err_throw "'os_util_error'" cCmd "string( unix_iExitCode )"}.

end procedure. /* unix_tail */



procedure unix_getFilePerm:

    define input    param pcFileName    as char no-undo.
    define output   param pcUser        as char no-undo.
    define output   param pcGroup       as char no-undo.
    define output   param piPerm        as int no-undo.

    define var str as char no-undo.

    if source-procedure <> this-procedure then
        pcFileName = unix_normalizePath( pcFileName ).

    if unix_isRelativePath( pcFileName ) then
        pcFileName = unix_normalizePath( pro_cWorkDir + "/" + pcFileName ).

    if search( pcFileName ) = ? then
        {slib/err_throw "'file_not_found'" pcFileName}.
        


    input through value( 'ls -l "' + pcFileName + '"' ).
    import unformatted str.
    input close.
   
    assign
        str = left-trim( str )
        
        piPerm  = ( if substr( str,  2, 1 ) = "r" then 400 else 0 )
                + ( if substr( str,  3, 1 ) = "w" then 200 else 0 )
                + ( if substr( str,  4, 1 ) = "x" then 100 else 0 )
          
                + ( if substr( str,  5, 1 ) = "r" then 40 else 0 )
                + ( if substr( str,  6, 1 ) = "w" then 20 else 0 )
                + ( if substr( str,  7, 1 ) = "x" then 10 else 0 )
 
                + ( if substr( str,  8, 1 ) = "r" then 4 else 0 )
                + ( if substr( str,  9, 1 ) = "w" then 2 else 0 )
                + ( if substr( str, 10, 1 ) = "x" then 1 else 0 )

        entry( 1, str, " " ) = ""
        str = left-trim( str )
        
        entry( 1, str, " " ) = ""
        str = left-trim( str )

        pcUser  = trim( entry( 1, str, " " ) )
        
        entry( 1, str, " " ) = ""
        str = left-trim( str )
        
        pcGroup = trim( entry( 1, str, " " ) ).

end procedure. /* getFilePerm */

procedure unix_setFilePerm:

    define input param pcFileName   as char no-undo.
    define input param pcUser       as char no-undo.
    define input param pcGroup      as char no-undo.
    define input param piPerm       as int no-undo.

    define var cCmd as char no-undo.

    if source-procedure <> this-procedure then
        pcFileName = unix_normalizePath( pcFileName ).

    if unix_isRelativePath( pcFileName ) then
        pcFileName = unix_normalizePath( pro_cWorkDir + "/" + pcFileName ).

    if search( pcFileName ) = ? then
        {slib/err_throw "'file_not_found'" pcFileName}.
 
    if pcUser = "" then pcUser = ?.
    if pcGroup = "" then pcGroup = ?.
    if piPerm = 0 then piPerm = ?.
    


    /* changing the permissions is done before changing the owner
    
       because changing the permissions is much less likely to fail. */

    if piPerm <> ? then do:
    
        cCmd = 'chmod ' + string( piPerm, "999" ) + ' "' + pcFileName + '"'.
    
        run unix_shell(
            input cCmd,
            input 'silent,wait' ).
    
        if unix_iExitCode <> 0 then
            {slib/err_throw "'os_util_error'" cCmd "string( unix_iExitCode )"}.
        
    end. /* piPerm <> ? */

    if pcUser <> ? and pcGroup <> ? then do:
    
        cCmd = 'chown ' + pcUser + ':' + pcGroup + ' "' + pcFileName + '"'.
    
        run unix_shell(
            input cCmd,
            input 'silent,wait' ).
    
        if unix_iExitCode <> 0 then
            {slib/err_throw "'os_util_error'" cCmd "string( unix_iExitCode )"}.
 
    end. /* pcUser <> ? and pcGroup <> ? */
        
    else
    if pcUser <> ? then do:
    
        cCmd = 'chown ' + pcUser + ' "' + pcFileName + '"'.
    
        run unix_shell(
            input cCmd,
            input 'silent,wait' ).
    
        if unix_iExitCode <> 0 then
            {slib/err_throw "'os_util_error'" cCmd "string( unix_iExitCode )"}.
 
    end. /* pcUser <> ? */

end procedure. /* unix_setFilePerm */



procedure unix_breakPath:

    define input    param pcPath    as char no-undo.
    define output   param pcDir     as char no-undo.
    define output   param pcFile    as char no-undo.
    define output   param pcExt     as char no-undo.

    define var i as int no-undo.

    assign
        pcDir   = ""
        pcFile  = ""
        pcExt   = "".

    if pcPath = ? then
        return.

    if source-procedure <> this-procedure then
        pcPath = unix_normalizePath( pcPath ).

    i = r-index( pcPath, "/" ). 
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

end procedure. /* unix_breakPath */

function unix_getSubPath returns char ( pcPath as char, pcFrom as char, pcTo as char ):

    &global xList 'dir,file,ext'

    define var iFrom    as int no-undo.
    define var iTo      as int no-undo.

    define var cDir     as char no-undo.
    define var cFile    as char no-undo.
    define var cExt     as char no-undo.

    define var str      as char no-undo.
    define var i        as int no-undo.

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
        pcPath = unix_normalizePath( pcPath ).



    run unix_breakPath(
        input   pcPath,
        output  cDir,
        output  cFile,
        output  cExt ).

    str = "".

    do i = iFrom to iTo:

        case entry( i, {&xList} ):

            when "dir"  then str = str + cDir.
            when "file" then str = str + cFile.
            when "ext"  then str = str + cExt.

        end case. /* entry */

    end. /* iFrom to iTo */

    return str.

    &undefine xList

end function. /* unix_getSubPath */

function unix_getRelativePath returns char ( pcSourceDir as char, pcPath as char ):

    define var cPath        as char no-undo.
    define var iSourceLen   as int no-undo.
    define var iLen         as int no-undo.
    define var i            as int no-undo.
    define var j            as int no-undo.

    if source-procedure <> this-procedure then
    assign
        pcSourceDir = unix_normalizePath( pcSourceDir )
        pcPath      = unix_normalizePath( pcPath ).

    if substr( pcSourceDir, length( pcSourceDir ), 1 ) = "/" then
       substr( pcSourceDir, length( pcSourceDir ), 1 ) = "".

    assign
        iSourceLen  = num-entries( pcSourceDir, "/" )
        iLen        = num-entries( pcPath, "/" ).

    i = 0.

    if substr( pcSourceDir, 1, 1 )  = "/" and substr( pcPath, 1, 1 )  = "/"
    or substr( pcSourceDir, 1, 1 ) <> "/" and substr( pcPath, 1, 1 ) <> "/" then do:   

        do while i + 1 <= iSourceLen
             and i + 1 <= iLen 
             and entry( i + 1, pcSourceDir, "/" ) = entry( i + 1, pcPath, "/" ):

            i = i + 1.

        end. /* do while */

    end. /* substr( ) = "/" */



    if i = 0 then

        cPath = pcPath.

    else do:

        cPath = "".

        do j = i + 1 to iSourceLen:

            cPath = cPath
                 + ( if cPath <> "" then "/" else "" )
                 + "..".

        end. /* i + 1 to iLen */

        do j = i + 1 to iLen:

            cPath = cPath
                 + ( if cPath <> "" then "/" else "" )
                 + entry( j, pcPath, "/" ).

        end. /* i + 1 to iLen */

    end. /* else */

    return cPath.

end function. /* unix_getRelativePath */

function unix_isRelativePath returns log ( pcPath as char ):

    /* assumes that if the call is from another procedure or function with in this library then
       the path has already been normalized. */

    if pcPath = ? then
         return no.

    if source-procedure <> this-procedure then
        pcPath = unix_normalizePath( pcPath ).

    if pcPath begins "/" then
         return no.
    else return yes.

end function. /* unix_isRelativePath */

function unix_normalizePath returns char ( pcPath as char ):

    define var cPath    as char no-undo.
    define var cRoot    as char no-undo.
    define var cDir     as char no-undo.
    define var iDir     as int no-undo.

    define var str      as char no-undo.
    define var i        as int no-undo.

    if pcPath = ? then
        return ?.

    pcPath = replace( trim( pcPath ), "~\", "/" ).

    do while index( pcPath, "//" ) <> 0:
        pcPath = replace( pcPath, "//", "/" ).
    end.



    if lookup( ".", pcPath, "/" ) > 0 or lookup( "..", pcPath, "/" ) > 0 then do:

        assign
            cRoot = ""
            cPath = "".

        if substr( pcPath, 1, 1 ) = "/" then

        assign
           cRoot = substr( pcPath, 1, 1 )
           substr( pcPath, 1, 1 ) = "".

        do iDir = 1 to num-entries( pcPath, "/" ):

            cDir = entry( iDir, pcPath, "/" ).

            if cDir = "." then do:

                if cPath <> "" or cRoot <> "" then
                    next.

                else
                cPath = cPath
                      + ( if cPath <> "" then "/" else "" )
                      + cDir.

            end. /* cDir = "." */

            else
            if cDir = ".." then do:

                if cPath <> "" and entry( num-entries( cPath, "/" ), cPath, "/" ) <> ".." then do:

                    str = "".
    
                    do i = 1 to num-entries( cPath, "/" ) - 1:

                        str = str
                            + ( if str <> "" then "/" else "" )
                            + entry( i, cPath, "/" ).

                    end. /* 1 to num-entries */

                    cPath = str.

                end. /* else */

                else
                cPath = cPath
                      + ( if cPath <> "" then "/" else "" )
                      + cDir.

            end. /* cDir = ".." */

            else
            cPath = cPath
                  + ( if cPath <> "" then "/" else "" )
                  + cDir.

        end. /* 1 to num-entries */

        pcPath = cPath.

        if cRoot <> "" then
            pcPath = cRoot + pcPath.

    end. /* lookup( ".." ) > 0 */

    return pcPath.

end function. /* unix_normalizePath */



function unix_getTempFile returns char ( pcUserChars as char, pcExt as char ): /* based on adecomm/_tmpfile.p */

    return unix_getTempFileByDir( pro_cTempFullDir, pcUserChars, pcExt ).

end function. /* unix_getTempFile */

function unix_getTempFileByDir returns char ( pcDir as char, pcUserChars as char, pcExt as char ): /* added after unix_getTempFile with the option to specify dir */

    define var cFileName as char no-undo.

    if source-procedure <> this-procedure then
       pcDir = unix_normalizePath( pcDir ).

    if pcDir = ? then
       pcDir = pro_cTempFullDir.

    if pcUserChars = ? then
       pcUserChars = "".

    if pcExt = ? then
       pcExt = "".



    if not pcDir matches "*/" then
       pcDir = pcDir + "/".
        
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

end function. /* unix_getTempFileByDir */



function unix_getBigFileSize returns dec ( pcFileName as char ):

    define var cFileSize    as char no-undo.
    define var dFileSize    as dec no-undo.

    if source-procedure <> this-procedure then
        pcFileName = unix_normalizePath( pcFileName ).

    if unix_isRelativePath( pcFileName ) then
        pcFileName = unix_normalizePath( pro_cWorkDir + "/" + pcFileName ).



    /***
    input through value( 'stat -c %s "' + pcFileName + '" 2> /dev/null' ).
    import unformatted cFileSize.
    input close.
    ***/
    
    input through value( 'ls -l "' + pcFileName + '" 2> /dev/null' ).
    import ^ ^ ^ ^ cFileSize.
    input close.    

    assign
        dFileSize = ?
        dFileSize = decimal( cFileSize ) no-error.

    return dFileSize.

end function. /* unix_getBigFileSize */

procedure unix_getDiskFreeSpace:

    define input    param pcPath    as char no-undo.
    define output   param pdTotal   as dec no-undo.
    define output   param pdFree    as dec no-undo.
    define output   param pdUsed    as dec no-undo.

    define var cTempFile    as char no-undo.
    define var cEntry       as char no-undo.
    define var cLine        as char no-undo.

    cTempFile = unix_getTempFile( "", ".out" ).

    run unix_shell( 
        input "df -B 1 " + pcPath + " > " + cTempFile,
        input "silent,wait" ).



    if unix_iExitCode <> 0 then

    assign
        pdTotal = ?
        pdFree  = ?
        pdUsed  = ?.

    else do:
    
        input from value( cTempFile ).
        
            import unformatted cLine.
            import unformatted cLine.
            
            assign
                cLine                   = trim( cLine )
    
                cEntry                  = entry( 1, cLine, " " )
                entry( 1, cLine, " " )  = ""
                cLine                   = trim( cLine ).
                
            if cLine = "" then
                import unformatted cLine.
                
            assign
                cLine                   = trim( cLine )
                
                cEntry                  = entry( 1, cLine, " " )
                entry( 1, cLine, " " )  = ""
                pdTotal                 = dec( cEntry )
                cLine                   = trim( cLine ).
                
            if cLine = "" then
                import unformatted cLine.
                
            assign
                cLine                   = trim( cLine )
                
                cEntry                  = entry( 1, cLine, " " )
                entry( 1, cLine, " " )  = ""
                pdUsed                  = dec( cEntry )
                cLine                   = trim( cLine ).
                
            if cLine = "" then
                import unformatted cLine.
                
            assign
                cLine                   = trim( cLine )
                
                cEntry                  = entry( 1, cLine, " " )
                entry( 1, cLine, " " )  = ""
                pdFree                  = dec( cEntry )
                cLine                   = trim( cLine ).
    
        input close.
        
    end. /* else */

    os-delete value( cTempFile ).

end procedure. /* unix_getDiskFreeSpace */



procedure unix_shell:

    define input param pcScript     as char no-undo.
    define input param pcOptions    as char no-undo.

    define var cScriptFile  as char no-undo.
    define var cErrFile     as char no-undo.
    define var i            as int no-undo.

    unix_iExitCode = 0.

    pcScript = replace( replace( pcScript, 
        "~r~n", "~n" ), 
            "~r", "~n" ).

    do i = 1 to num-entries( pcScript, "~n" ):

        entry( i, pcScript, "~n" ) = trim( entry( i, pcScript, "~n" ) ).

        if entry( i, pcScript, "~n" ) = "proenv" then
           entry( i, pcScript, "~n" ) =

                  'DLC="' + pro_cDlc + '"~n'
                + 'WRKDIR="' + pro_cWorkDir + '"~n'
                + 'PATH=$DLC/bin:' + os-getenv( "PATH" ) + '~n'
                + 'export DLC WRKDIR PATH~n'
                + 'cd $WRKDIR~n'
                + 'ulimit -f unlimited'.

    end. /* do i */

    do i = 1 to num-entries( pcOptions ):
        entry( i, pcOptions ) = trim( entry( i, pcOptions ) ).
    end.

    /***
    if  lookup( "pause", pcScript, "~n" ) > 0
    and lookup( "silent", pcOptions ) > 0 then
 
        {slib/err_throw "'unix_silent_pause'" pcBatch}.
    ***/



    assign
        cScriptFile = unix_getTempFile( "", ".sh" )
        cErrFile    = unix_getTempFile( "", ".err" ).

    output to value( cScriptFile ).

        put unformatted
            pcScript skip.

        if lookup( "wait", pcOptions ) > 0 then
        put unformatted
            'echo $? > "' + cErrFile + '"' skip.

        put unformatted
            'rm -f "' + cScriptFile + '"' skip.

    output close.

    unix silent value( "chmod 777 " + cScriptFile + " 1>/dev/null 2>&1" ).

    if lookup( "silent", pcOptions ) > 0 then

    unix silent value( cScriptFile + ' 1>/dev/null 2>&1'
        + ( if lookup( "wait", pcOptions ) > 0 then '' else ' &' ) ).

    else

    unix value( cScriptFile
        + ( if lookup( "wait", pcOptions ) > 0 then '' else ' &' ) ).



    if lookup( "wait", pcOptions ) > 0 then do:

        input from value( cErrFile ).

        do on error undo, leave:
            import unix_iExitCode.
        end.

        input close.

        os-delete value( cErrFile ).

    end. /* wait */

end procedure. /* unix_shell */

