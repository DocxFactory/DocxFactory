
/**
 * slibstr.p -
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

define var cHiChar          as char no-undo.
define var cHiCharSensitive as char case-sensitive no-undo.



on "close" of this-procedure do:

    delete procedure this-procedure.

end. /* on close */

procedure initializeProc:

    define var i as int no-undo.

    /* generate hichar and case-sensitive hichar according to the current/internal codepage and collation.
       note that the database codepage could be different and the hichar converted value may not be the highest char
       hi char value is alot trickier then hi date or int etc. :P */

    assign
        cHiChar             = chr(1)
        cHiCharSensitive    = chr(1).

    do i = 2 to 255:

        assign
            cHiChar             = max( cHiChar, chr(i) )
            cHiCharSensitive    = max( cHiCharSensitive, chr(i) ).

    end. /*  2 o 255 */

    /* length is not more then 32 because there are limits to index bracket searches see error 141 */

    assign
        cHiChar             = fill( cHiChar, 32 )
        cHiCharSensitive    = fill( cHiCharSensitive, 32 ).

end procedure. /* initializeProc */



function str_trimMultipleSpace returns char ( pcStr as char ):

    if pcStr = ? then
        return ?.

    if length( pcStr ) = 0 then
        return "".



    pcStr = replace( replace( replace( pcStr,

        "~r~n", " " ),
        "~n", " " ),
        "~r", " " ).

    if pcStr = " " then
        return " ".



    pcStr = trim( pcStr ).

    do while index( pcStr, "  " ) > 0:
        pcStr = replace( pcStr, "  ", " " ).
    end.

    return pcStr.

end function. /* str_trimMultipleSpace */



function str_alignLeft returns char ( pcStr as char, piWidth as int, pcCont as char ):

    define var iLen as int no-undo.
    define var i    as int no-undo.

    /* the string is not trimmed because it wouldn't be suitable for every case. 
       the string can be trimmed before being passed, if needed. */

    iLen = length( pcStr ).

    if piWidth > iLen then

        pcStr = pcStr + fill( " ", piWidth - iLen ).

    else
    if piWidth < iLen then do:

        pcStr = substr( pcStr, 1, piWidth ).

        if pcCont <> ? and pcCont <> "" then

        assign
            i = length( pcCont )
            substr( pcStr, piWidth - i + 1, i ) = pcCont.

    end. /* piWidth < iLen */

    return pcStr.

end function. /* str_alignLeft */

function str_alignRight returns char ( pcStr as char, piWidth as int, pcCont as char ):

    define var iLen as int no-undo.

    iLen = length( pcStr ).

    if piWidth > iLen then

        pcStr = fill( " ", piWidth - iLen ) + pcStr.

    else
    if piWidth < iLen then do:

        pcStr = substr( pcStr, iLen - piWidth + 1, piWidth ).

        if pcCont <> ? and pcCont <> "" then

        substr( pcStr, 1, length( pcCont ) ) = pcCont.

    end. /* piWidth < iLen */

    return pcStr.

end function. /* str_alignRight */

function str_alignCenter returns char ( pcStr as char, piWidth as int, pcCont as char ):

    define var iLen as int no-undo.
    define var i    as int no-undo.
    define var j    as int no-undo.

    assign
        iLen    = length( pcStr )

        i       = abs( piWidth - iLen )
        j       = i / 2.

    if piWidth > iLen then

        pcStr = fill( " ", j ) + pcStr + fill( " ", i - j ).

    else
    if piWidth < iLen then do:

        pcStr = substr( pcStr, j + 1, piWidth ).

        if pcCont <> ? and pcCont <> "" then

        assign
            i = length( pcCont )

            substr( pcStr, 1, i )               = pcCont
            substr( pcStr, piWidth - i + 1, i ) = pcCont.

    end. /* piWidth < iLen */

    return pcStr.

end function. /* str_alignCenter */



function str_padLeft returns char ( pcStr as char, pcCh as char, piWidth as int ):

    define var iLen as int no-undo.

    iLen = length( pcStr ).

    if piWidth > iLen then

        pcStr = fill( pcCh, piWidth - iLen ) + pcStr.

    else
    if piWidth < iLen then

        pcStr = substr( pcStr, iLen - piWidth + 1, piWidth ).

    return pcStr.

end function. /* str_padLeft */

