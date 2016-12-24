
/**
 * slibmath.p -
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

{slib/slibmathfrwd.i "forward"}

{slib/slibmathprop.i}

{slib/slibstr.i}

{slib/sliberr.i}

{slib/slibpro.i}



on "close" of this-procedure do:

    delete procedure this-procedure.

end. /* on close */

procedure initializeProc:

end procedure. /* initializeProc */



function math_getShortNum returns char ( pdNum as dec, pcFormat as char ):

    define var cPrefix  as char no-undo.
    define var cSuffix  as char no-undo.

    define var iLen     as int no-undo.
    define var iExp     as int no-undo.
    define var cExp     as char no-undo.

    define var str      as char no-undo.
    define var i        as int no-undo.
    define var j        as int no-undo.

    str = string( pdNum, pcFormat ) no-error.

    if not error-status:num-messages >= 1 then return str.



    /* separate out prefix and suffix from format */

    i = 1.

    do while index( "-+>9.<", substr( pcFormat, i, 1 ) ) = 0 and i <= length( pcFormat ):
        i = i + 1.
    end.

    if i > 1 then 
    assign
        cPrefix = substr( pcFormat, 1, i - 1 )

        substr( pcFormat, 1, i - 1 ) = "".



    i = length( pcFormat ).

    do while index( "-+>9.<", substr( pcFormat, i, 1 ) ) = 0 and i > 1:
        i = i - 1.
    end.

    if i < length( pcFormat ) then
    assign
        cSuffix = substr( pcFormat, i + 1, length( pcFormat ) - i )

        substr( pcFormat, i + 1, length( pcFormat ) - i ) = "".



    assign
        iLen    = length( cPrefix ) 
                + length( right-trim( pcFormat, "<" ) ) 
                + length( cSuffix )

        cExp    = ""
        iExp    = 0. /* exp( 10, 0 ) = 1 */

    _Short:

    repeat:

           /* decimal datatype limit 12 345 678 901 234 567 890 123 456 789 012 345 678 901 234 567 890 1234567890 */
        str = trim( string( pdNum, "->>,>>>,>>>,>>>,>>>,>>>,>>>,>>>,>>>,>>>,>>>,>>>,>>>,>>>,>>>,>>>,>>9.9<<<<<<<<<" ) ).

        if substr( str, length( str ) - 1, 2 ) = ".0" then
           substr( str, length( str ) - 1, 2 ) = "".

        str = cPrefix + str + cExp + cSuffix.

        if length( str ) <= iLen then leave _Short.



        /* remove thousand separators */

        repeat:

            i = index( str, "," ).

            if i = 0 then leave.

            substr( str, i, 1 ) = "".

            if length( str ) <= iLen then leave _Short.

        end. /* repeat */



        /* round decimal places */

        repeat:

            assign
               i = index( str, "." )
               j = length( str ) - length( cExp ) - length( cSuffix ).

            if i = 0 then leave.


                                   /* decimal datatype limit 12345678901234567890123456789012345678901234567890 1234567890 */
            str = trim( string( round( pdNum, j - i - 1 ), "->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>9.9<<<<<<<<<" ) ).

            if substr( str, length( str ) - 1, 2 ) = ".0" then
               substr( str, length( str ) - 1, 2 ) = "".

            str = cPrefix + str + cExp + cSuffix.

            if length( str ) <= iLen then leave _Short.

        end. /* repeat */



        /* divide by 1000 */

        if abs( pdNum ) <= 1 then leave _Short.

        assign
            pdNum   = pdNum / 1000.

            iExp    = iExp + 3.

        case iExp:

            when 3  then cExp = "K".
            when 6  then cExp = "M".
            when 9  then cExp = "B".
            when 12 then cExp = "T".

            otherwise
            cExp = "e" + string( iExp ).

        end case. /* iExp */

    end. /* repeat */



    str = str_alignRight( str, iLen, "" ).

    return str.

end function. /* math_getShortNum */



define temp-table ttCalc no-undo

    field iX as int.

define var dCalcRetVal as dec no-undo.

