    
/**
 * slibprofrwd.i -
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



function pro_getStackTrace          returns char    ( piStartLevel as int, plHideDlcProc as log ) {1}.

function pro_getRunFile             returns char    ( pcFileName as char ) {1}.

function pro_getBufferFieldValue    returns char    ( pcBufferFieldHandle as char ) {1}.

function pro_compareTable           returns log     ( phSourceBuffer as handle, phTargetBuffer as handle ) {1}.

function pro_parseKeywordPhraseList returns char    ( pcStatement as char, pcKeywordList as char ) {1}.
function pro_parseWordList          returns char    ( pcStatement as char, plRemoveSpaces as log, plFlat as log ) {1}.
function pro_replaceNnn             returns char    ( pcStr as char ) {1}.
function pro_unquote                returns char    ( pcStr as char ) {1}.
function pro_unparenthesis          returns char    ( pcStr as char ) {1}.
