
/**
 * slibpro.p -
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

{slib/slibprofrwd.i forward}

{slib/slibproprop.i}

{slib/slibstr.i}

{slib/sliberr.i}


 
function isRelativePath returns log private     ( pcPath as char ) forward.
function normalizePath  returns char private    ( pcPath as char ) forward.        



on "close" of this-procedure do:

    delete procedure this-procedure.

end. /* close */

procedure initializeProc:

    define var cVersion as char no-undo.
    define var str      as char no-undo.
    define var i        as int no-undo.
    define var j        as int no-undo.

    &if "{&opsys}" = "unix" &then

        assign
           pro_cDlc    = os-getenv( "DLC" )
           pro_cProCfg = os-getenv( "PROCFG" ).
        
        if pro_cProCfg = "" or pro_cProCfg = ? then
           pro_cProCfg = pro_cDlc + "/progress.cfg".

    &else

        get-key-value section "Startup" key "DLC"       value pro_cDlc.
        get-key-value section "Startup" key "PROCFG"    value pro_cProCfg.

        if pro_cDlc = "" or pro_cDlc = ? then
           pro_cDlc = os-getenv( "DLC" ).

        if pro_cProCfg = "" or pro_cProCfg = ? then
           pro_cProCfg = os-getenv( "PROCFG" ).

        if pro_cProCfg = "" or pro_cProCfg = ? then
           pro_cProCfg = pro_cDlc + "~\progress.cfg".

    &endif



    file-info:file-name = ".".
    pro_cWorkDir = file-info:full-pathname.

    &if "{&opsys}" = "unix" &then

        file-info:file-name = session:temp-dir.
        pro_cTempFullDir = file-info:full-pathname + "/".

    &else

        file-info:file-name = session:temp-dir.
        pro_cTempFullDir = file-info:full-pathname + "~\".

    &endif

    pro_cFullPropath = "".
    do i = 1 to num-entries( propath ).

        file-info:file-name = entry( i, propath ).
        if file-info:full-pathname <> ? then

            pro_cFullPropath = pro_cFullPropath

                + ( if pro_cFullPropath <> "" then "," else "" )
                + file-info:full-pathname.

    end. /* 1 to num-entries */



    pro_cFullProversion = proversion.

    file-info:file-name = pro_cDlc + "/version".
    if file-info:full-pathname <> ? then do:

        input from value( file-info:full-pathname ).
        import unformatted cVersion.
        input close.

        j = num-entries( cVersion, " " ).
        do i = 1 to j:

            str = entry( i, cVersion, " " ).
            if str begins proversion then do:

                pro_cFullProversion = str.
                leave.

            end. /* begins proversion */

        end. /* 1 to j */

    end. /* full-pathname <> ? */

    if index( pro_cFullProversion, "." ) = 2 then
        pro_cFullProversion = "0" + pro_cFullProversion.

    if length( pro_cFullProversion ) = 5 then
        pro_cFullProversion = pro_cFullProversion + "00".

end procedure. /* initializeProc */



procedure pro_appendEoln:

    define input param pcSourceFile as char no-undo.
    define input param pcTargetFile as char no-undo.

    define var iFileSize    as int no-undo.
    define var lEoln        as log no-undo.

    define var str          as char no-undo.
    define var ptr          as memptr no-undo.

    file-info:file-name = pcSourceFile.
    if file-info:full-pathname = ? then

        {slib/err_throw "'file_not_found'" pcSourceFile}.

    assign
        pcSourceFile    = file-info:full-pathname
        iFileSize       = file-info:file-size.

    if source-procedure <> this-procedure then
        pcTargetFile    = normalizePath( ( if isRelativePath( pcTargetFile ) then pro_cTempFullDir else "" ) + pcTargetFile ).



    lEoln = no.

    if iFileSize > 0 then do:

        {slib/err_try}:

            set-size( ptr ) = 3.

            input from value( pcSourceFile ) binary no-convert.
            seek input to max( iFileSize - 2, 1 ).
            import ptr.
            input close.

            str = get-string( ptr, 1 ).

            if str = "~r~n"
            or substr( str, length( str ), 1 ) = "~r"
            or substr( str, length( str ), 1 ) = "~n" then

                lEoln = yes.

        {slib/err_finally}:

            set-size( ptr ) = 0.

        {slib/err_end}.

    end. /* iFileSize > 0 */



    if pcTargetFile <> pcSourceFile then do:

        os-copy value( pcSourceFile )
                value( pcTargetFile ) {slib/err_os-error}.

    end. /* pcTargetFile <> pcSourceFile */

    if not lEoln then do:

        output to value( pcTargetFile ) append.
        put skip(1).
        output close.

    end. /* not lEoln */

end procedure. /* pro_appendEoln */



function pro_getStackTrace returns char ( piStartLevel as int, plHideDlcProc as log ):

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

end function. /* pro_getStackTrace */



function pro_getRunFile returns char ( pcFileName as char ):

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



    assign
        i = r-index( pcFileName, "." )

        cRFileName = ( if i > 0 then substr( pcFileName, 1, i - 1 ) else pcFileName ) + ".r"
        cPFileName = pcFileName.
    
    
    
    repeat i = 1 to num-entries( propath ):

        cDir = normalizePath( entry( i, propath ) ).

        if isRelativePath( cDir ) then
            cDir = normalizePath( pro_cWorkDir + "/" + cDir ).



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

end function. /* pro_getRunFile */

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