function math_calc returns dec ( pcExpression as char ):

    /* the function does not throw exceptions. if an error ocurred the functions returns a null value. */

    define buffer ttCalc for ttCalc.

    define var hQuery as handle no-undo.

    {slib/err_try}:

        create query hQuery.

        hQuery:set-buffers( buffer ttCalc:handle ).
        hQuery:query-prepare( 

            "for each ttCalc " +
                "where dynamic-function( 'setCalcRetVal', " + pcExpression + " ) <> ''" ) {slib/err_no-error}.

        hQuery:query-open( ) {slib/err_no-error}.

    {slib/err_catch}:

        {slib/err_return ?}.

    {slib/err_finally}:

        hQuery:query-close( ) no-error.
        delete object hQuery no-error.

    {slib/err_end}.

    return dCalcRetVal.

end function. /* math_calc */

function setCalcRetVal returns log private ( pdValue as dec ):

    dCalcRetVal = pdValue.

end function. /* setCalcRetVal */



function math_tan returns dec ( pdRadian as dec ):

    return math_sin( pdRadian ) / math_cos( pdRadian ).

end function. /* math_tan */

function math_sin returns dec ( pdRadian as dec ):

    define var dXrad    as dec no-undo.
    define var dDelta   as dec no-undo.
    define var dResult  as dec no-undo.
    define var n        as int no-undo.

    assign
        dXrad       = pdRadian
        dDelta      = dXrad
        dResult     = dXrad
        n           = 0.

    repeat:

        assign
            n       = n + 1
            dDelta  = ( - dDelta ) * ( dXrad * dXrad ) / ( ( 2 * n ) * ( 2 * n + 1 ) )
            dResult = dResult + dDelta.

        if abs( dDelta)             < 0.0000000001  /* epsilon */
        or abs( dResult / dDelta )  > 10000000000.0 /* places */

            then leave.

    end. /* repeat */

    return dResult.

end function. /* math_sin */

function math_cos returns dec ( pdRadian as dec ):

    define var dXrad    as dec no-undo.
    define var dDelta   as dec no-undo.
    define var dResult  as dec no-undo.
    define var n        as int no-undo.

    assign
        dXrad       = pdRadian
        dDelta      = 1
        dResult     = 1
        n           = 0.

    repeat:

        assign
            n       = n + 1
            dDelta  = ( - dDelta ) * ( dXrad * dXrad ) / ( ( 2 * n - 1 ) * ( 2 * n ) )
            dResult = dResult + dDelta.

        if abs( dDelta)             < 0.0000000001  /* epsilon */
        or abs( dResult / dDelta )  > 10000000000.0 /* places */

            then leave.

    end. /* repeat */

    return dResult.

end function. /* math_cos */

function math_deg2rad returns dec ( pdDegree as dec ):

    return {&math_xPi} * pdDegree / 180.

end function. /* math_dec2rad */



/***
function math_tan returns dec ( dRadian as dec ):

    define var dRetVal as dec no-undo.

    run tan( dRadian, output dRetVal ) no-error.

    return dRetVal.

end function. /* math_tan */

function math_tanByDeg returns dec ( dDegree as dec ):

    return math_tan( dDegree * {&math_xPi} / 180 ).

end function. /* math_tanByDeg */

function math_atan returns dec ( dRadian as dec ):

    define var dRetVal as dec no-undo.

    run atan( dRadian, output dRetVal ) no-error.

    return dRetVal.

end function. /* math_atan */

function math_atanByDeg returns dec ( dDegree as dec ):

    return math_atan( dDegree * {&math_xPi} / 180 ).

end function. /* math_atanByDeg */

function math_sin returns dec ( dRadian as dec ):

    define var dRetVal as dec no-undo.

    run sin( dRadian, output dRetVal ) no-error.

    return dRetVal.

end function. /* math_sin */

function math_sinByDeg returns dec ( dDegree as dec ):

    return math_sin( dDegree * {&math_xPi} / 180 ).

end function. /* math_sinByDeg */

function math_sinh returns dec ( dRadian as dec ):

    define var dRetVal as dec no-undo.

    run sinh( dRadian, output dRetVal ) no-error.

    return dRetVal.

