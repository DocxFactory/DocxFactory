
/**
 * slibinfozip.p -
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

{slib/slibinfozipfrwd.i "forward"}

{slib/slibinfozipprop.i}

&if "{&opsys}" begins "win" &then

    {slib/slibwin.i}

&else

    {slib/slibunix.i}

&endif

{slib/slibos.i}

{slib/slibdate.i}

{slib/slibpro.i}

{slib/sliberr.i}



define temp-table ttFile

    like infozip_ttFile.

define var cUtilZip     as char no-undo.
define var cUtilUnZip   as char no-undo.



on "close" of this-procedure do:

    delete procedure this-procedure.

end. /* on close */

procedure initializeProc:

    define input    param pcNoError as char no-undo.
    define output   param plOk      as log no-undo.

    define var cFileName as char no-undo.

    plOk = no.



    assign
        cUtilZip    = os_normalizePath( {&infozip_xUtilZip} )
        cUtilUnZip  = os_normalizePath( {&infozip_xUtilUnZip} ).

    if os_isRelativePath( cUtilZip ) then do:

        cFileName = os_getFullPath( cUtilZip ).

        if cFileName <> ? then
            cUtilZip = cFileName.

    end. /* os_isRelativePath */

    if os_isRelativePath( cUtilUnZip ) then do:

        cFileName = os_getFullPath( cUtilUnZip ).

        if cFileName <> ? then
            cUtilUnZip = cFileName.

    end. /* os_isRelativePath */



    if not os_isFileExists( cUtilZip ) then do:

        if pcNoError = "no-error" then do:

            apply "close" to this-procedure.
            return.

        end. /* no-error */

        else do:

            &if "{&opsys}" begins "win" &then

                message
                    'ZIP Util "' + cUtilZip + '" not found.'
                    skip(1)
                    'You may need to install the ZIP util first.'
                    skip(1)
                    'Look at the Standard Libraries BIN dir for instructions.'
                view-as alert-box.

            &else

                message
                    'Util Zip "' + cUtilZip + '" not found.'
                view-as alert-box.

            &endif

            quit.

        end. /* else */

    end. /* not os_isFileExists */

    if not os_isFileExists( cUtilUnZip ) then do:

        if pcNoError = "no-error" then do:

            apply "close" to this-procedure.
            return.

        end. /* no-error */

        else do:

            &if "{&opsys}" begins "win" &then

                message
                    'ZIP Util "' + cUtilUnZip + '" not found.'
                    skip(1)
                    'You may need to install the ZIP util first.'
                    skip(1)
                    'Look at the Standard Libraries BIN dir for instructions.'
                view-as alert-box.

            &else

                message
                    'ZIP Util "' + cUtilUnZip + '" not found.'
                view-as alert-box.

            &endif

            quit.

        end. /* else */

    end. /* not os_isFileExists */



    plOk = yes.

end procedure. /* initializeProc */



procedure infozip_add:

    define input param pcArchive    as char no-undo.
    define input param pcFiles      as char no-undo.
    define input param pcInputDir   as char no-undo.

    run infozip_addAdvanced(
        input pcArchive,
        input pcFiles,
        input pcInputDir,
        input "silent,wait" ).

end procedure. /* infozip_add */

procedure infozip_addConsole:

    define input param pcArchive    as char no-undo.
    define input param pcFiles      as char no-undo.
    define input param pcInputDir   as char no-undo.

    run infozip_addAdvanced(
        input pcArchive,
        input pcFiles,
        input pcInputDir,
        input "wait" ).

end procedure. /* infozip_addConsole */