procedure pro_parseQueryPhrase:

    define input    param pcQueryPhrase         as char no-undo.
    define output   param pcQuery               as char no-undo.
    define output   param pcRecordPhraseList    as char no-undo.
    define output   param pcQueryTuning         as char no-undo.
    define output   param plBreak               as log no-undo.
    define output   param pcByExpList           as char no-undo.
    define output   param pcByDescendList       as char no-undo.
    define output   param plIndexedReposition   as log no-undo.
    define output   param pcMaxRows             as char no-undo.

    define var cKeywordPhraseList   as char no-undo.
    define var iKeywordPhraseCnt    as int no-undo.
    define var cKeywordPhrase       as char no-undo.
    define var iKeywordPhrase       as int no-undo.
    define var cKeyword             as char no-undo.
    define var cPhrase              as char no-undo.
    define var cExp                 as char no-undo.
    define var cDescend             as char no-undo.

    define var str                  as char no-undo.
    define var idx                  as int no-undo.
    define var i                    as int no-undo.
    define var j                    as int no-undo.

    assign
        pcQuery             = ?
        pcRecordPhraseList  = ?
        pcQueryTuning       = ?
        plBreak             = no
        pcByExpList         = ?
        pcByDescendList     = ?
        plIndexedReposition = no
        pcMaxRows           = ?.



    assign
       cKeywordPhraseList   = pro_parseKeywordPhraseList(

            input pcQueryPhrase,
            input "for,~~,|preselect,~~,|query-tuning|break|by,by|indexed-reposition|max-rows" )

       iKeywordPhraseCnt    = num-entries( cKeywordPhraseList, chr(1) ).

    if cKeywordPhraseList = "" then
        {slib/err_throw "'pro_unable_to_understand'" pcQueryPhrase}.



    assign
        cKeywordPhrase      = entry( 1, cKeywordPhraseList, chr(1) )
        cKeyword            = entry( 1, cKeywordPhrase, chr(2) )
        cPhrase             = entry( 2, cKeywordPhrase, chr(2) ).

    if cKeyword <> "for" and cKeyword <> "preselect" then
        {slib/err_throw "'pro_unable_to_understand'" pcQueryPhrase}.

    if cPhrase = "" then
        {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

    assign
        pcQuery             = cKeyword
        pcRecordPhraseList  = replace( cPhrase, chr(3), chr(1) ).



    do iKeywordPhrase = 2 to iKeywordPhraseCnt:

        assign
            cKeywordPhrase  = entry( iKeywordPhrase, cKeywordPhraseList, chr(1) )
            cKeyword        = entry( 1, cKeywordPhrase, chr(2) )
            cPhrase         = entry( 2, cKeywordPhrase, chr(2) ).

        case cKeyword:

            when "query-tuning" then do:

                if pcQueryTuning <> ? then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                if cPhrase = "" then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                if not cPhrase matches "(*)" then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                pcQueryTuning = cPhrase.

            end. /* query-tuning */

            when "break" then do:

                if plBreak <> no then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                if cPhrase <> "" then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                plBreak = yes.

            end. /* break */

            when "by" then do:

                if pcByExpList <> ? then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                if cPhrase = "" then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                j = num-entries( cPhrase, chr(3) ). do i = 1 to j:

                    cExp = entry( i, cPhrase, chr(3) ).

                    if cExp = "" then
                        {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                    cDescend = "".

                    idx = r-index( cExp, " " ).
                    if idx > 0 then

                        cDescend = substr( cExp, idx + 1 ).

                    if  "descending" begins cDescend
                    and length( cDescend ) >= 4 then do:

                    &if {&pro_xProversion} >= "10" &then
                        if compare( "DESCENDING", "begins", cDescend, "raw" ) then
                        cDescend = "DESCENDING".

                        else
                        if compare( "descending", "begins", cDescend, "raw" ) then
                        cDescend = "descending".

                        else
                    &endif

                        cDescend = "descending".

                        cExp = right-trim( substr( cExp, 1, idx - 1 ) ).

                    end. /* idx > 0 */

                    if pcByExpList = ? then
                    assign
                        pcByExpList     = cExp
                        pcByDescendList = cDescend.

                    else
                    assign
                        pcByExpList     = pcByExpList       + chr(1) + cExp
                        pcByDescendList = pcByDescendList   + chr(1) + cDescend.

                end. /* 1 to iByCnt */

            end. /* by */

            when "indexed-reposition" then do:

                if plIndexedReposition <> no then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                if cPhrase <> "" then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                plIndexedReposition = yes.

            end. /* indexed-reposition */

            when "max-rows" then do:

                if pcMaxRows <> ? then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                if cPhrase = "" then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                pcMaxRows = cPhrase.

            end. /* max-rows */

        end case. /* cKeyword */

    end. /* 2 to iKeywordPhraseCnt */

end procedure. /* pro_parseQueryPhrase */

procedure pro_parseRecordPhrase:

    define input    param pcRecordPhrase    as char no-undo.
    define output   param pcJoin            as char no-undo.
    define output   param pcBuffer          as char no-undo.
    define output   param pcFields          as char no-undo.
    define output   param pcExcept          as char no-undo.
    define output   param plLeft            as log no-undo.
    define output   param plOuterJoin       as log no-undo.
    define output   param pcOf              as char no-undo.
    define output   param pcWhere           as char no-undo.
    define output   param pcUseIndex        as char no-undo.
    define output   param pcLock            as char no-undo.
    define output   param plNoPrefetch      as log no-undo.

    define var cKeywordPhraseList   as char no-undo.
    define var iKeywordPhraseCnt    as int no-undo.
    define var cKeywordPhrase       as char no-undo.
    define var iKeywordPhrase       as int no-undo.
    define var cKeyword             as char no-undo.
    define var cPhrase              as char no-undo.

    assign
        pcJoin          = ?
        pcBuffer        = ?
        pcFields        = ?
        pcExcept        = ?
        plLeft          = no
        plOuterJoin     = no
        pcOf            = ?
        pcWhere         = ?
        pcUseIndex      = ?
        pcLock          = ?
        plNoPrefetch    = no.



    assign
       cKeywordPhraseList   = pro_parseKeywordPhraseList(

            input pcRecordPhrase,
            input "each|first|last|left|fields|except|outer-join|of|where|use-index|exclusive-lock|exclusive|share-lock|share|no-lock|no-prefetch" )

       iKeywordPhraseCnt    = num-entries( cKeywordPhraseList, chr(1) ).

    if cKeywordPhraseList = "" then
        {slib/err_throw "'pro_unable_to_understand'" pcRecordPhrase}.



    assign
       cKeywordPhrase   = entry( 1, cKeywordPhraseList, chr(1) )
       cKeyword         = entry( 1, cKeywordPhrase, chr(2) )
       cPhrase          = entry( 2, cKeywordPhrase, chr(2) ).

    if cKeyword <> "each" and cKeyword <> "first" and cKeyword <> "last" then
        {slib/err_throw "'pro_unable_to_understand'" pcRecordPhrase}.

    if cPhrase = "" then
        {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

    assign
        pcJoin      = cKeyword
        pcBuffer    = cPhrase.



    do iKeywordPhrase = 2 to iKeywordPhraseCnt:

        assign
            cKeywordPhrase  = entry( iKeywordPhrase, cKeywordPhraseList, chr(1) )
            cKeyword        = entry( 1, cKeywordPhrase, chr(2) )
            cPhrase         = entry( 2, cKeywordPhrase, chr(2) ).

        case cKeyword:

            when "fields" then do:

                if iKeywordPhrase > 2 then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                if cPhrase = "" then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                if not cPhrase matches "(*)" then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                assign
                    pcFields = pro_unparenthesis( cPhrase )
                    pcFields = pro_parseWordList( pcFields, yes, no ).

            end. /* fields */

            when "except" then do:

                if iKeywordPhrase > 2 then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                if cPhrase = "" then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                if not cPhrase matches "(*)" then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                assign
                    pcExcept = pro_unparenthesis( cPhrase )
                    pcExcept = pro_parseWordList( pcExcept, yes, no ).

            end. /* except */

            when "left" then do:

                if plLeft <> no then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                if cPhrase <> "" then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                if not ( iKeywordPhrase + 1 <= iKeywordPhraseCnt
                     and entry( 1, entry( iKeywordPhrase + 1, cKeywordPhraseList, chr(1) ), chr(2) ) = "outer-join" ) then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                plLeft = yes.

            end. /* left */

            when "outer-join" then do:

                if plOuterJoin <> no then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                if cPhrase <> "" then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                plOuterJoin = yes.

            end. /* outer-join */

            when "of" then do:

                if pcOf <> ? then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                if cPhrase = "" then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                pcOf = cPhrase.

            end. /* of */

            when "where" then do:

                if pcWhere <> ? then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                pcWhere = cPhrase.

            end. /* where */

            when "use-index" then do:

                if pcUseIndex <> ? then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                if cPhrase = "" then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                pcUseIndex = cPhrase.

            end. /* use-index */

            when "exclusive-lock" or
            when "exclusive" or
            when "share-lock" or 
            when "share" or
            when "no-lock" then do:

                if pcLock <> ? then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                if cPhrase <> "" then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

            &if {&pro_xProversion} >= "10" &then
                if compare( cKeyword, "=", "SHARE", "raw" ) then
                pcLock = "SHARE-LOCK".

                else
                if compare( cKeyword, "=", "share", "raw" ) then
                pcLock = "share-lock".

                else
            &endif

                if cKeyword = "share" then
                pcLock = "share-lock".

            &if {&pro_xProversion} >= "10" &then
                else
                if compare( cKeyword, "=", "EXCLUSIVE", "raw" ) then
                pcLock = "EXCLUSIVE-LOCK".

                else
                if compare( cKeyword, "=", "exclusive", "raw" ) then
                pcLock = "exclusive-lock".
            &endif

                else
                if pcLock = "exclusive" then
                pcLock = "exlcusive-lock".

                else
                pcLock = cKeyword.

            end. /* exclusive-lock */

            when "no-prefetch" then do:

                if plNoPrefetch <> no then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                if cPhrase <> "" then
                    {slib/err_throw "'pro_unable_to_understand'" cKeyword}.

                plNoPrefetch = yes.

            end. /* no-prefetch */

        end case. /* cKeyword */

    end. /* 2 to iKeywordPhraseCnt */

    if pcWhere = "" then
       pcWhere = ?.

end procedure. /* pro_parseRecordPhrase */

procedure pro_getBufferField:

    define input    param phQuery   as handle no-undo.
    define input    param pcField   as char no-undo.
    define output   param phField   as handle no-undo.
    define output   param piExtent  as int no-undo.

    pcField = trim( pcField ).

    if not valid-handle( phQuery ) then
        {slib/err_throw "'pro_invalid_query'"}.

    case phQuery:type:

        when "buffer" then
        run getBufferFieldByBuffer(
            input   phQuery,
            input   pcField,
            output  phField,
            output  piExtent ).

        when "query" then
        run getBufferFieldByQuery(
            input   phQuery,
            input   pcField,
            output  phField,
            output  piExtent ).

        otherwise
        {slib/err_throw "'pro_invalid_query'"}.

    end case. /* phQuery:type */

end procedure. /* pro_getBufferField */

procedure getBufferFieldByBuffer private:

    define input    param phBuffer  as handle no-undo.
    define input    param pcField   as char no-undo.
    define output   param phField   as handle no-undo.
    define output   param piExtent  as int no-undo.

    define var str  as char no-undo.
    define var i    as int no-undo.
    define var j    as int no-undo.

    assign
        phField     = ?
        piExtent    = 0.

    if pcField matches "*[*]" then do:

        assign
           i        = index ( pcField, "[" )
           j        = index ( pcField, "]" )

           str      = trim( substr( pcField, i + 1, ( j - 1 ) - ( i + 1 ) + 1 ) )
           pcField  = trim( substr( pcField, 1, i - 1 ) ).

        assign
           piExtent = ?
           piExtent = int( str ) no-error.

        if piExtent = ? then
            {slib/err_throw "'pro_invalid_extent'" pcField str}.

        if piExtent < 0 then
            {slib/err_throw "'pro_invalid_extent'" pcField "string( piExtent )"}.

    end. /* matches "*[*]" */



    case num-entries( pcField, "." ):

        when 1 then do:

            phField = phBuffer:buffer-field( pcField ) no-error.

        end. /* when 1 */

        when 2 then do:

            if phBuffer:name = entry( 1, pcField, "." ) then

                phField = phBuffer:buffer-field( entry( 2, pcField, "." ) ) no-error.

        end. /* when 2 */

        when 3 then do:

            if  phBuffer:dbname = entry( 1, pcField, "." )
            and phBuffer:name   = entry( 2, pcField, "." ) then

                phField = phBuffer:buffer-field( entry( 3, pcField, "." ) ) no-error.

        end. /* when 3 */

        otherwise
        {slib/err_throw "'pro_field_not_found'" pcField}.

    end case. /* num-entries( "." ) */

    if phField = ? then
        {slib/err_throw "'pro_field_not_found'" pcField}.

    if piExtent = 0 and phField:extent > 0 then
        {slib/err_throw "'pro_invalid_extent'" pcField "string( piExtent )"}.

    if piExtent > phField:extent then
        {slib/err_throw "'pro_invalid_extent'" pcField "string( piExtent )"}.

end procedure. /* getBufferFieldByBuffer */

procedure getBufferFieldByQuery private:

    define input    param phQuery   as handle no-undo.
    define input    param pcField   as char no-undo.
    define output   param phField   as handle no-undo.
    define output   param piExtent  as int no-undo.

    define var cDatabase    as char no-undo.
    define var cBuffer      as char no-undo.
    define var hBuffer      as handle no-undo.
    define var iBuffer      as int no-undo.
    define var cField       as char no-undo.

    define var hndl         as handle no-undo.
    define var str          as char no-undo.
    define var i            as int no-undo.
    define var j            as int no-undo.

    assign
        phField     = ?
        piExtent    = 0.

    if pcField matches "*[*]" then do:

        assign
           i        = index ( pcField, "[" )
           j        = index ( pcField, "]" )

           str      = trim( substr( pcField, i + 1, ( j - 1 ) - ( i + 1 ) + 1 ) )
           pcField  = trim( substr( pcField, 1, i - 1 ) ).

        assign
           piExtent = ?
           piExtent = int( str ) no-error.

        if piExtent = ? then
            {slib/err_throw "'pro_invalid_extent'" pcField str}.

        if piExtent < 0 then
            {slib/err_throw "'pro_invalid_extent'" pcField "string( piExtent )"}.

    end. /* matches "*[*]" */



    case num-entries( pcField, "." ):

        when 1 then do:

            do iBuffer = 1 to phQuery:num-buffers:

                hBuffer = phQuery:get-buffer-handle( iBuffer ).
                hndl = hBuffer:buffer-field( pcField ) no-error.

                if hndl <> ? then do:

                    if phField <> ? then
                        {slib/err_throw "'pro_ambiguous_field_name'" pcField}.

                    phField = hndl.

                end. /* hndl <> ? */

            end. /* do iBuffer */

        end. /* when 1 */

        when 2 then do:

            assign
                cBuffer = entry( 1, pcField, "." )
                cField  = entry( 2, pcField, "." ).

            do iBuffer = 1 to phQuery:num-buffers:

                hBuffer = phQuery:get-buffer-handle( iBuffer ).

                if hBuffer:name <> cBuffer then
                    next.

                hndl = hBuffer:buffer-field( cField ) no-error.

                if hndl <> ? then do:

                    if phField <> ? then
                        {slib/err_throw "'pro_ambiguous_field_name'" pcField}.

                    phField = hndl.

                end. /* hndl <> ? */

            end. /* do iBuffer */

        end. /* when 2 */

        when 3 then do:

            assign
                cDatabase   = entry( 1, pcField, "." )
                cBuffer     = entry( 2, pcField, "." )
                cField      = entry( 3, pcField, "." ).

            do iBuffer = 1 to phQuery:num-buffers:

                hBuffer = phQuery:get-buffer-handle( iBuffer ).

                if hBuffer:dbname   <> cDatabase
                or hBuffer:name     <> cBuffer then
                    next.

                phField = hBuffer:buffer-field( cField ) no-error.

                if phField <> ? then
                    leave.

            end. /* do iBuffer */

        end. /* when 3 */

        otherwise
        {slib/err_throw "'pro_field_not_found'" pcField}.

    end case. /* num-entries( "." ) */

    if phField = ? then
        {slib/err_throw "'pro_field_not_found'" pcField}.

    if piExtent = 0 and phField:extent > 0 then
        {slib/err_throw "'pro_invalid_extent'" pcField "string( piExtent )"}.

    if piExtent > phField:extent then
        {slib/err_throw "'pro_invalid_extent'" pcField "string( piExtent )"}.

end procedure. /* getBufferFieldByQuery */



function pro_getBufferFieldValue returns char ( pcBufferFieldHandle as char ):

    define var hndl as handle no-undo.

    hndl = widget-handle( pcBufferFieldHandle ).

    return string( hndl:buffer-value ).

end function. /* pro_getBufferFieldValue */



function pro_compareTable returns log ( phSourceBuffer as handle, phTargetBuffer as handle ):

    define var hSourceField as handle no-undo.
    define var hTargetField as handle no-undo.
    define var i            as int no-undo.

    if not valid-handle( phSourceBuffer )
    or phSourceBuffer:type <> "buffer" then

        {slib/err_throw "'pro_invalid_buffer'"}.

    if not valid-handle( phTargetBuffer )
    or phTargetBuffer:type <> "buffer" then

        {slib/err_throw "'pro_invalid_buffer'"}.

    if phTargetBuffer:num-fields
    <> phSourceBuffer:num-fields then

        return no.

    do i = 1 to phSourceBuffer:num-fields:

        assign
           hSourceField = phSourceBuffer:buffer-field( i )
           hTargetField = phTargetBuffer:buffer-field( hSourceField:name ) no-error.

        if hTargetField = ?
        or hTargetField:extent <> hSourceField:extent then

            return no.

    end. /* do iSourceField */

end function. /* pro_compareTable */



procedure pro_parseParamList:

    define input    param pcParamListExp    as char no-undo.
    define output   param pcParamNameList   as char no-undo.
    define output   param pcParamValueList  as char no-undo.

    define var cParamListExp    as char no-undo.
    define var cParamName       as char no-undo.
    define var cParamValue      as char no-undo.

    define var str              as char no-undo.
    define var i                as int no-undo.

    assign
        cParamListExp       = pro_parseWordList( pcParamListExp, yes, no )
        cParamListExp       = replace( replace( replace( cParamListExp,

            chr(1) + "," + chr(1),  chr(3) ),
            chr(1) + ",",           chr(3) ),
                     "," + chr(1),  chr(3) )

        pcParamNameList     = ""
        pcParamValueList    = "".

    do i = 1 to num-entries( cParamListExp, chr(3) ):

        str = entry( i, cParamListExp, chr(3) ).

        case num-entries( str, chr(1) ):

            when 1 then do:
    
                assign
                    cParamName  = pro_unquote( entry( 1, str, chr(1) ) )
                    cParamValue = "*".
    
            end. /* num-entries = 1 */

            when 3 then do:
    
                if entry( 2, str, chr(1) ) <> "=" then
    
                    {slib/err_throw "'pro_invalid_param_exp'" pcParamListExp}.
    
                assign
                    cParamName  = pro_unquote( entry( 1, str, chr(1) ) ).
                    cParamValue = pro_unquote( entry( 3, str, chr(1) ) ).
    
            end. /* num-entries = 3 */

            otherwise
    
                {slib/err_throw "'pro_invalid_param_exp'" pcParamListExp}.
    
        end case. /* num-entries( str, chr(1) ) */

        assign
            pcParamNameList    = pcParamNameList 
                + ( if pcParamNameList <> "" then chr(1) else "" )
                + cParamName

            pcParamValueList   = pcParamValueList
                + ( if pcParamValueList <> "" then chr(1) else "" )
                + cParamValue.

    end. /* 1 to num-entries */

end procedure. /* pro_parseParamList */



function pro_parseKeywordPhraseList returns char (

    input pcStatement               as char,
    input pcKeywordSeparatorList    as char ):

    define var cKeywordSeparatorList    as char no-undo.
    define var cKeywordList             as char no-undo.
    define var cSeparatorList           as char no-undo.

    define var cKeywordPhraseList       as char no-undo.
    define var cKeyword                 as char no-undo.
    define var cPhrase                  as char no-undo.
    define var cSeparator               as char no-undo.

    define var iWordCnt                 as int no-undo.
    define var iWord                    as int no-undo.
    define var cWord                    as char no-undo.

    define var str                      as char no-undo.
    define var ch                       as char no-undo.
    define var i                        as int no-undo.
    define var j                        as int no-undo.

    assign
        pcStatement             = pro_parseWordList( pcStatement, no, no )

        cKeywordSeparatorList   = trim( pcKeywordSeparatorList, chr(1) )
        cKeywordList            = ""
        cSeparatorList          = "".

    assign
        i = 1
        j = length( cKeywordSeparatorList ).

    do while i <= j:

        ch = substr( cKeywordSeparatorList, i, 1 ).

        case ch:

            when "|" then
            assign
                ch  = chr(1).

            when "," then
            assign
                ch  = chr(2).

            when "~~" then
            assign
                ch  = substr( cKeywordSeparatorList, i + 1, 1 )
                i   = i + 1.

        end case. /* substr */

        assign
            str = str + ch
            i   = i + 1.

    end. /* 1 to j */

    cKeywordSeparatorList = str.



    j = num-entries( cKeywordSeparatorList, chr(1) ). do i = 1 to j:

        str = trim( entry( i, cKeywordSeparatorList, chr(1) ) ).

        case num-entries( str, chr(2) ):

            when 1 then
            assign
                cKeyword    = entry( 1, str, "/" )
                cSeparator  = "".

            when 2 then
            assign
                cKeyword    = entry( 1, str, chr(2) )
                cSeparator  = entry( 2, str, chr(2) ).

            otherwise
            {slib/err_throw "'"pro_invalid_keyword_separator_exp"'" pcKeywordSeparatorList}.

        end case. /* num-entries */

        if cKeywordList = "" then
        assign
            cKeywordList    = cKeyword
            cSeparatorList  = cSeparator.

        else
        assign
            cKeywordList    = cKeywordList      + chr(1) + cKeyword
            cSeparatorList  = cSeparatorList    + chr(1) + cSeparator.

    end. /* 1 to j */



    assign
        cKeywordPhraseList  = ""
        cKeyword            = ""
        cPhrase             = ""
        cSeparator          = ""

        iWordCnt            = num-entries( pcStatement, chr(1) )
        iWord               = 1.

    repeat while iWord <= iWordCnt:

        cWord = entry( iWord, pcStatement, chr(1) ).

        if cSeparator <> "" and cWord = cSeparator then do:

            cPhrase = trim( cPhrase ) + chr(3).

            if iWord + 1 <= iWordCnt and entry( iWord + 1, pcStatement, chr(1) ) = " " then
               iWord = iWord + 1.

        end. /* cSeparator <> "" */

        else do:

            i = lookup( cWord, cKeywordList, chr(1) ). if i > 0 then do:

                if cKeyword <> "" or cPhrase <> "" then
                   cKeywordPhraseList = cKeywordPhraseList

                        + ( if cKeywordPhraseList <> "" then chr(1) else "" )
                        + cKeyword + chr(2) + trim( cPhrase ).

                assign
                   cKeyword     = cWord
                   cPhrase      = ""
                   cSeparator   = entry( i, cSeparatorList, chr(1) ).

            end. /* lookup( cWord ) > 0 */

            else do:

                cPhrase = cPhrase + cWord.

            end. /* else */

        end. /* else */

        iWord = iWord + 1.

    end. /* repeat */

    if cKeyword <> "" or cPhrase <> "" then
       cKeywordPhraseList = cKeywordPhraseList

            + ( if cKeywordPhraseList <> "" then chr(1) else "" )
            + cKeyword + chr(2) + trim( cPhrase ).

    return cKeywordPhraseList.

end function. /* pro_parseKeywordPhraseList */



define var cParseWordList as char no-undo.

function pro_parseWordList returns char (

    input pcStatement       as char,
    input plRemoveSpaces    as log,
    input plFlat            as log ):

    define var ptr as memptr no-undo.

    assign
        pcStatement = trim( pcStatement )
        pcStatement = pro_replaceNnn( pcStatement ).

    {slib/err_try}:

        set-size( ptr ) = length( pcStatement ).
        put-string( ptr, 1, length( pcStatement ) ) = pcStatement.

        cParseWordList = "".

        run pro_parse(
            input   ptr,
            input   plRemoveSpaces,
            input   plFlat,

            input   "setParseWordList",
            input   this-procedure ).

    {slib/err_catch}:

        {slib/err_throw last}.

    {slib/err_finally}:

        set-size( ptr ) = 0.

    {slib/err_end}.

    return cParseWordList.

end function. /* pro_parseWordList */

procedure setParseWordList private:

    define input param pcWordList as char no-undo.

    cParseWordList = cParseWordList

        + ( if cParseWordList <> "" then chr(1) else "" )
        + pcWordList.

end procedure. /* setParseWordList */



define var iParseLineNum as int no-undo.

procedure pro_parseFile:

    define input    param pcFileName        as char no-undo.
    define input    param plRemoveSpaces    as log no-undo.
    define input    param plFlat            as log no-undo.
    define output   param table             for pro_ttLine.

    define var ptr as memptr no-undo.

    {slib/err_try}:

        file-info:file-name = pcFileName.

        if file-info:full-pathname = ? then
            {slib/err_throw "'file_not_found'" pcFileName}.    

        pcFileName = file-info:full-pathname.

        set-size( ptr ) = file-info:file-size.    

        input from value( pcFileName ).
        import ptr.
        input close.



        empty temp-table pro_ttLine.

        iParseLineNum = 0.

        run pro_parse(
            input   ptr,
            input   plRemoveSpaces,
            input   plFlat,

            input   "insertParseLine",
            input   this-procedure ).

    {slib/err_catch}:

        {slib/err_throw last}.

    {slib/err_finally}:

        set-size( ptr ) = 0.

    {slib/err_end}.

end procedure. /* pro_parseFile */

procedure insertParseLine private:

    define input param pcWordList as char no-undo.

    iParseLineNum = iParseLineNum + 1.

    create pro_ttLine. 
    assign
        pro_ttLine.iLineNum = iParseLineNum
        pro_ttLine.cLine    = pcWordList.

end procedure. /* insertParseLine */



procedure pro_parse:

    define input param ppCode           as memptr no-undo.
    define input param plRemoveSpaces   as log no-undo.
    define input param plFlat           as log no-undo.

    define input param pcCallBack       as char no-undo.
    define input param phProc           as handle no-undo.

    define var lLineEnd             as log no-undo.
    define var lLastSeparator       as log no-undo.

    define var cWordList            as char no-undo.
    define var cWord                as char no-undo.
    define var cSeparator           as char no-undo.

    define var cQuote               as char no-undo.
    define var iOpenCmmt            as int no-undo.
    define var iOpenBracket         as int no-undo.
    define var iOpenSquareBracket   as int no-undo.

    define var iLen                 as int no-undo.
    define var iPos                 as int no-undo.
    define var str                  as char no-undo.
    define var ch                   as char no-undo.
    define var i                    as int no-undo.

    /* need to replace ~NNN to characters and remove from the function */

    if plRemoveSpaces = ? then
       plRemoveSpaces = yes.

    if plFlat = ? then
       plFlat = no.

    assign
        lLineEnd        = no
        lLastSeparator  = no

        cWordList       = ""
        cWord           = ""
        cSeparator      = ""

        iLen            = get-size( ppCode )
        iPos            = 1.

    repeat while iPos <= iLen:

        ch = get-string( ppCode, iPos, 1 ).

        if ch = "/" and get-string( ppCode, iPos + 1, 1 ) = "*" and cWord = "" then do:

            assign
                iOpenCmmt   = 1
                iPos        = iPos + 2.

            repeat while iPos <= iLen and iOpenCmmt > 0:

                case get-string( ppCode, iPos, 2 ):

                    when "/*" then
                    assign
                        iOpenCmmt   = iOpenCmmt + 1
                        iPos        = iPos + 2.

                    when "*/" then
                    assign
                        iOpenCmmt   = iOpenCmmt - 1
                        iPos        = iPos + 2.

                    otherwise
                    assign
                        iPos        = iPos + 1.

                end case. /* get-string */

            end. /* repeat */

        end. /* ch = "/" */

        else
        if  index( " ~n~r~t,=<>!|()[]", ch ) > 0

         or index( ".:", ch ) > 0
        and ( iPos = iLen or index( " ~r~n~t", get-string( ppCode, iPos + 1, 1 ) ) > 0 ) then do:

            case ch:

                when " " or
                when "~n" or
                when "~r" or
                when "~t" then do:

                    if plRemoveSpaces then
                         cSeparator = "".
                    else cSeparator = " ".

                    iPos = iPos + 1.

                    repeat while iPos <= iLen:

                        ch = get-string( ppCode, iPos, 1 ).

                        if  ch <> " "
                        and ch <> "~n"
                        and ch <> "~r" then
                            leave.

                        iPos = iPos + 1.

                    end. /* do while */

                end. /* when " " */
                
                when "<" then do:

                    if index( ">=", get-string( ppCode, iPos + 1, 1 ) ) > 0 then
                    assign
                        cSeparator  = get-string( ppCode, iPos, 2 )
                        iPos        = iPos + 2.

                    else
                    assign
                        cSeparator  = ch
                        iPos        = iPos + 1.

                end. /* when "<" */

                when ">" then do:

                    if index( "=", get-string( ppCode, iPos + 1, 1 ) ) > 0 then
                    assign
                        cSeparator  = get-string( ppCode, iPos, 2 )
                        iPos        = iPos + 2.

                    else
                    assign
                        cSeparator  = ch
                        iPos        = iPos + 1.

                end. /* when ">" */

                when "." or
                when "," or
                when ":" or
                when "=" or
                when "!" or
                when "|" then do:

                    assign
                        cSeparator  = ch
                        iPos        = iPos + 1.

                    if ch = "."
                    or ch = ":" then
                        lLineEnd = yes.

                end. /* when "," */
                
                when "(" or
                when ")" or
                when "[" or
                when "]" then do:

                    if plFlat then do:

                        assign
                            cSeparator  = ch
                            iPos        = iPos + 1.
                            
                    end. /* plFlat */
                    
                    else do:

                        assign
                            iOpenBracket        = 0
                            iOpenSquareBracket  = 0
    
                            cSeparator          = "".
    
                        repeat:
    
                            if index( "()[]", ch ) > 0 then do:
    
                                assign
                                    cSeparator  = cSeparator + ch
                                    iPos        = iPos + 1.
    
                                case ch:
    
                                    when "(" then iOpenBracket = iOpenBracket + 1.
                                    when ")" then iOpenBracket = iOpenBracket - 1.
                                    when "[" then iOpenBracket = iOpenSquareBracket + 1.
                                    when "]" then iOpenBracket = iOpenSquareBracket - 1.
    
                                end case.
    
                                if  iOpenBracket        < 0 then {slib/err_throw "'pro_closing_bracket_not_expected'"           cSeparator}.
                                if  iOpenSquareBracket  < 0 then {slib/err_throw "'pro_closing_square_bracket_not_expected'"    cSeparator}.
    
                                if  iOpenBracket        = 0
                                and iOpenSquareBracket  = 0 then leave.
    
                            end. /* index > 0 */
    
                            else
                            if ch = '"'
                            or ch = "'" then do:
    
                                assign
                                    cQuote      = ch
                                    cSeparator  = cSeparator + ch
                                    iPos        = iPos + 1.
    
                                repeat:
    
                                    if iPos > iLen then
                                        {slib/err_throw "'pro_unmatched_quote'" cSeparator}.
    
                                    ch = get-string( ppCode, iPos, 1 ).
    
                                    if ch = cQuote then do:
    
                                        if get-string( ppCode, iPos + 1, 1 ) = cQuote then do:
    
                                            assign
                                                cSeparator  = cSeparator + get-string( ppCode, iPos, 2 )
                                                iPos        = iPos + 2.
    
                                        end. /* get-string = cQuote */
    
                                        else do:
    
                                            assign
                                                cSeparator  = cSeparator + ch
                                                iPos        = iPos + 1.
    
                                            leave.
    
                                        end. /* else */
    
                                    end. /* ch = cQuote */
    
                                    else
                                    if ch = "~~" or ch = "~\" and opsys = "unix" then do:
    
                                        assign
                                            cSeparator  = cSeparator + get-string( ppCode, iPos, 2 )
                                            iPos        = iPos + 2.
    
                                    end. /* "~" */

                                    else
                                    if asc( ch ) >= 1 and asc( ch ) <= 31 then do:

                                        case ch:

                                            when "~n" then str = "~~n".
                                            when "~r" then str = "~~r".
                                            when "~f" then str = "~~f".
                                            when "~t" then str = "~~t".
                                            when "~b" then str = "~~b".
                                            when "~E" then str = "~~E".

                                            otherwise do:

                                                assign
                                                    i = asc( ch )

                                                    str = string( i mod 8, "9" )        i = ( i - i mod 8 ) / 8
                                                    str = string( i mod 8, "9" ) + str  i = ( i - i mod 8 ) / 8
                                                    str = string( i,       "9" ) + str

                                                    str = "~~" + str.

                                            end. /* otherwise */

                                        end case.

                                        assign
                                            cSeparator  = cSeparator + str
                                            iPos        = iPos + 1.

                                    end. /* asc >= 1 and asc <= 31 */

                                    else do:
    
                                        assign
                                            cSeparator  = cSeparator + ch
                                            iPos        = iPos + 1.
    
                                    end. /* else */
    
                                end. /* repeat */
    
                            end. /* '"' */
    
                            else
                            if ch = "~~" or ch = "~\" and opsys = "unix" then do:
    
                                assign
                                    cSeparator  = cSeparator + get-string( ppCode, iPos, 2 )
                                    iPos        = iPos + 2.
    
                            end. /* "~" */
    
                            else do:
    
                                assign
                                    cSeparator  = cSeparator + ch
                                    iPos        = iPos + 1.
    
                            end. /* else */
    
                            if iPos > iLen then leave.
    
                            ch = get-string( ppCode, iPos, 1 ).
    
                        end. /* repeat */
    
                        if iOpenBracket         > 0 then {slib/err_throw "'pro_unclosed_bracket'"           cSeparator}.
                        if iOpenSquareBracket   > 0 then {slib/err_throw "'pro_unclosed_square_bracket'"    cSeparator}.

                    end. /* else */

                end. /* "(" */

            end case. /* ch */    

            if length( cWord ) > 0 then

                cWordList = cWordList
                     + ( if cWordList <> "" then chr(1) else "" )
                     + cWord.

            if length( cSeparator ) > 0 then do:

                if lLastSeparator then do:

                    if cSeparator <> " " then do:

                        if substr( cWordList, length( cWordList ) - 1, 2 ) = "~001 " then
                           substr( cWordList, length( cWordList ) - 1, 2 ) = "".

                        cWordList = cWordList
                             + ( if cWordList <> "" then chr(1) else "" )
                             + cSeparator.

                    end. /* cSeparator <> " " */
                    
                end. /* lLastSeparator */

                else

                cWordList = cWordList
                     + ( if cWordList <> "" then chr(1) else "" )
                     + cSeparator.

            end. /* length( ) > 0 */

            assign
                lLastSeparator  = yes

                cWord           = ""
                cSeparator      = "".

        end. /* index( ) > 0 */

        else do:

            assign
                lLastSeparator  = no.

            if ch = "~~" or ch = "~\" and opsys = "unix" then
            assign
                cWord   = cWord + get-string( ppCode, iPos, 2 )
                iPos    = iPos + 2.

            else
            if ch = '"' or ch = "'" then do:

                assign
                    cQuote  = ch
                    cWord   = ch
                    iPos    = iPos + 1.

                repeat:

                    if iPos > iLen then
                        {slib/err_throw "'pro_unmatched_quote'" cWord}.

                    ch = get-string( ppCode, iPos, 1 ).

                    if ch = cQuote then do:

                        if get-string( ppCode, iPos + 1, 1 ) = cQuote then do:

                            assign
                                cWord   = cWord + get-string( ppCode, iPos, 2 )
                                iPos    = iPos + 2.

                        end. /* get-string = cQuote */

                        else do:

                            assign
                                cWord   = cWord + ch
                                iPos    = iPos + 1.

                            leave.

                        end. /* else */

                    end. /* ch = cQuote */

                    else
                    if ch = "~~" or ch = "~\" and opsys = "unix" then do:

                        assign
                            cWord   = cWord + get-string( ppCode, iPos, 2 )
                            iPos    = iPos + 2.

                    end. /* ch = "~~" */

                    else
                    if asc( ch ) >= 1 and asc( ch ) <= 31 then do:

                        case ch:

                            when "~n" then str = "~~n".
                            when "~r" then str = "~~r".
                            when "~f" then str = "~~f".
                            when "~t" then str = "~~t".
                            when "~b" then str = "~~b".
                            when "~E" then str = "~~E".
                            
                            otherwise do:

                                assign
                                    i = asc( ch )

                                    str = string( i mod 8, "9" )        i = ( i - i mod 8 ) / 8
                                    str = string( i mod 8, "9" ) + str  i = ( i - i mod 8 ) / 8
                                    str = string( i,       "9" ) + str

                                    str = "~~" + str.

                            end. /* otherwise */
                        
                        end case.

                        assign
                            cWord   = cWord + str
                            iPos    = iPos + 1.

                    end. /* asc >= 1 and asc <= 31 */

                    else do:

                        assign
                            cWord   = cWord + ch
                            iPos    = iPos + 1.

                    end. /* else */

                end. /* repeat */

            end. /* ch = '"' */

            else
            assign
                cWord   = cWord + ch
                iPos    = iPos + 1.

        end. /* else */

        if lLineEnd or iPos > iLen then do:
                
            if cWord <> "" then 
               cWordList = cWordList
                    + ( if cWordList <> "" then chr(1) else "" )
                    + cWord.

            cWordList = trim( cWordList, " ~001" ).

            if cWordList <> "" then do:
            
                run value( pcCallBack ) in phProc ( cWordList ).

                assign
                    lLineEnd        = no
                    lLastSeparator  = no

                    cWordList       = ""
                    cWord           = ""
                    cSeparator      = "".

            end. /* cWordList <> "" */

        end. /* lLineEnd */

    end. /* repeat */

end procedure. /* pro_parse */



function pro_replaceNnn returns char ( pcStr as char ):

    define var n as char no-undo.
    define var i as int no-undo.
    define var j as int no-undo.

    j = index( pcStr, "~~" ).

    repeat while j <> 0:

        _Nnn:

        do:

            n = substr( pcStr, j + 1, 1 ). if not ( n >= "0" and n <= "3" ) then leave _Nnn.
            i =     int(n) * 64.

            n = substr( pcStr, j + 2, 1 ). if not ( n >= "0" and n <= "9" ) then leave _Nnn.
            i = i + int(n) * 8.

            n = substr( pcStr, j + 3, 1 ). if not ( n >= "0" and n <= "9" ) then leave _Nnn.
            i = i + int(n).

            if i > 255 then i = i mod 256.
            if i = 000 then i = 032.

            assign
                substr( pcStr, j, 4 ) = chr(i)

                j = index( pcStr, "~~", j + 4 ).

            next.

        end. /* _Nnn */

        j = index( pcStr, "~~", j + 2 ).

    end. /* repeat */

    return pcStr.

end function. /* replaceNnn */

function pro_unquote returns char ( pcStr as char ):

    define var cQuote   as char no-undo.
    define var iLen     as int no-undo.
    define var iPos     as int no-undo.

    define var str      as char no-undo.
    define var ch       as char no-undo.

    pcStr = trim( pcStr ).

    if  not pcStr begins '"'
    and not pcStr begins "'" then return pcStr.

    assign
        cQuote  = substr( pcStr, 1, 1 )
        str     = ""

        iLen    = length( pcStr )
        iPos    = 2.

    repeat while iPos <= iLen:

        ch = substr( pcStr, iPos, 1 ).

        if ch = cQuote then do:

            if substr( pcStr, iPos + 1, 1 ) = cQuote then do:

                assign
                    str     = str + ch
                    iPos    = iPos + 2.

            end. /* substr = cQuote */

            else do:

                assign
                    iPos    = iPos + 1.

                leave.

            end. /* else */

        end. /* ch = cQuote */

        else
        if ch = "~~" or ch = "~\" and opsys = "unix" then do:

            assign
                str     = str + substr( pcStr, iPos + 1, 1 )
                iPos    = iPos + 2.

        end. /* "~" */

        else do:

            assign
                str     = str + ch
                iPos    = iPos + 1.

        end. /* else */

    end. /* repeat */

    return str.

end function. /* pro_unquote */

function pro_unparenthesis returns char ( pcStr as char ):

    pcStr = trim( pcStr ).

    if substr( pcStr, 1, 1 ) = "(" then
       substr( pcStr, 1, 1 ) = "".

    if substr( pcStr, length( pcStr ), 1 ) = ")" then
       substr( pcStr, length( pcStr ), 1 ) = "".

    pcStr = trim( pcStr ).
    
    return pcStr.

end function. /* pro_unparenthesis */