end function. /* math_sinh */

function math_sinhByDeg returns dec ( dDegree as dec ):

    return math_sinh( dDegree * {&math_xPi} / 180 ).

end function. /* math_sinhByDeg */

function math_asin returns dec ( dRadian as dec ):

    define var dRetVal as dec no-undo.

    run asin( dRadian, output dRetVal ) no-error.

    return dRetVal.

end function. /* math_asin */

function math_asinByDeg returns dec ( dDegree as dec ):

    return math_asin( dDegree * {&math_xPi} / 180 ).

end function. /* math_asinByDeg */

function math_cos returns dec ( dRadian as dec ):

    define var dRetVal as dec no-undo.

    run cos( dRadian, output dRetVal ) no-error.

    return dRetVal.

end function. /* math_cos */

function math_cosByDeg returns dec ( dDegree as dec ):

    return math_cos( dDegree * {&math_xPi} / 180 ).

end function. /* math_cosByDeg */

function math_cosh returns dec ( dRadian as dec ):

    define var dRetVal as dec no-undo.

    run cosh( dRadian, output dRetVal ) no-error.

    return dRetVal.

end function. /* math_cosh */

function math_coshByDeg returns dec ( dDegree as dec ):

    return math_cosh( dDegree * {&math_xPi} / 180 ).

end function. /* math_coshByDeg */

function math_acos returns dec ( dRadian as dec ):

    define var dRetVal as dec no-undo.

    run acos( dRadian, output dRetVal ) no-error.

    return dRetVal.

end function. /* math_acos */

function math_acosByDeg returns dec ( dDegree as dec ):

    return math_acos( dDegree * {&math_xPi} / 180 ).

end function. /* math_acosByDeg */



&if "{&opsys}" begins "win" &then

    &global xLib 'msvcrt40.dll'

&else

    &global xLib '/usr/lib/libm.so' /* /usr/lib/libm.1 */

&endif

procedure tan external {&xLib} cdecl:

    define input    param dblRadian as double no-undo.
    define return   param dblRetVal as double no-undo.

end procedure. /* tan */

procedure atan external {&xLib} cdecl:

    define input    param dblRadian as double no-undo.
    define return   param dblRetVal as double no-undo.

end procedure. /* atan */

procedure sin external {&xLib} cdecl:

    define input    param dblRadian as double no-undo.
    define return   param dblRetVal as double no-undo.

end procedure. /* sin */

procedure sinh external {&xLib} cdecl:

    define input    param dblRadian as double no-undo.
    define return   param dblRetVal as double no-undo.

end procedure. /* sinh */

procedure asin external {&xLib} cdecl:

    define input    param dblRadian as double no-undo.
    define return   param dblRetVal as double no-undo.

end procedure. /* asin */

procedure cos external {&xLib} cdecl:

    define input    param dblRadian as double no-undo.
    define return   param dblRetVal as double no-undo.

end procedure. /* cos */

procedure cosh external {&xLib} cdecl:

    define input    param dblRadian as double no-undo.
    define return   param dblRetVal as double no-undo.

end procedure. /* cosh */

procedure acos external {&xLib} cdecl:

    define input    param dblRadian as double no-undo.
    define return   param dblRetVal as double no-undo.

end procedure. /* acos */

&undefine xLib
***/



function math_ceil returns int ( pdValue as dec ):

    define var i as int no-undo.

    i = trunc( pdValue, 0 ).

    if i = pdValue then
        return i.

    else
    if i > 0 then
        return i + 1.

    else
    if i < 0 then
        return i.

end function. /* math_ceil */

function math_floor returns int ( pdValue as dec ):

    define var i as int no-undo.

    i = trunc( pdValue, 0 ).

    if i = pdValue then
        return i.

    else
    if i > 0 then
        return i.

    else
    if i < 0 then
        return i - 1.

end function. /* math_floor */



