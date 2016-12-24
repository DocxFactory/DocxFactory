
/**
 * slibbidifunc_win9x.i -
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



&global xFunc {1}

&if "{2}" = "Cur" &then

    &global xCursor yes

&else

    &global xCursor no

&endif



&if     "{&xFunc}" = "Log2VisLtr" &then

    &scoped xLangMatch  eng
    &scoped xLangFlip   heb

    &scoped xRead       ltr
    &scoped xDispMatch  ltr
    &scoped xDispFlip   rtl
    &scoped xDispNum    ltr



&elseif "{&xFunc}" = "Log2VisRtl" &then

    &scoped xLangMatch  heb
    &scoped xLangFlip   eng

    &scoped xRead       ltr
    &scoped xDispMatch  rtl
    &scoped xDispFlip   ltr
    &scoped xDispNum    ltr



&elseif "{&xFunc}" = "VisLtr2Log" &then

    &scoped xLangMatch  eng
    &scoped xLangFlip   heb

    &scoped xRead       ltr
    &scoped xDispMatch  ltr
    &scoped xDispFlip   rtl
    &scoped xDispNum    ltr



&elseif "{&xFunc}" = "VisRtl2Log" &then

    &scoped xLangMatch  heb
    &scoped xLangFlip   eng

    &scoped xRead       rtl
    &scoped xDispMatch  ltr
    &scoped xDispFlip   rtl
    &scoped xDispNum    rtl

&endif



&if not {&xCursor} &then

function bidi_{&xFunc}      returns char ( pcStr as char ):

&else

function bidi_{&xFunc}Cur   returns char ( pcStr as char, piCur as int, output piRetCur as int ):

&endif

    define var cRetVal      as char no-undo.
    define var iLen         as int no-undo.
    define var iPos         as int no-undo.
    define var ch           as char no-undo.

    /* slibbidiflip_win9x.i variables */

    define var cFlipRetVal  as char no-undo.
    define var cFlipStr     as char no-undo.
    define var iFlipPos     as int no-undo.

    &if {&xCursor} &then

    define var iFlipCur     as int no-undo.
    define var iFlipRetCur  as int no-undo.

    &endif

    /* slibbidinum_win9x.i variables */

    define var cNumRetVal   as char no-undo.

    &if {&xCursor} &then

    define var iNumRetCur   as int no-undo.

    &endif

    if  session:cpinternal <> "iso8859-8"
    and session:cpinternal <> "ibm862" then return pcStr.



    assign
        iLen = length( pcStr )
        iPos = 

            &if     "{&xRead}" = "ltr" &then 1
            &elseif "{&xRead}" = "rtl" &then iLen
            &endif

        cRetVal = ""

        &if {&xCursor} &then piRetCur = ? &endif.

    repeat while

            &if     "{&xRead}" = "ltr" &then iPos <= iLen
            &elseif "{&xRead}" = "rtl" &then iPos >= 1
            &endif:

        ch = substr( pcStr, iPos, 1 ).

        if  &if     "{&xLangFlip}" = "heb" &then {slib/slibbidirange.i ch cAlef cTaf}
            &elseif "{&xLangFlip}" = "eng" &then {slib/slibbidirange.i ch "'a'" "'z'"}
            &endif

            then do:

            {slib/slibbidiflip_win9x.i}

            &if not {&xCursor} &then

                &if     "{&xDispMatch}" = "ltr" &then cRetVal = cRetVal + cFlipRetVal.
                &elseif "{&xDispMatch}" = "rtl" &then cRetVal = cFlipRetVal + cRetVal.
                &endif

            &else /* not xCursor */

                &if     "{&xDispMatch}" = "ltr" &then

                    assign
                        piRetCur    = iFlipRetCur + length( cRetVal )   when iFlipRetCur    <> ? 

                        cRetVal     = cRetVal + cFlipRetVal.

                &elseif "{&xDispMatch}" = "rtl" &then

                    assign
                        piRetCur    = iFlipRetCur                       when iFlipRetCur    <> ?
                        piRetCur    = piRetCur + length( cFlipRetVal )  when piRetCur       <> ?

                        cRetVal     = cFlipRetVal + cRetVal.

                &endif /* rtl */

            &endif /* else */

        end. /* xLangFlip */



        else
        if {slib/slibbidirange.i ch "'0'" "'9'"}
        or ( ch = "#" or ch = "$" or ch = "%" )

        and &if     "{&xRead}" = "ltr" &then iPos < iLen    and {slib/slibbidirange.i "substr( pcStr, iPos + 1, 1 )" "'0'" "'9'"}
            &elseif "{&xRead}" = "rtl" &then iPos > 1       and {slib/slibbidirange.i "substr( pcStr, iPos - 1, 1 )" "'0'" "'9'"}
            &endif 

            then do:

            {slib/slibbidinum_win9x.i}

            &if not {&xCursor} &then

                &if     "{&xDispMatch}" = "ltr" &then cRetVal = cRetVal + cNumRetVal.
                &elseif "{&xDispMatch}" = "rtl" &then cRetVal = cNumRetVal + cRetVal.
                &endif

            &else /* not xCursor */

                &if     "{&xDispMatch}" = "ltr" &then

                    assign
                        piRetCur    = iNumRetCur + length( cRetVal )    when iNumRetCur <> ?

                        cRetVal     = cRetVal + cNumRetVal.

                &elseif "{&xDispMatch}" = "rtl" &then

                    assign
                        piRetCur    = iNumRetCur                        when iNumRetCur <> ?
                        piRetCur    = piRetCur + length( cNumRetVal )   when piRetCur   <> ?

                        cRetVal     = cNumRetVal + cRetVal.

                &endif /* rtl */

            &endif /* else */

        end. /* 0..9 */



        else do:

            &if "{&xRead}" <> "{&xDispMatch}" &then

                case ch:

                    when "("    then ch = ")".
                    when ")"    then ch = "(".
                    when "["    then ch = "]".
                    when "]"    then ch = "[".
                    when "~{"   then ch = "~}".
                    when "~}"   then ch = "~{".
                    when ">"    then ch = "<".
                    when "<"    then ch = ">".

                 end case. /* ch */

            &endif /* read <> disp */

            assign

                &if     "{&xDispMatch}" = "ltr" &then cRetVal = cRetVal + ch
                &elseif "{&xDispMatch}" = "rtl" &then cRetVal = ch + cRetVal
                &endif

                &if {&xCursor} &then

                    &if     "{&xDispMatch}" = "ltr" &then

                        piRetCur = length( cRetVal )    when piRetCur  = ? and iPos = piCur

                    &elseif "{&xDispMatch}" = "rtl" &then

                        piRetCur = 1                    when piRetCur  = ? and iPos = piCur
                        piRetCur = piRetCur + 1         when piRetCur <> ?

                    &endif /* rtl */

                &endif /* xCursor */

                &if     "{&xRead}" = "ltr" &then iPos = iPos + 1
                &elseif "{&xRead}" = "rtl" &then iPos = iPos - 1
                &endif.

        end. /* else */

    end. /* repeat */

    return cRetVal.

end function. /* bidi_{&xFunc} */