procedure infozip_addAdvanced:

    define input param pcArchive    as char no-undo.
    define input param pcFiles      as char no-undo. /* relative input dir */
    define input param pcInputDir   as char no-undo.
    define input param pcOptions    as char no-undo.

    define var cFiles   as char no-undo.
    define var cCmd     as char no-undo.

    define var str      as char no-undo.
    define var i        as int no-undo.

    assign
        pcArchive   = os_normalizePath( pcArchive )
        pcInputDir  = os_normalizePath( pcInputDir ).

    cFiles = "".

    do i = 1 to num-entries( pcFiles, "|" ):

        str = infozip_normalizePath( entry( i, pcFiles, "|" ) ).

        if str <> "" then

        cFiles = cFiles 
            + ( if cFiles <> "" then "|" else "" )
            + str.

    end. /* 1 to num-entries */

    pcFiles = cFiles.

    if pcFiles = "" then
       pcFiles = ?.

    if pcFiles = ? then
       pcFiles = "*".

    if pcInputDir = "" then
       pcInputDir = ?.

    if pcInputDir = ? then
       pcInputDir = pro_cWorkDir.

    if os_isRelativePath( pcInputDir ) then
       pcInputDir = os_normalizePath( pro_cWorkDir + "/" + pcInputDir ).

    if os_isRelativePath( pcArchive ) then
       pcArchive = os_normalizePath( pro_cWorkDir + "/" + pcArchive ).

    if not os_isDirExists( pcInputDir ) then
        {slib/err_throw "'dir_not_found'" pcInputDir}.



    cFiles = "".

    do i = 1 to num-entries( pcFiles, "|" ):

        str = entry( i, pcFiles, "|" ).

        cFiles = cFiles 
            + ( if cFiles <> "" then " " else "" )
            + ( if index( str, ' ' ) > 0 then '"' + str + '"' else str ).

    end. /* 1 to num-entries */

    cCmd = replace( replace( replace( {&infozip_xCmdAdd},

        "%zip%",        cUtilZip ),
        "%archive%",    '"' + pcArchive + '"' ), 
        "%files%",      cFiles ).

    &if "{&opsys}" begins "win" &then

        if win_isUncPath( pcInputDir ) then

        run win_batch(
            input 'pushd "' + pcInputDir + '"~n'
                + cCmd,

            input pcOptions ).

        else

        run win_batch(
            input 'cd /d "' + pcInputDir + '"~n'
                + cCmd,

            input pcOptions ).

        if win_iErrorLevel <> 0 then
            {slib/err_throw "'infozip_util_error'" cCmd "'Exit Code ' + string( win_iErrorLevel )"}.
          
    &else
    
        run unix_shell( 
            input 'ulimit -f unlimited ~n'
                + 'cd "' + pcInputDir + '"~n'
                + cCmd,

            input pcOptions ).

        if unix_iExitCode <> 0 then
            {slib/err_throw "'infozip_util_error'" cCmd "'Exit Code ' + string( unix_iExitCode )"}.
 
    &endif

end procedure. /* infozip_add */



procedure infozip_del:

    define input param pcArchive    as char no-undo.
    define input param pcFiles      as char no-undo.

    define var cFiles   as char no-undo.
    define var cCmd     as char no-undo.

    define var str      as char no-undo.
    define var i        as int no-undo.

    pcArchive = os_normalizePath( pcArchive ).

    cFiles = "".

    do i = 1 to num-entries( pcFiles, "|" ):

        str = infozip_normalizePath( entry( i, pcFiles, "|" ) ).

        if str <> "" then

        cFiles = cFiles 
            + ( if cFiles <> "" then "|" else "" )
            + str.

    end. /* 1 to num-entries */

    pcFiles = cFiles.

    if pcFiles = "" then
       pcFiles = ?.

    if pcFiles = ? then
       pcFiles = "*".

    if os_isRelativePath( pcArchive ) then
        pcArchive = os_normalizePath( pro_cWorkDir + "/" + pcArchive ).

    if not os_isFileExists( pcArchive ) then
        {slib/err_throw "'infozip_archive_not_exists'" pcArchive}.



    cFiles = "".

    do i = 1 to num-entries( pcFiles, "|" ):

        cFiles = cFiles 
            + ( if cFiles <> "" then " " else "" )
            + '"' + replace( entry( i, pcFiles, "|" ), "[", "[[]" ) + '"'.

    end. /* 1 to num-entries */

    cCmd = replace( replace( replace( {&infozip_xCmdDel},

        "%zip%",        cUtilZip ),
        "%archive%",    '"' + pcArchive + '"' ), 
        "%files%",      cFiles ).

    &if "{&opsys}" begins "win" &then

        run win_batch( 
            input cCmd,
            input "silent,wait" ).

        if win_iErrorLevel <> 0 then
            {slib/err_throw "'infozip_util_error'" cCmd "'Exit Code ' + string( win_iErrorLevel )"}.
          
    &else

        run unix_shell( 
            input "ulimit -f unlimited ~n"
                + cCmd,
            input "silent,wait" ).

        if unix_iExitCode <> 0 then
            {slib/err_throw "'infozip_util_error'" cCmd "'Exit Code ' + string( unix_iExitCode )"}.

    &endif

