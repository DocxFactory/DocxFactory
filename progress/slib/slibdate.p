
/**
 * slibdate.p -
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

{slib/slibdatefrwd.i forward}

{slib/slibstr.i}

{slib/slibpro.i}



function getIntStrict returns int private 

    ( pcValue as char ) forward.



define var cWeekDay as char extent 7 no-undo init

    [ "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" ].

/*** locale ***
define var cWeekDayHeb as char extent 7 no-undo init

    [ "ראשון", "שני", "שלישי", "רביעי", "חמישי", "שישי", "שבת" ].
*** locale ***/

define var cMonth as char extent 12 no-undo init

    [ "January",    "February",     "March", 
      "April",      "May",          "June", 
      "July",       "August",       "September",
      "October",    "November",     "December" ].

/*** locale ***
define var cMonthHeb as char extent 12 no-undo init

    [ "ינואר",      "פברואר",       "מרץ", 
      "אפריל",      "מאי",      "יוני", 
      "יולי",       "אוגוסט",    "ספטמבר",
      "אוקטובר",    "נובמבר",     "דצמבר" ].
*** locale ***/



on "close" of this-procedure do:

    delete procedure this-procedure.

end. /* on close */

procedure initializeProc:

end procedure. /* initializeProc */



/* the native 4gl string and date functions are faster then the date2str and str2date functions.
   the difference may be especially noticeable when used in highly repetitive tasks or queries.
   if possible use 4gl functions. */

&if {&pro_xProversion} >= "10" &then

function date_DatetimeTz2Str returns char ( ptDatetimeTz as datetime-tz, pcFormat as char ):

    return date_Date2Str( 

        date    ( ptDatetimeTz ),
        mtime   ( ptDatetimeTz ), 
        timezone( ptDateTimeTz ), 
        pcFormat ).

end function. /* date_DatetimeTz2Str */

function date_Datetime2Str returns char ( ptDatetime as datetime, pcFormat as char ):

    return date_Date2Str( 

        date ( ptDatetime ),
        mtime( ptDatetime ), 
        0,
        pcFormat ).

end function. /* date_Datetime2Str */

&endif /* proversion >= "10" */

