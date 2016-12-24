
/**
 * slibunixfrwd.i -
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



function unix_getHostName       returns char {1}.
function unix_getUserName       returns char {1}.

function unix_getSubPath        returns char ( pcPath as char, pcFrom as char, pcTo as char ) {1}.
function unix_getRelativePath   returns char ( pcSourceDir as char, pcPath as char ) {1}.
function unix_isRelativePath    returns log  ( pcPath as char ) {1}.
function unix_normalizePath     returns char ( pcPath as char ) {1}.

function unix_getTempFile       returns char ( pcUserChars as char, pcExt as char ) {1}.
function unix_getTempFileByDir  returns char ( pcDir as char, pcUserChars as char, pcExt as char ) {1}.

function unix_getBigFileSize    returns dec ( pcFileName as char ) {1}.