end procedure. /* infozip_del */

procedure infozip_extract:

    define input param pcArchive    as char no-undo.    
    define input param pcFiles      as char no-undo. /* the files in the archive */
    define input param pcOutDir     as char no-undo.

    run infozip_extractAdvanced(
        input pcArchive,
        input pcFiles,
        input pcOutDir,
        input "silent,wait" ).

end procedure. /* infozip_extract */

procedure infozip_extractConsole:

    define input param pcArchive    as char no-undo.    
    define input param pcFiles      as char no-undo. /* the files in the archive */
    define input param pcOutDir     as char no-undo.

    run infozip_extractAdvanced(
        input pcArchive,
        input pcFiles,
        input pcOutDir,
        input "wait" ).

end procedure. /* infozip_extractConsole */

procedure infozip_extractAdvanced:

    define input param pcArchive    as char no-undo.    
    define input param pcFiles      as char no-undo. /* the files in the archive */
    define input param pcOutDir     as char no-undo.
    define input param pcOptions    as char no-undo.

    define var cFiles   as char no-undo.
    define var cCmd     as char no-undo.

    define var str      as char no-undo.
    define var i        as int no-undo.

    assign
        pcArchive   = os_normalizePath( pcArchive )
        pcOutDir    = os_normalizePath( pcOutDir ).

    cFiles = "".

    do i = 1 to num-entries( pcFiles, "|" ):

        str = infozip_normalizePath( entry( i, pcFiles, "|" ) ).

        if str <> "" then

        cFiles = cFiles 
            + ( if cFiles <> "" then "|" else "" )
            + str.

    end. /* 1 to num-entries */

    pcFiles = cFiles.

    if pcFiles = "" then
       pcFiles = ?.

    if pcFiles = ? then
       pcFiles = "*".

    if pcOutDir = "" then
       pcOutDir = ?.

    if pcOutDir = ? then
       pcOutDir = pro_cWorkDir.

    if os_isRelativePath( pcOutDir ) then
       pcOutDir = os_normalizePath( pro_cWorkDir + "/" + pcOutDir ).

    if os_isRelativePath( pcArchive ) then
       pcArchive = os_normalizePath( pro_cWorkDir + "/" + pcArchive ).

    if substr( pcOutDir, length( pcOutDir ), 1 ) = {&os_xDirSeparator} then
       substr( pcOutDir, length( pcOutDir ), 1 ) = "".



    if not os_isFileExists( pcArchive ) then
        {slib/err_throw "'infozip_archive_not_exists'" pcArchive}.

    if not os_isDirExists( pcOutDir ) then do:

        run os_createDir( pcOutDir ).

        if not os_isDirExists( pcOutDir ) then
            {slib/err_throw "'dir_create_failed'" pcOutDir}.

    end. /* not dirExists */



    cFiles = "".

    do i = 1 to num-entries( pcFiles, "|" ):

        cFiles = cFiles 
            + ( if cFiles <> "" then " " else "" )
            + '"' + replace( entry( i, pcFiles, "|" ), "[", "[[]" ) + '"'.

    end. /* 1 to num-entries */

    cCmd = replace( replace( replace( replace( {&infozip_xCmdExtract},

        "%unzip%",      cUtilUnZip ),
        "%archive%",    '"' + pcArchive + '"' ),
        "%outdir%",     '"' + pcOutDir + '"' ),
        "%files%",      cFiles ).

    &if "{&opsys}" begins "win" &then

        run win_batch(
            input cCmd,
            input pcOptions ).

        if win_iErrorLevel <> 0 then
            {slib/err_throw "'infozip_util_error'" cCmd "'Exit Code ' + string( win_iErrorLevel )"}.

    &else

        run unix_shell( 
            input "ulimit -f unlimited ~n"
                + cCmd,
            input pcOptions ).

        if unix_iExitCode <> 0 then
            {slib/err_throw "'infozip_util_error'" cCmd "'Exit Code ' + string( unix_iExitCode )"}.

    &endif

end procedure. /* infozip_extractAdvanced */

