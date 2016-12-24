
/**
 * slibbidi.i -
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



/* &arguments -

    {1} - bidi specification.

        ansi - default

        win9x - the old windows algorithm differs slighly from the one used in windows 2k and above.

        unicode - not yet supported. in the unicode spec bi directional languages are nested. in the ansi spec languages alternate.*/



&if defined( xSLibBidi ) = 0 &then

    &if "{1}" = "" &then
          &scoped xSpec ansi
    &else &scoped xSpec {1}
    &endif

    {slib/start-slib.i "'slib/slibbidi_{&xSpec}.p'"}

    {slib/slibbidifrwd.i "in super"}

    &global xSLibBidi defined

&endif /* defined = 0 */
