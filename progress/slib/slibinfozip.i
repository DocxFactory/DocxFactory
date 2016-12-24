
/**
 * slibinfozip.i
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



&if defined( xSLibInfoZip ) = 0 &then

    define var infozip_lRunning as log no-undo.

    &if "{1}" = "no-error" &then
          {slib/start-slib.i "'slib/slibinfozip.p'" "( 'no-error', output infozip_lRunning )"}
    &else {slib/start-slib.i "'slib/slibinfozip.p'" "( ?,          output infozip_lRunning )"}
    &endif

    {slib/slibinfozipfrwd.i "in super"}

    {slib/slibinfozipprop.i}

    &glob xSLibInfoZip defined

&endif /* defined */