procedure infozip_list:

    define input    param pcArchive    as char no-undo.
    define input    param pcFiles      as char no-undo.
    define output   param table for infozip_ttFile.

    define var cLength      as char no-undo.
    define var cDateTime    as char no-undo.
    define var cName        as char no-undo.

    define var tDate        as date no-undo.
    define var iMTime       as int no-undo.
    define var iTimeZone    as int no-undo.

    define var cTempFile    as char no-undo.
    define var cFiles       as char no-undo.
    define var cCmd         as char no-undo.

    define var str          as char no-undo.
    define var i            as int no-undo.

    empty temp-table infozip_ttFile.

    pcArchive = os_normalizePath( pcArchive ).

    cFiles = "".

    do i = 1 to num-entries( pcFiles, "|" ):

        str = infozip_normalizePath( entry( i, pcFiles, "|" ) ).

        if str <> "" then

        cFiles = cFiles 
            + ( if cFiles <> "" then "|" else "" )
            + str.

    end. /* 1 to num-entries */

    pcFiles = cFiles.

    if pcFiles = "" then
       pcFiles = ?.

    if pcFiles = ? then
       pcFiles = "*".

    if os_isRelativePath( pcArchive ) then
       pcArchive = os_normalizePath( pro_cWorkDir + "/" + pcArchive ).

    if not os_isFileExists( pcArchive ) then
        {slib/err_throw "'infozip_archive_not_exists'" pcArchive}.



    cFiles = "".

    do i = 1 to num-entries( pcFiles, "|" ):

        cFiles = cFiles 
            + ( if cFiles <> "" then " " else "" )
            + '"' + replace( entry( i, pcFiles, "|" ), "[", "[[]" ) + '"'.

    end. /* 1 to num-entries */

    assign
        cTempFile   = os_getTempFile( "", ".out" )

        cCmd        = replace( replace( replace( {&infozip_xCmdList},

            "%unzip%",      cUtilUnZip ),
            "%archive%",    '"' + pcArchive + '"' ),
            "%files%",      cFiles )

        cCmd        = cCmd + ' > "' + cTempFile + '"'.

    &if "{&opsys}" begins "win" &then

        run win_batch(
            input cCmd,
            input "wait,silent" ).

    &elseif "{&opsys}" begins "unix" &then

        run unix_shell( 
            input "ulimit -f unlimited ~n"
                + cCmd,
            input "wait,silent" ).

    &endif



    if os_isFileExists( cTempFile ) then do:

        input from value( cTempFile ).
        
        repeat:
        
            import unformatted str.
            
            assign
                str                     = trim( str )
                entry( 1, str, " " )    = ""
                
                str                     = trim( str )
                entry( 1, str, " " )    = ""
                
                str                     = trim( str )
                entry( 1, str, " " )    = ""
                
                str                     = trim( str )
                cLength                 = entry( 1, str, " " )
                entry( 1, str, " " )    = ""
                
                str                     = trim( str )
                entry( 1, str, " " )    = ""
                
                str                     = trim( str )
                entry( 1, str, " " )    = ""
                
                str                     = trim( str )
                cDateTime               = entry( 1, str, " " )
                entry( 1, str, " " )    = ""
                
                str                     = trim( str )
                cName                   = entry( 1, str, " " ).

                run date_Str2Date(
                    input   cDateTime,
                    input   "yyyymmdd.hhiiss",
    
                    output  tDate,
                    output  iMTime,
                    output  iTimeZone ).

                create infozip_ttFile.
                assign
                    infozip_ttFile.cPath    = cName
                    infozip_ttFile.tDate    = tDate
                    infozip_ttFile.iTime    = iMTime / 1000
                    infozip_ttFile.dLength  = dec( cLength ).
 
        end. /* repeat */
        
        input close. /* cTempFile */

        os-delete value( cTempFile ).

    end. /* fileexists */

end procedure. /* infozip_list */



/*
note that shelling out is a relatively time expensive operation and it may take 
as much time to check if a file exists as extracting it from the archive. 
*/

function infozip_isFileExists returns log ( pcArchive as char, pcFiles as char ):

    empty temp-table ttFile.

    run infozip_list(
        input   pcArchive,
        input   pcFiles,
        output  table ttFile ).

    if can-find( first ttFile ) then
        return yes.

    return no.

end function. /* infozip_isFileExists */

function infozip_normalizePath returns char ( pcPath as char ):

    pcPath = replace( trim( pcPath ), "~\", "/" ).

    do while index( pcPath, "//" ) <> 0:
        pcPath = replace( pcPath, "//", "/" ).
    end.

    if substr( pcPath, 1, 1 ) = "/" then
       substr( pcPath, 1, 1 ) = "".

    return pcPath.

end function. /* infozip_normalizePath */