function math_and returns {&pro_xBigInt} ( piValue1 as {&pro_xBigInt}, piValue2 as {&pro_xBigInt} ):

    define var retval   as {&pro_xBigInt} no-undo.
    define var i        as int no-undo.

    do i = 1 to &if "{&pro_xBigInt}" = "int64" &then 64 &else 32 &endif:

        put-bits( retval, i, 1 ) =

            ( if  get-bits( piValue1, i, 1 ) = 1
              and get-bits( piValue2, i, 1 ) = 1 then 1 else 0 ).

    end. /* 1 to 32 */

    return retval.

end function. /* math_and */

function math_or returns {&pro_xBigInt} ( piValue1 as {&pro_xBigInt}, piValue2 as {&pro_xBigInt} ):

    define var retval   as {&pro_xBigInt} no-undo.
    define var i        as int no-undo.

    do i = 1 to &if "{&pro_xBigInt}" = "int64" &then 64 &else 32 &endif:

        put-bits( retval, i, 1 ) =

            ( if get-bits( piValue1, i, 1 ) = 1
              or get-bits( piValue2, i, 1 ) = 1 then 1 else 0 ).

    end. /* 1 to 32 */

    return retval.

end function. /* math_or */

function math_xor returns {&pro_xBigInt} ( piValue1 as {&pro_xBigInt}, piValue2 as {&pro_xBigInt} ):

    define var retval   as {&pro_xBigInt} no-undo.
    define var i        as int no-undo.

    do i = 1 to &if "{&pro_xBigInt}" = "int64" &then 64 &else 32 &endif:

        put-bits( retval, i, 1 ) =

            ( if get-bits( piValue1, i, 1 ) = 1
              or get-bits( piValue2, i, 1 ) = 1 

              and not ( get-bits( piValue1, i, 1 ) = 1
                    and get-bits( piValue2, i, 1 ) = 1 )

                then 1 else 0 ).

    end. /* 1 to 32 */

    return retval.

end function. /* math_xor */

function math_not returns {&pro_xBigInt} ( piValue as {&pro_xBigInt} ):

    define var retval   as {&pro_xBigInt} no-undo.
    define var i        as int no-undo.

    do i = 1 to &if "{&pro_xBigInt}" = "int64" &then 64 &else 32 &endif:

        put-bits( retval, i, 1 ) = 

            ( if get-bits( piValue, i, 1 ) = 1 then 0 else 1 ).

    end. /* 1 to 32 */

    return retval.

end function. /* math_not */



function math_Hex2Int returns {&pro_xBigInt} ( pcHex as char ):

    /* the function does not throw exceptions. if an error ocurred the functions returns a null value. */

    define var iPos         as int no-undo.
    define var iOffset      as int no-undo.
    define var lNegative    as log no-undo.

    define var retval       as {&pro_xBigInt} no-undo.
    define var i            as int no-undo.

    if pcHex = ? then return ?.

    if pcHex begins "-" then

    assign
        substr( pcHex, 1, 1 )   = ""
        lNegative               = yes.

    if pcHex begins "0x" then

        substr( pcHex, 1, 2 )   = "".



    assign
        iPos    = length( pcHex )
        iOffset = 1.

    repeat while iPos >= 1:

        i = index( "0123456789ABCDEF", substr( pcHex, iPos, 1 ) ) - 1.

        if i = -1 then return ?.

        put-bits( retval, iOffset, 4 ) = i.

        assign
            iPos    = iPos - 1
            iOffset = iOffset + 4.

    end. /* repeat */

    return ( if lNegative then -1 * retval else retval ).

end function. /* math_Hex2Int */

function math_Int2Hex returns char ( piInt as {&pro_xBigInt} ):

    define var iOffset      as int no-undo.
    define var lNegative    as log no-undo.

    define var retval       as char no-undo.
    define var i            as int no-undo.

    if piInt = ? then return ?.

    if piInt < 0 then

    assign
        piInt       = abs( piInt )
        lNegative   = yes.



    iOffset = &if "{&pro_xBigInt}" = "int64" &then 61 &else 29 &endif. /* num of bits - 3 */

    repeat while iOffset >= 1:

        i = get-bits( piInt, iOffset, 4 ).

        if not ( i = 0 and retval = "" ) then

            retval = retval + substr( "0123456789ABCDEF", i + 1, 1 ). 

        iOffset = iOffset - 4.

    end. /* repeat */

    return ( if lNegative then "-" else "" ) + ( if retval = "" then "0" else retval ).

