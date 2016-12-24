
/**
 * slibbidinum_ansi.i -
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
    cNumRetVal  = ""

    &if {&xCursor} &then iNumRetCur = ? &endif.

repeat while

    &if     "{&xRead}" = "ltr" &then iPos <= iLen
    &elseif "{&xRead}" = "rtl" &then iPos >= 1
    &endif:

    ch = substr( pcStr, iPos, 1 ).

    if {slib/slibbidirange.i ch "'0'" "'9'"}
    or ( ch = "#" or ch = "$" or ch = "%" )
    or ( ch = "," or ch = "." or ch = ":" or ch = "/" )

    and &if     "{&xRead}" = "ltr" &then iPos < iLen    and {slib/slibbidirange.i "substr( pcStr, iPos + 1, 1 )" "'0'" "'9'"} 
        &elseif "{&xRead}" = "rtl" &then iPos > 1       and {slib/slibbidirange.i "substr( pcStr, iPos - 1, 1 )" "'0'" "'9'"}
        &endif then

    assign

        &if     "{&xDispNum}" = "ltr" &then cNumRetVal = cNumRetVal + ch
        &elseif "{&xDispNum}" = "rtl" &then cNumRetVal = ch + cNumRetVal
        &endif

        &if {&xCursor} &then

            &if     "{&xDispNum}" = "ltr" &then

                iNumRetCur = length( cNumRetVal )   when iNumRetCur  = ? and iPos = piCur

            &elseif "{&xDispNum}" = "rtl" &then

                iNumRetCur = 1                      when iNumRetCur  = ? and iPos = piCur
                iNumRetCur = iNumRetCur + 1         when iNumRetCur <> ?

            &endif /* rtl */

        &endif /* xCursor */

        &if     "{&xRead}" = "ltr" &then iPos = iPos + 1
        &elseif "{&xRead}" = "rtl" &then iPos = iPos - 1
        &endif.

    else leave.

end. /* repeat */