function date_Date2Str returns char ( ptDate as date, piMTime as int, piTimeZone as int, pcFormat as char ):

    define var iHour        as int no-undo init ?.
    define var iMin         as int no-undo init ?.
    define var iSec         as int no-undo init ?.
    define var iMSec        as int no-undo init ?.

    define var iLen         as int no-undo.
    define var iPos         as int no-undo.
    define var iEntryLen    as int no-undo.

    define var retval       as char no-undo.
    define var str          as char no-undo.
    define var ch           as char no-undo case-sensitive.
    define var x            as dec no-undo.
    define var i            as int no-undo.
    define var j            as int no-undo.

    case pcFormat:

        when "x-date"       then pcFormat = "yyyy-mm-dd".
        when "x-datetime"   then pcFormat = "yyyy-mm-ddThh:ii:ss.fff".
        when "x-datetimetz" then pcFormat = "yyyy-mm-ddThh:ii:ss.fffzzzzzz".

    end case. /* pcFormat */



    assign
        iLen = length( pcFormat )
        iPos = 1.

    repeat while iPos <= iLen:

        ch = substr( pcFormat, iPos, 1 ). 

        if ch = "~~" then

        assign
            iPos = iPos + 1 retval = retval + substr( pcFormat, iPos, 1 )
            iPos = iPos + 1.

        else            

        /*** locale ***
        if index( "mdjJywuUXqhHisfpPzעיח", ch ) > 0 then do:
        *** locale ***/

        if index( "mdjJywuUXqhHisfpPz", ch ) > 0 then do:

            if index( "hHisfpP", ch ) > 0 and iHour = ? then do:

                assign
                    i       = piMTime
                    iMSec   = i mod 1000    i = ( i - iMSec )   / 1000
                    iSec    = i mod 60      i = ( i - iSec )    / 60
                    iMin    = i mod 60      i = ( i - iMin )    / 60
                    iHour   = i.

            end. /* index > 0 */



            assign
                iPos        = iPos + 1
                iEntryLen   = 1.

            do while iPos <= iLen and substr( pcFormat, iPos, 1 ) = ch:

                assign
                    iPos        = iPos + 1
                    iEntryLen   = iEntryLen + 1.

            end. /* do while */

            case ch:

                when "m" then do:

                    i = month( ptDate ).

                    if iEntryLen = 1 then
                    retval = retval + string( i ). 

                    else
                    if iEntryLen = 2 then
                    retval = retval + string( i, "99" ).

                    else
                    if iEntryLen = 3 then
                    retval = retval + substr( cMonth[i], 1, 3 ).

                    else
                    if iEntryLen = 5 then
                    retval = retval + cMonth[i].

                    else
                    retval = retval + substr( pcFormat, iPos - iEntryLen, iEntryLen ).

                end. /* m */

                /*** locale ***
                when "ח" then do:

                    i = month( ptDate ).

                    if iEntryLen = 1 then
                    retval = retval + string( i ). 

                    else
                    if iEntryLen = 2 then
                    retval = retval + string( i, "99" ).

                    else
                    if iEntryLen = 3 then
                    retval = retval + substr( cMonthHeb[i], 1, 3 ).

                    else
                    if iEntryLen = 5 then
                    retval = retval + cMonthHeb[i].

                    else
                    retval = retval + substr( pcFormat, iPos - iEntryLen, iEntryLen ).

                end. /* ח */
                *** locale ***/

                when "d" then do:

                    i = day( ptDate ).

                    if iEntryLen = 1 then
                    retval = retval + string( i ). 

                    else
                    if iEntryLen = 2 then
                    retval = retval + string( i, "99" ).

                    else
                    retval = retval + substr( pcFormat, iPos - iEntryLen, iEntryLen ).

                end. /* d */

                when "j" then do:

                    i = ptDate - date( 1, 1, year( ptDate ) ) + 1.

                    if iEntryLen = 1 then
                    retval = retval + string( i ).

                    else
                    if iEntryLen = 3 then
                    retval = retval + string( i, "999" ).

                    else
                    retval = retval + substr( pcFormat, iPos - iEntryLen, iEntryLen ).

                end. /* j */

                when "J" then do:

                    x = ( ptDate - date( 1, 1, 1970 ) ) * 86400000.0 + piMTime.

                    retval = retval + string( x ).

                end. /* J */

                when "y" then do:

                    i = year( ptDate ).

                    if iEntryLen = 1 then
                    retval = retval + string( i ).

                    else
                    if iEntryLen = 2 then
                    retval = retval + string( i mod 100, "99" ).

                    else
                    if iEntryLen = 4 then
                    retval = retval + string( i ).

                    else
                    retval = retval + substr( pcFormat, iPos - iEntryLen, iEntryLen ).

                end. /* y */

                when "w" then do:

                    i = weekday( ptDate ).

                    if iEntryLen = 1 then
                    retval = retval + string( i ). 

                    else
                    if iEntryLen = 3 then
                    retval = retval + substr( cWeekDay[i], 1, 3 ).

                    else
                    if iEntryLen = 5 then
                    retval = retval + cWeekDay[i].

                    else
                    retval = retval + substr( pcFormat, iPos - iEntryLen, iEntryLen ).

                end. /* w */

                /*** locale ***
                when "י" then do:

                    i = weekday( ptDate ).

                    if iEntryLen = 1 then
                    retval = retval + string( i ). 

                    else
                    if iEntryLen = 3 then
                    retval = retval + chr( asc( "א" ) + i - 1 ).

                    else
                    if iEntryLen = 5 then
                    retval = retval + cWeekDayHeb[i].

                    else
                    retval = retval + substr( pcFormat, iPos - iEntryLen, iEntryLen ).

                end. /* w */
                *** locale ***/

                when "u" or when "U" then do:

                    i = date_getWeekNum( ptDate, ( if ch = "u" then 2 else 1 ) ).

                    if iEntryLen = 1 then
                    retval = retval + string( i ). 

                    else
                    if iEntryLen = 2 then
                    retval = retval + string( i, "99" ).

                    else
                    retval = retval + substr( pcFormat, iPos - iEntryLen, iEntryLen ).

                end. /* u or U */

                when "X" then do:

                    x = ( ptDate - date( 1, 1, 1970 ) ) * 86400.0 + trunc( piMTime / 1000, 0 ).

                    retval = retval + string( x ).

                end. /* X */

                when "q" then do:

                    i = date_getQuarter( ptDate ).

                    if iEntryLen = 1 then
                    retval = retval + string( i ).

                end. /* u or U */

                when "h" or when "H" then do:

                    i = iHour.
                    if ch = "H" and i > 12 then i = i - 12. /* this is not mod */

                    if iEntryLen = 1 then
                    retval = retval + string( i mod 24 ). 

                    else
                    if iEntryLen = 2 then
                    retval = retval + string( i mod 24, "99" ).

                    else
                    if iEntryLen = 3 and ch = "h" then
                    retval = retval + string( i ).

                    else
                    retval = retval + substr( pcFormat, iPos - iEntryLen, iEntryLen ).

                end. /* h or H */

                when "i" then do:

                    if iEntryLen = 1 then
                    retval = retval + string( iMin ). 

                    else
                    if iEntryLen = 2 then
                    retval = retval + string( iMin, "99" ).

                    else
                    retval = retval + substr( pcFormat, iPos - iEntryLen, iEntryLen ).

                end. /* i */

                when "s" then do:

                    if iEntryLen = 1 then
                    retval = retval + string( iSec ). 

                    else
                    if iEntryLen = 2 then
                    retval = retval + string( iSec, "99" ).

                    else
                    retval = retval + substr( pcFormat, iPos - iEntryLen, iEntryLen ).

                end. /* s */

                when "f" then do:

                    if iEntryLen < 3 then
                    retval = retval + substr( string( iMSec, "999" ), 1, iEntryLen ).

                    else
                    if iEntryLen = 3 then
                    retval = retval + string( iMSec, "999" ).

                    else
                    retval = retval + substr( pcFormat, iPos - iEntryLen, iEntryLen ).

                end. /* f */

                when "p" or when "P" then do:

                    if iEntryLen = 2 then do:

                        str = ( if iHour < 12 then "am" else "pm" ).

                        if ch = "p" then
                        retval = retval + str.

                        else
                        retval = retval + caps( str ).

                    end. /* len = 2 */

                    else
                    retval = retval + substr( pcFormat, iPos - iEntryLen, iEntryLen ).

                end. /* p or P */

                /*** locale ***
                when "ע" then do:

                    if iEntryLen = 1 then
                    retval = retval + ( if iHour < 12 then "בוקר" else "ערב" ).

                    else
                    retval = retval + substr( pcFormat, iPos - iEntryLen, iEntryLen ).

                end. /* ע */
                *** locale ***/

                when "z" then do:

                    if iEntryLen = 3 then do:

                        retval = retval + ( if piTimeZone < 0 then "-" else "+" )

                            + string( ( abs( piTimeZone ) - abs( piTimeZone ) mod 60 ) mod 24 , "99" ).

                    end. /* len = 3 */

                    else
                    if iEntryLen = 6 or iEntryLen = 5 then do:

                        assign
                            i =   abs( piTimeZone )       mod 60
                            j = ( abs( piTimeZone ) - i ) mod 24.

                        retval = retval + ( if piTimeZone < 0 then "-" else "+" )

                            + string( j, "99" ) + ( if iEntryLen = 6 then ":" else "" )
                            + string( i, "99" ).

                    end. /* len = 6 */

                    else
                    retval = retval + substr( pcFormat, iPos - iEntryLen, iEntryLen ).

                end. /* z */

            end case. /* ch */

        end. /* index > 0 */

        else

        assign
            retval  = retval + ch
            iPos    = iPos + 1.

    end. /* repeat */

    return retval.

end function. /* date_Date2Str */



&if {&pro_xProversion} >= "10" &then

