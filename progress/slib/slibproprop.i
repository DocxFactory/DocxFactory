
/**
 * slibproprop.i -
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



/* defines a "99.9x" formatted proversion preprocessor so "09.1d" can be compared with "10.0b" */

&if index( proversion, "." ) = 2 &then

    &global pro_xProversion '0' + proversion

&else
    &global pro_xProversion proversion

&endif



&if {&pro_xProversion} >= "10.1b" &then

    &global pro_xBigInt int64

&else

    &global pro_xBigInt int

&endif



define new global shared var pro_cDlc               as char no-undo.
define new global shared var pro_cProCfg            as char no-undo.
define new global shared var pro_cWorkDir           as char no-undo.
define new global shared var pro_cTempFullDir       as char no-undo.
define new global shared var pro_cFullPropath       as char no-undo.
define new global shared var pro_cFullProversion    as char no-undo.



define temp-table pro_ttLine no-undo

    field iLineNum  as int
    field cLine     as char
    
    index iLineNum is primary unique
          iLineNum.