function str_padRight returns char ( pcStr as char, pcCh as char, piWidth as int ):

    define var iLen as int no-undo.

    iLen = length( pcStr ).

    if piWidth > iLen then

        pcStr = pcStr + fill( pcCh, piWidth - iLen ).

    else
    if piWidth < iLen then

        pcStr = substr( pcStr, 1, piWidth ).

    return pcStr.

end function. /* str_padRight */

function str_padCenter returns char ( pcStr as char, pcCh as char, piWidth as int ):

    define var iLen as int no-undo.
    define var i    as int no-undo.
    define var j    as int no-undo.

    assign
        iLen    = length( pcStr )

        i       = abs( piWidth - iLen )
        j       = i / 2.

    if piWidth > iLen then

        pcStr = fill( pcCh, j ) + pcStr + fill( pcCh, i - j ).

    else
    if piWidth < iLen then

        pcStr = substr( pcStr, j + 1, piWidth ).

    return pcStr.

end function. /* str_padCenter */



function str_concat returns char ( pcStr as char, pcEntry as char, pcDelim as char, plAddAlways as log ):

    if pcDelim = ? then
       pcDelim = ",". /* like entry, num-entries etc. default delimiter */

    if plAddAlways or lookup( pcEntry, pcStr, pcDelim ) = 0 then

       pcStr = pcStr 
             + ( if pcStr <> "" then pcDelim else "" ) 
             + pcEntry.

    return pcStr.

end function. /* str_concat */

function str_concatL returns char ( pcStr as char, pcEntry as char, pcDelim as char, plAddAlways as log ):

    if pcDelim = ? then
       pcDelim = ",".

    if plAddAlways or lookup( pcEntry, pcStr, pcDelim ) = 0 then

       pcStr = pcEntry 
             + ( if pcStr <> "" then pcDelim else "" )
             + pcStr.

    return pcStr.

end function. /* str_concatL */



function str_getEntries returns char ( pcEntryList as char, piEntryFrom as int, piEntryTo as int, pcDelim as char ):

    define var RetVal   as char no-undo.
    define var iLen     as int no-undo.
    define var i        as int no-undo.

    if pcEntryList = ? then
        return ?.

    if pcDelim = ? then
       pcDelim = ",".

    iLen = num-entries( pcEntryList, pcDelim ).



    if piEntryFrom = ? then
       piEntryFrom = 1.

    if piEntryTo = ? then
       piEntryTo = iLen.

    if piEntryFrom < 1 then
       piEntryFrom = 1.

    if piEntryTo > iLen then
       piEntryTo = iLen.



    RetVal = "".

    do i = piEntryFrom to piEntryTo:

        RetVal = RetVal
            + ( if RetVal <> "" then pcDelim else "" )
            + entry( i, pcEntryList, pcDelim ).

    end. /* piEntryFrom to piEntryTo */

    return RetVal.
    
end function. /* str_getEntries */

function str_sortEntries returns char ( pcEntryList as char, plDescend as log, pcDelim as char ): /* uses a simple insert sort */

    define var cStr         as char no-undo.
    define var iPos         as int no-undo.
    define var iLen         as int no-undo.

    define var cStr2        as char no-undo.
    define var iPos2        as int no-undo.
    define var iLen2        as int no-undo.
    define var cEntryList2  as char no-undo.

    if pcEntryList = ? then
        return ?.

    if plDescend = ? then
       plDescend = no.

    if pcDelim = ? then
       pcDelim = ",".

    assign
        iLen        = num-entries( pcEntryList, pcDelim )
        iLen2       = 0

        cEntryList2 = "".



    do iPos = 1 to iLen:

        cStr = entry( iPos, pcEntryList, pcDelim ).

        if iLen2 = 0 then

            cEntryList2 = cStr.

        else do:

            do iPos2 = 1 to iLen2:

                cStr2 = entry( iPos2, cEntryList2, pcDelim ).

                if plDescend = no   and cStr <= cStr2
                or plDescend = yes  and cStr >= cStr2 then
                    leave.

            end. /* do iPos2 */

            if iPos2 > iLen2 then

                 cEntryList2 = cEntryList2 + pcDelim + cStr.

            else entry( iPos2, cEntryList2, pcDelim ) = cStr + pcDelim + cStr2.

        end. /* else */

        iLen2 = iLen2 + 1.

    end. /* do iPos */

    return cEntryList2.

end function. /* str_sortEntries */