end function. /* math_Int2Hex */

function math_Int642Dec returns dec ( ppInt64 as memptr ):
 
    &scoped xBigInt 4294967296 /* 2^32 */
 
    define var x1 as dec no-undo.
    define var x2 as dec no-undo.
 
    x1 = get-long( ppInt64, 1 ).
    if x1 < 0 then x1 = x1 + {&xBigInt}.
 
    x2 = get-long( ppInt64, 5).
    if x2 < 0 then x2 = x2 + {&xBigInt}.
 
    return x1 + ( x2 * {&xBigInt} ).

    &undefine xBigInt

end function. /* math_Int642Dec */



/* personally i find isNum( ) clearer then isNaN( ) but isNaN( ) is a very commonly used function. */

function math_isNaN returns log ( pcValue as char ):

    return not math_isNum( pcValue ).

end function. /* isNaN */

function math_isNum returns log ( pcValue as char ):

    define var x as dec no-undo.

    pcValue = trim( pcValue ).

    /* although int( ) accepts right side +- signs and a leading * acts like a plus sign
       do not treat these values as valid numbers and like wise do the same for blanks and nulls. */

    if pcValue = "" or pcValue = ? or pcValue = "?"

    or pcValue begins "*"
    or index( "+-", substr( pcValue, length( pcValue ), 1 ) ) > 0 then

        return no.

    x = dec( pcValue ) no-error.

    if error-status:num-messages > 0 then
         return no.
    else return yes.

end function. /* math_isNum */

function math_isDec returns log ( pcValue as char ):

    return math_isNum( pcValue ).

end function. /* isInt */

function math_isInt returns log ( pcValue as char ):

    if index( pcValue, session:numeric-decimal-point ) > 0 then
        return no.

    return math_isNum( pcValue ).

end function. /* isInt */

function math_isAllNumChars returns log ( pcValue as char ):

    define var i as int no-undo.

    pcValue = right-trim( pcValue ). /* do not take into account right side blanks. blank on the left are not legal */

    do i = 1 to length( pcValue ):

        if not ( substr( pcValue, i, 1 ) >= "0"
             and substr( pcValue, i, 1 ) <= "9" ) then

            return no.

    end. /* 1 to length */

    return yes.

end function. /* isInt */

function math_isLog returns log ( pcValue as char ):

    pcValue = trim( pcValue ).

    case pcValue:

        when "true" or
        when "false" or 
        when "yes" or
        when "no" then
        return yes.

        otherwise
        return no.

    end case. /* pcXLog */

end function. /* math_isLog */



/* replaces mfg/pro hidate/lodate. since functions in queries are resolved once when the query is opened it has the same result and cleaner more elegant code */

function math_HiInt returns int ( piValue as int ):

    if piValue = 0 then
         return 2147483647.
    else return piValue.

end function. /* math_HiInt */

function math_LoInt returns int ( piValue as int ):

    if piValue = 0 then
         return -2147483648.
    else return piValue.

end function. /* math_LoInt */

function math_HiDec returns dec ( pdValue as dec ):

    if pdValue = 0 then
         return 9999999999999999999999999999999999999999999999999.9.
    else return pdValue.

end function. /* math_HiDec */

function math_LoDec returns dec ( pdValue as dec ):

    if pdValue = 0 then
         return -9999999999999999999999999999999999999999999999999.9.
    else return pdValue.

end function. /* math_LoDec */

&if {&pro_xProversion} >= "10.1b" &then

function math_HiInt64 returns int64 ( piValue as int64 ):

    if piValue = 0 then
         return 9223372036854775807.
    else return piValue.

end function. /* math_HiInt64 */

function math_LoInt64 returns int64 ( piValue as int64 ):

    if piValue = 0 then
         return -9223372036854775808.
    else return piValue.

end function. /* math_LoInt64 */

&endif /* proversion >= 10.1b */