function date_Str2DatetimeTz returns datetime-tz ( pcStr as char, pcFormat as char ):

    define var tDate        as date no-undo.
    define var iMTime       as int no-undo.
    define var iTimeZone    as int no-undo.

    run date_Str2Date(

        input   pcStr,
        input   pcFormat,

        output  tDate,
        output  iMTime,
        output  iTimeZone ).

    return datetime-tz( tDate, ( if iMTime <> ? then iMTime else 0 ), ( if iTimeZone <> ? then iTimeZone else 0 ) ).

end function. /* date_Str2DatetimeTz */

function date_Str2Datetime returns datetime ( pcStr as char, pcFormat as char ):

    define var tDate        as date no-undo.
    define var iMTime       as int no-undo.
    define var iTimeZone    as int no-undo.

    run date_Str2Date(

        input   pcStr,
        input   pcFormat,

        output  tDate,
        output  iMTime,
        output  iTimeZone ).

    return datetime( tDate, ( if iMTime <> ? then iMTime else 0 ) ).

end function. /* date_Str2Datetime */

&endif /* proversion >= "10" */

procedure date_Str2Date:

    define input    param pcStr         as char no-undo.
    define input    param pcFormat      as char no-undo.
    define output   param ptDate        as date no-undo init ?.
    define output   param piMTime       as int no-undo  init ?.
    define output   param piTimeZone    as int no-undo  init ?.

    define var tDate        as date no-undo init ?.
    define var iMonth       as int no-undo  init ?.
    define var iDay         as int no-undo  init ?.
    define var iYearDay     as int no-undo  init ?.
    define var iYear        as int no-undo  init ?.

    define var iQuarter     as int no-undo  init ?.
    define var iWeekNum     as int no-undo  init ?.
    define var iWeekDay     as int no-undo  init ?.
    define var iWeekPerDay  as int no-undo  init ?.

    define var iMTime       as int no-undo  init ?.
    define var iHour        as int no-undo  init ?.
    define var iMin         as int no-undo  init ?.
    define var iSec         as int no-undo  init ?.
    define var iMSec        as int no-undo  init ?.

    define var l12Hours     as log no-undo  init ?.
    define var lPm          as log no-undo  init ?.
    define var iTimeZone    as int no-undo  init ?.

    define var iStrLen      as int no-undo.
    define var iStrPos      as int no-undo.
    define var iFormLen     as int no-undo.
    define var iFormPos     as int no-undo.
    define var iEntryLen    as int no-undo.

    define var str          as char no-undo.
    define var ch           as char no-undo case-sensitive.
    define var x            as dec no-undo.
    define var i            as int no-undo.
    define var j            as int no-undo.

    case pcFormat:

        when "x-date"       then pcFormat = "yyyy-mm-dd".
        when "x-datetime"   then pcFormat = "yyyy-mm-ddThh:ii:ss.fff".
        when "x-datetimetz" then pcFormat = "yyyy-mm-ddThh:ii:ss.fffzzzzzz".

    end case. /* pcFormat */



    assign
        pcStr       = str_trimMultipleSpace( pcStr )
        pcFormat    = str_trimMultipleSpace( pcFormat )

        iStrLen     = length( pcStr )
        iStrPos     = 1

        iFormLen    = length( pcFormat )
        iFormPos    = 1.

    repeat while iFormPos <= iFormLen:

        if iStrPos > iStrLen then return.



        ch = substr( pcFormat, iFormPos, 1 ). 

        if ch = "~~" then do:

            assign
                iStrPos     = iStrPos + 1
                iFormPos    = iFormPos + 2.

        end. /* "~" */

        else            

        /*** locale ***
        if index( "mdjJywuUXqhHisfpPzעיח", ch ) > 0 then do:
        *** locale ***/

        if index( "mdjJywuUXqhHisfpPz", ch ) > 0 then do:

            assign
                iFormPos    = iFormPos + 1
                iEntryLen   = 1.

            do while iFormPos <= iFormLen and substr( pcFormat, iFormPos, 1 ) = ch:

                assign
                    iFormPos    = iFormPos + 1
                    iEntryLen   = iEntryLen + 1.

            end. /* do while */



            case ch:

                when "m" then do:

                    if iEntryLen = 1 then do:

                        do i = 2 to 1 by -1:

                            if iStrPos + i - 1 > iStrLen then next.

                            iMonth = getIntStrict( substr( pcStr, iStrPos, i ) ).

                            if iMonth = ? then
                                 next.
                            else leave.

                        end. /* 2 to 1 */

                        if i = 0 then return.

                        if iMonth < 1 or iMonth > 12 then return.

                        iStrPos = iStrPos + i.

                    end. /* len = 1 */

                    else
                    if iEntryLen = 2 then do:

                        if iStrPos + iEntryLen - 1 > iStrLen then return.

                        iMonth = getIntStrict( substr( pcStr, iStrPos, iEntryLen ) ).

                        if iMonth = ?
                        or iMonth < 1 or iMonth > 12 then return.

                        iStrPos = iStrPos + iEntryLen.

                    end. /* len = 2 */

                    else
                    if iEntryLen = 3 then do:

                        if iStrPos + iEntryLen - 1 > iStrLen then return.

                        do i = 1 to 12:

                            if substr( pcStr, iStrPos, iEntryLen ) = substr( cMonth[i], 1, iEntryLen ) then
                                leave.

                        end. /* 1 to 12 */

                        if i = 13 then return.

                        assign
                            iMonth  = i
                            iStrPos = iStrPos + iEntryLen.

                    end. /* len = 3 */

                    else
                    if iEntryLen = 5 then do:

                        do i = 1 to 12:

                            j = length( cMonth[i] ).

                            if iStrPos + j - 1 > iStrLen then next.

                            if substr( pcStr, iStrPos, j ) = cMonth[i] then leave.

                        end. /* 1 to 12 */

                        if i = 13 then return.

                        assign
                            iMonth  = i
                            iStrPos = iStrPos + j.

                    end. /* len = 5 */

                    else
                    iStrPos = iStrPos + iEntryLen.

                end. /* m */

                /*** locale ***
                when "ח" then do:

                    if iEntryLen = 1 then do:

                        do i = 2 to 1 by -1:

                            if iStrPos + i - 1 > iStrLen then next.

                            iMonth = getIntStrict( substr( pcStr, iStrPos, i ) ).

                            if iMonth = ? then
                                 next.
                            else leave.

                        end. /* 2 to 1 */

                        if i = 0 then return.

                        if iMonth < 1 or iMonth > 12 then return.

                        iStrPos = iStrPos + i.

                    end. /* len = 1 */

                    else
                    if iEntryLen = 2 then do:

                        if iStrPos + iEntryLen - 1 > iStrLen then return.

                        iMonth = getIntStrict( substr( pcStr, iStrPos, iEntryLen ) ).

                        if iMonth = ?
                        or iMonth < 1 or iMonth > 12 then return.

                        iStrPos = iStrPos + iEntryLen.

                    end. /* len = 2 */

                    else
                    if iEntryLen = 3 then do:

                        if iStrPos + iEntryLen - 1 > iStrLen then return.

                        do i = 1 to 12:

                            if substr( pcStr, iStrPos, iEntryLen ) = substr( cMonthHeb[i], 1, iEntryLen ) then
                                leave.

                        end. /* 1 to 12 */

                        if i = 13 then return.

                        assign
                            iMonth  = i
                            iStrPos = iStrPos + iEntryLen.

                    end. /* len = 3 */

                    else
                    if iEntryLen = 5 then do:

                        do i = 1 to 12:

                            j = length( cMonthHeb[i] ).

                            if iStrPos + j - 1 > iStrLen then next.

                            if substr( pcStr, iStrPos, j ) = cMonthHeb[i] then leave.

                        end. /* 1 to 12 */

                        if i = 13 then return.

                        assign
                            iMonth  = i
                            iStrPos = iStrPos + j.

                    end. /* len = 5 */

                    else
                    iStrPos = iStrPos + iEntryLen.

                end. /* ח */
                *** locale ***/

                when "d" then do:

                    if iEntryLen = 1 then do:

                        do i = 2 to 1 by -1:

                            if iStrPos + i - 1 > iStrLen then next.

                            iDay = getIntStrict( substr( pcStr, iStrPos, i ) ).

                            if iDay = ? then
                                 next.
                            else leave.

                        end. /* 2 to 1 */

                        if i = 0 then return.

                        if iDay < 1 or iDay > 31 then return.

                        iStrPos = iStrPos + i.

                    end. /* len = 1 */

                    else
                    if iEntryLen = 2 then do:

                        if iStrPos + iEntryLen - 1 > iStrLen then return.

                        iDay = getIntStrict( substr( pcStr, iStrPos, iEntryLen ) ).

                        if iDay = ?
                        or iDay < 1 or iDay > 31 then return.

                        iStrPos = iStrPos + iEntryLen.

                    end. /* len = 2 */

                    else
                    iStrPos = iStrPos + iEntryLen.

                end. /* d */

                when "j" then do:

                    if iEntryLen = 1 then do:

                        do i = 3 to 1 by -1:

                            if iStrPos + i - 1 > iStrLen then next.

                            iYearDay = getIntStrict( substr( pcStr, iStrPos, i ) ).

                            if iYearDay = ? then
                                 next.
                            else leave.

                        end. /* 2 to 1 */

                        if i = 0 then return.

                        if iYearDay < 1 or iYearDay > 366 then return.

                        iStrPos = iStrPos + i.

                    end. /* len = 1 */

                    else
                    if iEntryLen = 3 then do:

                        if iStrPos + iEntryLen - 1 > iStrLen then return.

                        iYearDay = getIntStrict( substr( pcStr, iStrPos, iEntryLen ) ).

                        if iYearDay = ?
                        or iYearDay < 1 or iYearDay > 366 then return.

                        iStrPos = iStrPos + iEntryLen.

                    end. /* len = 3 */

                    else
                    iStrPos = iStrPos + iEntryLen.

                end. /* j */

                when "J" then do:

                    i = 0.

                    repeat:

                        ch = substr( pcStr, iStrPos + ( i + 1 ) - 1, 1 ).

                        if not ( ch >= "0" and ch <= "9" ) then
                            leave.

                        i = i + 1.

                        if iStrPos + i - 1 >= iStrLen then
                            leave.

                    end. /* repeat */

                    if i = 0 then
                        return.



                    x = dec( substr( pcStr, iStrPos, i ) ) no-error.

                    if error-status:num-messages > 0 then
                        return.

                    assign
                        iDay        = trunc( x / 86400000.0, 0 )
                        iMTime      = x - iDay * 86400000.0
                        tDate       = date( 1, 1, 1970 ) + iDay
                        iTimeZone   = 0.

                    iStrPos = iStrPos + i.

                end. /* J */

                when "y" then do:

                    if iEntryLen = 1 then do:

                        do i = 5 to 1 by -1:

                            if iStrPos + i - 1 > iStrLen then next.

                            iYear = getIntStrict( substr( pcStr, iStrPos, i ) ).

                            if iYear = ? then
                                 next.
                            else leave.

                        end. /* 5 to 1 */

                        if i = 0 then return.

                        iStrPos = iStrPos + i.

                    end. /* len = 1 */

                    else
                    if iEntryLen = 2 then do:

                        if iStrPos + iEntryLen - 1 > iStrLen then return.

                        iYear = getIntStrict( substr( pcStr, iStrPos, iEntryLen ) ).

                        if iYear = ?
                        or iYear < 1 or iYear > 99 then return.

                        assign
                            i = session:year-offset mod 100
                            j = session:year-offset - i.

                        if iYear >= i then
                             iYear = j + iYear.
                        else iYear = j + iYear + 100.

                        iStrPos = iStrPos + iEntryLen.

                    end. /* len = 2 */

                    else
                    if iEntryLen = 4 then do:

                        if iStrPos + iEntryLen - 1 > iStrLen then return.

                        iYear = getIntStrict( substr( pcStr, iStrPos, iEntryLen ) ).

                        if iYear = ? then return.

                        iStrPos = iStrPos + iEntryLen.

                    end. /* len = 4 */

                    else
                    iStrPos = iStrPos + iEntryLen.

                end. /* y */

                when "w" then do:

                    if iEntryLen = 1 then do:

                        iWeekDay = getIntStrict( substr( pcStr, iStrPos, iEntryLen ) ).

                        if iWeekDay = ?
                        or iWeekDay < 1 or iWeekDay > 7 then return.

                        iStrPos = iStrPos + iEntryLen.

                    end. /* len = 1 */

                    else
                    if iEntryLen = 3 then do:

                        if iStrPos + iEntryLen > iStrLen then return.

                        do i = 1 to 7:

                            if substr( pcStr, iStrPos, iEntryLen ) = substr( cWeekDay[i], 1, iEntryLen ) then
                                leave.

                        end. /* 1 to 7 */

                        if i = 8 then return.

                        assign
                            iWeekDay    = i
                            iStrPos     = iStrPos + iEntryLen.

                    end. /* len = 3 */

                    else
                    if iEntryLen = 5 then do:

                        do i = 1 to 7:

                            j = length( cWeekDay[i] ).

                            if iStrPos + j - 1 > iStrLen then next.

                            if substr( pcStr, iStrPos, j ) = cWeekDay[i] then leave.

                        end. /* 1 to 7 */

                        if i = 8 then return.

                        assign
                            iWeekDay    = i
                            iStrPos     = iStrPos + j.

                    end. /* len = 5 */

                    else
                    iStrPos = iStrPos + iEntryLen.

                end. /* w */

                /*** locale ***
                when "י" then do:

                    if iEntryLen = 1 then do:

                        iWeekDay = getIntStrict( substr( pcStr, iStrPos, iEntryLen ) ).

                        if iWeekDay = ?
                        or iWeekDay < 1 or iWeekDay > 7 then return.

                        iStrPos = iStrPos + iEntryLen.

                    end. /* len = 1 */

                    else
                    if iEntryLen = 3 then do:

                        iWeekDay = asc( substr( pcStr, iStrPos, 1 ) ) - asc( "א" ) + 1.

                        if iWeekDay = ?
                        or iWeekDay < 1 or iWeekDay > 7 then return.

                        iStrPos = iStrPos + 1.

                    end. /* len = 3 */

                    else
                    if iEntryLen = 5 then do:

                        do i = 1 to 7:

                            j = length( cWeekDay[i] ).

                            if iStrPos + j - 1 > iStrLen then next.

                            if substr( pcStr, iStrPos, j ) = cWeekDay[i] then leave.

                        end. /* 1 to 7 */

                        if i = 8 then return.

                        assign
                            iWeekDay    = i
                            iStrPos     = iStrPos + j.

                    end. /* len = 5 */

                    else
                    iStrPos = iStrPos + iEntryLen.

                end. /* י */
                *** locale ***/

                when "u" or when "U" then do:

                    if iEntryLen = 1 then do:

                        do i = 2 to 1 by -1:

                            if iStrPos + i - 1 > iStrLen then next.

                            iWeekNum = getIntStrict( substr( pcStr, iStrPos, i ) ).

                            if iWeekNum = ? then
                                 next.
                            else leave.

                        end. /* 2 to 1 */

                        if i = 0 then return.

                        if iWeekNum < 1 or iWeekNum > 53 then return.

                        iStrPos = iStrPos + i.

                    end. /* len = 1 */

                    else
                    if iEntryLen = 2 then do:

                        if iStrPos + iEntryLen - 1 > iStrLen then return.

                        iWeekNum = getIntStrict( substr( pcStr, iStrPos, iEntryLen ) ).

                        if iWeekNum = ?
                        or iWeekNum < 1 or iWeekNum > 53 then return.

                        iStrPos = iStrPos + iEntryLen.

                    end. /* len = 2 */

                    else
                    iStrPos = iStrPos + iEntryLen.

                    iWeekPerDay = ( if ch = "u" then 2 else 1 ).

                end. /* u or U */

                when "X" then do:

                    i = 0.

                    repeat:

                        ch = substr( pcStr, iStrPos + ( i + 1 ) - 1, 1 ).

                        if not ( ch >= "0" and ch <= "9" ) then
                            leave.

                        i = i + 1.

                        if iStrPos + i - 1 >= iStrLen then
                            leave.

                    end. /* repeat */

                    if i = 0 then
                        return.



                    x = dec( substr( pcStr, iStrPos, i ) ) no-error.

                    if error-status:num-messages > 0 then
                        return.

                    assign
                        iDay        = trunc( x / 86400.0, 0 )
                        iMTime      = x - iDay * 86400.0
                        iMTime      = iMTime * 1000
                        tDate       = date( 1, 1, 1970 ) + iDay
                        iTimeZone   = 0.

                    iStrPos = iStrPos + i.

                end. /* X */

                when "q" then do:

                    iQuarter = getIntStrict( substr( pcStr, iStrPos, iEntryLen ) ).

                    if iQuarter = ?
                    or iQuarter < 1 or iQuarter > 4 then return.

                    iStrPos = iStrPos + iEntryLen.

                end. /* q */

                when "h" or when "H" then do:

                    if iEntryLen = 1 then do:

                        do i = 2 to 1 by -1:

                            if iStrPos + i - 1 > iStrLen then next.

                            iHour = getIntStrict( substr( pcStr, iStrPos, i ) ).

                            if iHour = ? then
                                 next.
                            else leave.

                        end. /* 2 to 1 */

                        if i = 0 then return.

                        l12Hours = ( if ch = "H" then yes else no ).
                        if iHour > ( if l12Hours then 12 else 23 ) then return.

                        iStrPos = iStrPos + i.

                    end. /* len = 1 */

                    else
                    if iEntryLen = 2 then do:

                        if iStrPos + iEntryLen - 1 > iStrLen then return.

                        assign
                           l12Hours = ( if ch = "H" then yes else no )

                           iHour = getIntStrict( substr( pcStr, iStrPos, iEntryLen ) ).

                        if iHour = ?
                        or iHour > ( if l12Hours then 12 else 23 ) then return.

                        iStrPos = iStrPos + iEntryLen.

                    end. /* len = 2 */

                    else
                    if iEntryLen = 3 and ch = "h" then do:

                        do i = 3 to 1 by -1:

                            if iStrPos + i - 1 > iStrLen then next.

                            iHour = getIntStrict( substr( pcStr, iStrPos, i ) ).

                            if iHour = ? then
                                 next.
                            else leave.

                        end. /* 2 to 1 */

                        if i = 0 then return.

                        l12Hours = no.

                        iStrPos = iStrPos + i.

                    end. /* len = 1 */

                    else
                    iStrPos = iStrPos + iEntryLen.

                end. /* h or H */

                when "i" then do:

                    if iEntryLen = 1 then do:

                        do i = 2 to 1 by -1:

                            if iStrPos + i - 1 > iStrLen then next.

                            iMin = getIntStrict( substr( pcStr, iStrPos, i ) ).

                            if iMin = ? then
                                 next.
                            else leave.

                        end. /* 2 to 1 */

                        if i = 0 then return.

                        if iMin > 59 then return.

                        iStrPos = iStrPos + i.

                    end. /* len = 1 */

                    else
                    if iEntryLen = 2 then do:

                        if iStrPos + iEntryLen - 1 > iStrLen then return.

                        iMin = getIntStrict( substr( pcStr, iStrPos, iEntryLen ) ).

                        if iMin = ?
                        or iMin > 59 then return.

                        iStrPos = iStrPos + iEntryLen.

                    end. /* len = 2 */

                    else
                    iStrPos = iStrPos + iEntryLen.

                end. /* i */

                when "s" then do:

                    if iEntryLen = 1 then do:

                        do i = 2 to 1 by -1:

                            if iStrPos + i - 1 > iStrLen then next.

                            iSec = getIntStrict( substr( pcStr, iStrPos, i ) ).

                            if iSec = ? then
                                 next.
                            else leave.

                        end. /* 2 to 1 */

                        if i = 0 then return.

                        if iSec > 59 then return.

                        iStrPos = iStrPos + i.

                    end. /* len = 1 */

                    else
                    if iEntryLen = 2 then do:

                        if iStrPos + iEntryLen - 1 > iStrLen then return.

                        iSec = getIntStrict( substr( pcStr, iStrPos, iEntryLen ) ).

                        if iSec = ?
                        or iSec > 59 then return.

                        iStrPos = iStrPos + iEntryLen.

                    end. /* len = 2 */

                    else
                    iStrPos = iStrPos + iEntryLen.

                end. /* s */

                when "f" then do:

                    if iEntryLen <= 3 then do:

                        if iStrPos + iEntryLen - 1 > iStrLen then return.

                        iMSec = getIntStrict( substr( pcStr, iStrPos, iEntryLen ) ).

                        if iMSec = ? then return.

                        iStrPos = iStrPos + iEntryLen.

                    end. /* len <= 3 */

                    else
                    iStrPos = iStrPos + iEntryLen.

                end. /* f */

                when "p" or when "P" then do:

                    if iEntryLen = 2 then do:

                        if iStrPos + 1 > iStrLen then return.

                        case substr( pcStr, iStrPos, 2 ):

                            when "am" then lPm = no.
                            when "pm" then lPm = yes.

                            otherwise
                            return.

                        end case. /* substr */

                        iStrPos = iStrPos + 2.

                    end. /* len = 2 */

                    else
                    iStrPos = iStrPos + iEntryLen.

                end. /* p or P */

                /*** locale ***
                when "ע" then do:

                    if iEntryLen = 1 then do:

                        if iStrPos + 1 > iStrLen then return.

                        if substr( pcStr, iStrPos, 4 ) = "בוקר" then
                        assign
                            lPm     = no
                            iStrPos = iStrPos + 4.
                        
                        else
                        if substr( pcStr, iStrPos, 3 ) = "ערב" then
                        assign
                            lPm     = yes
                            iStrPos = iStrPos + 3.

                        else
                        return.

                    end. /* len = 2 */

                    else
                    iStrPos = iStrPos + iEntryLen.

                end. /* ע */
                *** locale ***/

                when "z" then do:

                    if iEntryLen = 3 then do:

                        str = substr( pcStr, iStrPos, iEntryLen ).

                        assign
                           iTimeZone = ?
                           iTimeZone =

                                int( substr( str, 1, 1 ) + "1" /* +1 or -1 */ ) *
                                int( substr( str, 2, 2 ) ) * 60 no-error.

                        if iTimeZone = ? then return.

                        iStrPos = iStrPos + iEntryLen.

                    end. /* len = 3 */

                    else
                    if iEntryLen = 6 or iEntryLen = 5 then do:

                        str = substr( pcStr, iStrPos, iEntryLen ).

                        assign
                           iTimeZone = ?
                           iTimeZone =

                                  int( substr( str, 1, 1 ) + "1" /* +1 or -1 */ ) *
                                ( int( substr( str, 2, 2 ) ) * 60
                                + int( substr( str, 

                                    ( if iEntryLen = 6 then 5 else 4 /* with or without ":" */ ), 2 ) ) ) no-error.

                        if iTimeZone = ? then return.

                        iStrPos = iStrPos + iEntryLen.

                    end. /* len = 6 */

                    else
                    iStrPos = iStrPos + iEntryLen.

                end. /* z */

            end case. /* ch */

        end. /* index > 0 */

        else do:

            assign
                iStrPos     = iStrPos + 1
                iFormPos    = iFormPos + 1.

        end. /* else */

    end. /* repeat */



    if tDate = ? then do:

        if iMonth <> ? and iDay <> ? and iYear <> ? then do:

            assign
               tDate = ?
               tDate = date( iMonth, iDay, iYear ) no-error.

            if tDate = ? then return.

        end. /* iMonth <> ? */

        else
        if iWeekNum <> ? and iWeekDay <> ? and iYear <> ? then do:

            tDate = date_getDateByWeek( iYear, iWeekNum, iWeekDay, iWeekPerDay ).

            if tDate = ? then return.

        end. /* iWeekNum <> ? */

        else
        if iYearDay <> ? and iYear <> ? then do: 

            tDate = date( 1, 1, iYear ) + iYearDay.

            if year( tDate ) > iYear then return.

        end. /* iYearDay <> ? */

        else
        if iMonth <> ? and iYear <> ? and iDay = ? then do:

            assign
               tDate = ?
               tDate = date( iMonth, 1, iYear ) no-error.

            if tDate = ? then return.

        end. /* iMonth <> ? */

        else
        if iWeekNum <> ? and iWeekDay = ? and iYear <> ? then do:

            tDate = date_getDateByWeek( iYear, iWeekNum, 1, iWeekPerDay ).

            if tDate = ? then return.

        end. /* iWeekNum <> ? */

    end. /* tDate <> ? */



    if iMTime = ? then do:

        if iHour <> ? and ( not l12Hours or lPm <> ? ) then do:

            iMTime = ( if l12Hours then ( iHour + ( if lPm then 12 else 0 ) ) else iHour ) * 3600000.

            if iMin <> ? then do: 

                iMTime = iMTime + iMin * 60000.

                if iSec <> ? then do:

                    iMTime = iMTime + iSec * 1000.

                    if iMSec <> ? then

                        iMTime = iMTime + iMSec.

                end. /* iSec > ? */

            end. /* iMin <> ? */

        end. /* iHour <> ? */

    end. /* iMTime <> ? */

    assign
        ptDate      = tDate
        piMTime     = iMTime
        piTimeZone  = iTimeZone.

    if piMTime >= 86400000 and ptDate <> ? then /* 24 * 60 * 60 * 1000 */

    assign
        ptDate      = ptDate + ( piMTime - piMTime mod 86400000 ) / 86400000
        piMTime     = piMTime mod 86400000.

