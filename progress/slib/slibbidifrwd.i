
/**
 * slibbidifrwd.i -
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



function bidi_Log2VisLtr    returns char ( pcStr as char ) {1}.
function bidi_Log2VisRtl    returns char ( pcStr as char ) {1}.
function bidi_VisLtr2Log    returns char ( pcStr as char ) {1}.
function bidi_VisRtl2Log    returns char ( pcStr as char ) {1}.

function bidi_Log2VisLtrCur returns char ( pcStr as char, piCur as int, output piRetCur as int ) {1}.
function bidi_Log2VisRtlCur returns char ( pcStr as char, piCur as int, output piRetCur as int ) {1}.
function bidi_VisLtr2LogCur returns char ( pcStr as char, piCur as int, output piRetCur as int ) {1}.
function bidi_VisRtl2LogCur returns char ( pcStr as char, piCur as int, output piRetCur as int ) {1}.
