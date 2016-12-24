
/**
 * slibbidi_win9x.p -
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

define var cAlef    as char no-undo.
define var cTaf     as char no-undo.



on close of this-procedure do:

    delete procedure this-procedure.

end. /* close of this-procedure */

procedure initializeProc:

    if session:cpinternal begins "utf" then
    run bidi_setHebChars( chr(1488), chr(1522) ).

    else
    if session:cpinternal = "ibm862" then
    run bidi_setHebChars( chr(128), chr(154) ).

    else
 /* if session:cpinternal = "iso8859-8" then */
    run bidi_setHebChars( chr(224), chr(250) ).

end procedure. /* initializeProc */



{slib/slibbidifunc_win9x.i "Log2VisLtr"}
{slib/slibbidifunc_win9x.i "Log2VisRtl"}
{slib/slibbidifunc_win9x.i "VisLtr2Log"}
{slib/slibbidifunc_win9x.i "VisRtl2Log"}

{slib/slibbidifunc_win9x.i "Log2VisLtr" "Cur"}
{slib/slibbidifunc_win9x.i "Log2VisRtl" "Cur"}
{slib/slibbidifunc_win9x.i "VisLtr2Log" "Cur"}
{slib/slibbidifunc_win9x.i "VisRtl2Log" "Cur"}



procedure bidi_setHebChars:

    define input param pcAlef   as char no-undo.
    define input param pcTaf    as char no-undo.

    assign
        cAlef   = pcAlef
        cTaf    = pcTaf.

end procedure. /* bidi_setHebChars */