end procedure. /* date_Str2Date */

function getIntStrict returns int private ( pcValue as char ):

    define var retval as int no-undo.

    if index( pcValue, " " ) > 0
    or index( pcValue, "?" ) > 0
    or index( pcValue, "+" ) > 0
    or index( pcValue, "-" ) > 0
    or index( pcValue, "*" ) > 0
    or index( pcValue, "," ) > 0
    or index( pcValue, "." ) > 0 then return ?.

    retval = int( pcValue ) no-error.

    if error-status:error then
         return ?.
    else return retval.

end function. /* getIntStrict */



function date_isDate returns log ( pcStr as char ):

    define var tDate as date no-undo.

    assign
       tDate = ?
       tDate = date( pcStr ) no-error.

    if tDate = ? then
         return no.
    else return yes.

end function. /* date_isDate */

&if {&pro_xProversion} >= "10" &then

function date_isDateTime returns log ( pcStr as char ):

    define var tDateTime as datetime no-undo.

    assign
       tDateTime = ?
       tDateTime = datetime( pcStr ) no-error.

    if tDateTime = ? then
         return no.
    else return yes.

end function. /* date_isDateTime */

function date_isDateTimeTz returns log ( pcStr as char ):

    define var tDateTimeTz as datetime-tz no-undo.

    assign
       tDateTimeTz = ?
       tDateTimeTz = datetime-tz( pcStr ) no-error.

    if tDateTimeTz = ? then
         return no.
    else return yes.

