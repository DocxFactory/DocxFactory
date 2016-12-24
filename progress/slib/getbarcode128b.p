
/**
 * getbarcode128b.p -
 *
 * the barcode string is compatible with the code128bwin font and has to be used together 
 * with that font (there is a copy of the font in the slib/bin dir).
 *
 * http://www.barcodeisland.com/code128.phtml was a great help !
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
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Contact information
 *  Email: alonblich@gmail.com
 *  Phone: +972-54-218-8086
 */

define input    param pcValue   as char no-undo.
define output   param pcBarcode as char no-undo.

define var cCharset     as char no-undo.
define var cZeroChar    as char no-undo.
define var iCheckSum    as int no-undo.

define var str          as char no-undo.
define var ch           as char no-undo.
define var i            as int no-undo.
define var j            as int no-undo.



/* i opted to use a var instead of a preprocessor because of the ~ issues */

assign
    cCharset    = "!~"#$%~&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[~\]^_`abcdefghijklmnopqrstuvwxyz~{|~}~~"

        + chr(145) /* del */
        + chr(146) /* fnc3 */
        + chr(147) /* fnc2 */
        + chr(148) /* shift */
        + chr(149) /* code-c */
        + chr(150) /* fnc4 */
        + chr(151) /* code-a */
        + chr(152) /* fnc-1 */
        + chr(153) /* start-a */
        + chr(154) /* start-b */
        + chr(155) /* start-c */
        + chr(156) /* stop + 11 */

    cZeroChar   = chr(128).



/* convert to code128b codepage */

str = "".

do i = 1 to length( pcValue ):

    j = index( cCharset, substr( pcValue, i, 1 ) ).

    if j = 0 then
       j = index( cCharset, "?" ).

    str = str + chr(j).

end. /* 1 to length */

pcValue = str.



/* calc checksum */

iCheckSum = 104. /* start-b */

do i = 1 to length( str ):

    iCheckSum = iCheckSum + i * asc( substr( str, i, 1 ) ).

end. /* 1 to length */

iCheckSum = iCheckSum mod 103.

if iCheckSum = 0 then
   iCheckSum = 255. /* chr(0) is used for string null terminations and cannot be used directly */



pcBarcode = chr(104) /* start-b */ + pcValue + chr( iCheckSum ) + chr(106) /* stop + 11 */.



/* convert to the font codepage */

str = "".

do i = 1 to length( pcBarcode ):

    ch = substr( pcBarcode, i, 1 ).

    case ch:

        when chr(255) then /* actually zero */
        str = str + cZeroChar.

        otherwise
        str = str + substr( cCharset, asc( ch ), 1 ).

    end case.

end. /* 1 to length */

pcBarcode = str.