function str_soundex returns char ( pcStr as char ):

    define var cRetVal  as char no-undo.
    define var iLen     as int no-undo.
    define var iPos     as int no-undo.

    define var iLast    as int no-undo.
    define var i        as int no-undo.

    assign
        pcStr   = str_trimMultipleSpace( pcStr )
        pcStr   = caps( pcStr )

        iPos    = 1.

    repeat while iPos <= length( pcStr ):

        if substr( pcStr, iPos, 3 ) = "ITE" then assign
           substr( pcStr, iPos, 3 ) = "IGHT"
           iPos = iPos + 4.

        else
        if substr( pcStr, iPos, 2 ) = "DG" then assign
           substr( pcStr, iPos, 2 ) = "G"
           iPos = iPos + 1.

        else
        if substr( pcStr, iPos, 3 ) = "TIA" then assign
           substr( pcStr, iPos, 3 ) = "SHA"
           iPos = iPos + 4.

        else
        if substr( pcStr, iPos, 3 ) = "CHM" then assign
           substr( pcStr, iPos, 3 ) = "M"
           iPos = iPos + 1.

        else
        if substr( pcStr, iPos, 2 ) = "DT" then assign
           substr( pcStr, iPos, 2 ) = "T"
           iPos = iPos + 1.

        else
        if substr( pcStr, iPos, 3 ) = "CKS" then assign
           substr( pcStr, iPos, 3 ) = "X"
           iPos = iPos + 1.

        else
        if substr( pcStr, iPos, 2 ) = "CK" then assign
           substr( pcStr, iPos, 2 ) = "C"
           iPos = iPos + 1.

        else
        if substr( pcStr, iPos, 2 ) = "PH" then assign
           substr( pcStr, iPos, 2 ) = "F"
           iPos = iPos + 1.
           
        else
        if substr( pcStr, iPos, 2 ) = "PF" then assign
           substr( pcStr, iPos, 2 ) = "F"
           iPos = iPos + 1.

        else
           iPos = iPos + 1.

    end. /* repeat */

    if substr( pcStr, 1, 2 ) = "WR" then assign
       substr( pcStr, 1, 2 ) = "R".

    else
    if substr( pcStr, 1, 2 ) = "KN" then assign
       substr( pcStr, 1, 2 ) = "N".
       
    else
    if substr( pcStr, 1, 2 ) = "PN" then assign
       substr( pcStr, 1, 2 ) = "N".
    
    else
    if substr( pcStr, 1, 1 ) = "X" then assign
       substr( pcStr, 1, 1 ) = "Z".

    else
    if substr( pcStr, 1, 1 ) = "K" then assign
       substr( pcStr, 1, 1 ) = "C".



    assign
       cRetVal  = substr( pcStr, 1, 1 )

       iLast    = ?

       iLen     = length( pcStr ).

    do iPos = 2 to iLen:

        i = ?.

        case substr( pcStr, iPos, 1 ):
        
            when "B" or 
            when "F" or
            when "P" or
            when "V" then i = 1.

            when "C" or
            when "G" or
            when "J" or
            when "K" or
            when "Q" or
            when "S" or
            when "X" or
            when "Z" then i = 2.

            when "D" or
            when "T" then i = 3.
            
            when "L" then i = 4.
            
            when "M" or
            when "N" then i = 5.

            when "R" then i = 6.

        end case.

        if  i <> ?
        and i <> iLast then do:
        
            assign
                cRetVal = cRetVal + string(i)
                iLast   = i.

            if length( cRetVal ) = 4 then
                leave.

        end. /* i <> ? */

    end. /* do iPos */

    if length( cRetVal ) < 4 then
        cRetVal = cRetVal + fill( "0", 4 - length( cRetVal ) ).

    return cRetVal.

end function. /* str_soundex */



/* replaces mfg/pro hichar. since functions in queries are resolved once when the query is opened it has the same result and cleaner more elegant code. 
   note the remarks about hichar in initializeProc */

function str_HiChar returns char ( pcStr as char ):

    if pcStr = "" then
         return cHiChar.
    else return pcStr.

end function. /* str_HiChar */

function str_LoChar returns char ( pcStr as char ): /* although low value functions isn't required for chars because it's required for all other variables and datatypes it may still be used for creating consistent looking code. */

    if pcStr = "" then
         return "".
    else return pcStr.

end function. /* str_LoChar */



/* hi/lo char for case-sensitive fields */

function str_HiCharSensitive returns char ( pcStr as char ):

    if pcStr = "" then
         return cHiCharSensitive.
    else return pcStr.

end function. /* str_HiCharSensitive */

function str_LoCharSensitive returns char ( pcStr as char ):

    if pcStr = "" then
         return "".
    else return pcStr.

end function. /* str_LoCharSensitive */