end function. /* date_isDateTime */

&endif /* proversion >= 10 */

function date_isDateTimeByFormat returns log ( pcStr as char, pcFormat as char ):

    define var tDate        as date no-undo.
    define var iMTime       as int no-undo.
    define var iTimeZone    as int no-undo.

    run date_Str2Date(

        input   pcStr,
        input   pcFormat,

        output  tDate,
        output  iMTime,
        output  iTimeZone ).

    /* if a time is specified and not included or invalid in the string the whole function fails and the 
       date returned equals ?. either it all works or nothing works. */

    return tDate <> ?.

end function. /* date_isDateTimeByFormat */

function date_isTime returns log ( pcStr as char ):

    define var tDate    as date no-undo.
    define var iTime    as int no-undo.
    define var iZone    as int no-undo.
    
    run date_str2date(
        input   pcStr,
        input   "hh:ii:ss",
        output  tDate,
        output  iTime,
        output  iZone ).
        
    if iTime <> ? then
        return yes.

    return no.
    
end function. /* date_isTime */



function date_getTimeInterval returns int ( ptHiDate as date, piHiTime as int, ptLoDate as date, piLoTime as int ):

    if ptHiDate = ? then
       ptHiDate = today.

    if piHiTime = ? then
       piHiTime = time.

    if ptLoDate = ? then
       ptLoDate = today.

    if piLoTime = ? then
       piLoTime = time.

    return ( ptHiDate - ptLoDate ) * 86400 + ( piHiTime - piLoTime ).

