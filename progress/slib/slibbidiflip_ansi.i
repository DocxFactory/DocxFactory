
/**
 * slibbidiflip_ansi.i -
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



assign
    cFlipStr    = &if "{1}" <> "" &then {1} &else "" &endif
    iFlipPos    = iPos

    &if {&xCursor} &then

    iFlipCur    = &if "{1}" <> "" &then iNumRetCur &else ? &endif
    iFlipRetCur = ?

    &endif

    cFlipRetVal = cFlipStr.

repeat while

        &if     "{&xRead}" = "ltr" &then iPos <= iLen
        &elseif "{&xRead}" = "rtl" &then iPos >= 1
        &endif:

    ch = substr( pcStr, iPos, 1 ).

    if  &if     "{&xLangMatch}" = "heb" &then {slib/slibbidirange.i ch cAlef cTaf}
        &elseif "{&xLangMatch}" = "eng" &then {slib/slibbidirange.i ch "'a'" "'z'"}
        &endif then

        leave.



    else
    if {slib/slibbidirange.i ch "'0'" "'9'"}
    or ( ch = "#" or ch = "$" or ch = "%" )

    and &if     "{&xRead}" = "ltr" &then iPos < iLen    and {slib/slibbidirange.i "substr( pcStr, iPos + 1, 1 )" "'0'" "'9'"}
        &elseif "{&xRead}" = "rtl" &then iPos > 1       and {slib/slibbidirange.i "substr( pcStr, iPos - 1, 1 )" "'0'" "'9'"}
        &endif 

        then do:

        {slib/slibbidinum_ansi.i}

        &if not {&xCursor} &then

            &if     "{&xDispFlip}" = "ltr" &then cFlipStr = cFlipStr + cNumRetVal.
            &elseif "{&xDispFlip}" = "rtl" &then cFlipStr = cNumRetVal + cFlipStr.
            &endif

        &else /* not xCursor */

            &if     "{&xDispFlip}" = "ltr" &then

                assign
                    iFlipCur = iNumRetCur + length( cFlipStr )  when iNumRetCur <> ?

                    cFlipStr = cFlipStr + cNumRetVal.

            &elseif "{&xDispFlip}" = "rtl" &then

                assign
                    iFlipCur = iNumRetCur                       when iNumRetCur <> ?
                    iFlipCur = iFlipCur + length( cNumRetVal )  when iFlipCur   <> ?

                    cFlipStr = cNumRetVal + cFlipStr.

            &endif /* rtl */

        &endif /* else */

        assign
            iFlipPos    = iPos
            cFlipRetVal = cFlipStr

            &if {&xCursor} &then iFlipRetCur = iFlipCur &endif.

    end. /* num */



    else do:

        &if "{&xRead}" <> "{&xDispFlip}" &then

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

        &endif /* read <> write */

        assign

            &if     "{&xDispFlip}" = "ltr" &then cFlipStr = cFlipStr + ch
            &elseif "{&xDispFlip}" = "rtl" &then cFlipStr = ch + cFlipStr
            &endif

            &if {&xCursor} &then

                &if     "{&xDispFlip}" = "ltr" &then

                    iFlipCur = length( cFlipStr )   when iFlipCur  = ? and iPos = piCur

                &elseif "{&xDispFlip}" = "rtl" &then

                    iFlipCur = 1                    when iFlipCur  = ? and iPos = piCur
                    iFlipCur = iFlipCur + 1         when iFlipCur <> ?

                &endif /* rtl */

            &endif /* xCursor */

            &if     "{&xRead}" = "ltr" &then iPos = iPos + 1
            &elseif "{&xRead}" = "rtl" &then iPos = iPos - 1
            &endif.

        if  &if     "{&xLangFlip}" = "heb" &then {slib/slibbidirange.i ch cAlef cTaf}
            &elseif "{&xLangFlip}" = "eng" &then {slib/slibbidirange.i ch "'a'" "'z'"}
            &endif then

        assign
            iFlipPos    = iPos
            cFlipRetVal = cFlipStr

            &if {&xCursor} &then iFlipRetCur = iFlipCur &endif.

    end. /* else */

end. /* repeat */

iPos = iFlipPos.