end function. /* date_getTimeInterval */

function date_getMTimeInterval returns int ( ptHiDate as date, piHiMTime as int, ptLoDate as date, piLoMTime as int ):

    if ptHiDate = ? then
       ptHiDate = today.

    if piHiMTime = ? then

    &if {&pro_xProversion} >= "10" &then 
       piHiMTime = mtime.
    &else
       piHiMTime = time * 1000.
    &endif

    if ptLoDate = ? then
       ptLoDate = today.

    if piLoMTime = ? then
    &if {&pro_xProversion} >= "10" &then 
       piLoMTime = mtime.
    &else
       piLoMTime = time * 1000.
    &endif

    return ( ptHiDate - ptLoDate ) * 86400000 + ( piHiMTime - piLoMTime ).

end function. /* date_getMTimeInterval */

function date_getMonthStart returns date ( piYear as int, piMonth as int ):

    define var t as date no-undo.

    if piMonth > 12 then
    assign
        piYear  = piYear + ( piMonth - piMonth mod 12 ) / 12
        piMonth = piMonth mod 12.

    else

    if piMonth < 1 then
    assign
        piYear  = piYear - ( ( abs( piMonth ) + 1 ) - ( abs( piMonth ) + 1 ) mod 12 ) / 12 - 1
        piMonth = 12 - ( abs( piMonth ) + 1 ) mod 12 + 1.

    t = date( piMonth, 1, piYear ) no-error.

    return t.

end function. /* date_getMonthStart */

function date_getMonthEnd returns date ( piYear as int, piMonth as int ):

    define var t as date no-undo.

    piMonth = piMonth + 1. /* next month ... */

    if piMonth > 12 then
    assign
        piYear  = piYear + ( piMonth - piMonth mod 12 ) / 12
        piMonth = piMonth mod 12.

    else

    if piMonth < 1 then
    assign
        piYear  = piYear - ( ( abs( piMonth ) + 1 ) - ( abs( piMonth ) + 1 ) mod 12 ) / 12 - 1
        piMonth = 12 - ( abs( piMonth ) + 1 ) mod 12 + 1.

    t = date( piMonth, 1, piYear ) - 1 /* ... minus one day */ no-error.

    return t.

end function. /* date_getMonthEnd */

function date_getEndOfMonth returns date ( piMonth as int, piYear as int ):

    return date_getMonthEnd( piYear, piMonth ).

end function. /* date_getEndOfMonth */



function date_getQuarter returns int ( ptDate as date ):

    define var i as int no-undo.

    i = month( ptDate ).

    return int( ( i - i mod 3 ) / 3 ) + ( if i mod 3 <> 0 then 1 else 0 ).

end function. /* date_getQuarter */

function date_getWeekNum returns int ( ptDate as date, piPerWeekDay as int ):

    define var retval   as int no-undo.
    define var t        as date no-undo.
    define var i        as int no-undo.

    /* start with the first week of the year then go to the start of that week (by piPerWeekDay). calculate week number from that starting point. */

    assign
       t = date( 1, 1, year( ptDate ) )
       i = weekday(t).

    if i > piPerWeekDay then
       t = t - ( i - piPerWeekDay ).

    else
    if i < piPerWeekDay then
       t = t - ( 7 - ( i - piPerWeekDay ) ).



    assign
       i = ptDate - t + 1

    retval = retval + int( ( i - i mod 7 ) / 7 ).

    if i mod 7 <> 0 then retval = retval + 1.

    return retval.

end function. /* date_getWeekNum */

function date_getDateByWeek returns date ( piYear as int, piWeekNum as int, piWeekDay as int, piPerWeekDay as int ):

    define var retval   as date no-undo.
    define var t        as date no-undo.
    define var i        as int no-undo.

    /* start with the first week of the year then go to the start of that week by piPerWeekDay. calculate date from that starting point. */

    assign
       t = date( 1, 1, piYear )
       i = weekday(t).

    if i > piPerWeekDay then
       t = t - ( i - piPerWeekDay ).

    else
    if i < piPerWeekDay then
       t = t - ( 7 - ( piPerWeekDay - i ) ).



    retval = t + ( piWeekNum - 1 ) * 7.

    if piWeekDay > piPerWeekDay then
        retval = retval + ( piWeekDay - piPerWeekDay ).

    else
    if piWeekDay < piPerWeekDay then
        retval = retval + ( 7 - ( piPerWeekDay - piWeekDay ) ).

    if year( retval ) <> piYear then return ?.

    return retval.

end function. /* getDateByWeek */

function date_getAge returns dec ( ptBirthday as date, ptDate as date ):

    define var tPrevBirthday    as date no-undo.
    define var tNextBirthday    as date no-undo.

    if ptDate = ? then
       ptDate = today.

    if  month( ptBirthDay ) = 2 and day( ptBirthDay ) = 29 then do:

        if ptDate >= date( 2, 28, year( ptDate ) ) + 1 then
        assign
            tPrevBirthday   = date( 2, 28, year( ptDate ) )     + 1
            tNextBirthday   = date( 2, 28, year( ptDate ) + 1 ) + 1.
    
        else
        assign
            tPrevBirthday   = date( 2, 28, year( ptDate ) - 1 ) + 1
            tNextBirthday   = date( 2, 28, year( ptDate ) )     + 1.

    end. /* feb 29 */

    else do:

        if ptDate >= date( month( ptBirthday ), day( ptBirthDay ), year( ptDate ) ) then
        assign
            tPrevBirthday   = date( month( ptBirthday ), day( ptBirthday ), year( ptDate ) )
            tNextBirthday   = date( month( ptBirthday ), day( ptBirthday ), year( ptDate ) + 1 ).
    
        else
        assign
            tPrevBirthday   = date( month( ptBirthday ), day( ptBirthday ), year( ptDate ) - 1 )
            tNextBirthday   = date( month( ptBirthday ), day( ptBirthday ), year( ptDate ) ).

    end. /* else */

    return year( tPrevBirthday ) - year( ptBirthday ) + ( ptDate - tPrevBirthday ) / ( tNextBirthday - tPrevBirthday ).

end function. /* date_getAge */



/* replaces mfg/pro hidate/lodate. since functions in queries are resolved once when the query is opened it has the same result and cleaner more elegant code */

function date_LoDate returns date ( ptDate as date ):

    if ptDate = ? then
         return 1/1/-32768.
    else return ptDate.

end function. /* date_LoDate */

function date_HiDate returns date ( ptDate as date ):

    if ptDate = ? then
         return 12/31/32767.
    else return ptDate.

end function. /* date_HiDate */

&if {&pro_xProversion} >= "10" &then

function date_LoDatetime returns datetime ( ptDatetime as datetime ):

    if ptDatetime = ? then
         return datetime( 1/1/-32768, 0 ).
    else return ptDatetime.

end function. /* date_LoDatetime */

function date_HiDatetime returns datetime ( ptDatetime as datetime ):

    if ptDatetime = ? then
         return datetime( 12/31/32767, 86399999 ).
    else return ptDatetime.

end function. /* date_HiDatetime */

function date_LoDatetimeTz returns datetime-tz ( ptDatetimeTz as datetime-tz ):

    if ptDatetimeTz = ? then
         return datetime-tz( 1/1/-32768, 0, -14 ).
    else return ptDatetimeTz.

end function. /* date_LoDatetimeTz */

function date_HiDatetimeTz returns datetime-tz ( ptDatetimeTz as datetime-tz ):

    if ptDatetimeTz = ? then
         return datetime-tz( 12/31/32767, 86399999, 14 ).
    else return ptDatetimeTz.

end function. /* date_HiDatetimeTz */

&endif /* proversion >= 10 */
