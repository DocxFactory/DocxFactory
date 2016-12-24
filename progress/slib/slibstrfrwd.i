    
/**
 * slibstrfrwd.i -
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


function str_trimMultipleSpace  returns char ( pcStr as char ) {1}.

function str_alignLeft          returns char ( pcStr as char, piWidth as int, pcCont as char ) {1}.
function str_alignRight         returns char ( pcStr as char, piWidth as int, pcCont as char ) {1}.
function str_alignCenter        returns char ( pcStr as char, piWidth as int, pcCont as char ) {1}.

function str_padLeft            returns char ( pcStr as char, pcCh as char, piWidth as int ) {1}.
function str_padRight           returns char ( pcStr as char, pcCh as char, piWidth as int ) {1}.
function str_padCenter          returns char ( pcStr as char, pcCh as char, piWidth as int ) {1}.

function str_concat             returns char ( pcStr as char, pcEntry as char, pcDelim as char, plAddAlways as log ) {1}.
function str_concatL            returns char ( pcStr as char, pcEntry as char, pcDelim as char, plAddAlways as log ) {1}.

function str_getEntries         returns char ( pcEntryList as char, piEntryFrom as int, piEntryTo as int, pcDelim as char ) {1}.
function str_sortEntries        returns char ( pcEntryList as char, plDescend as log, pcDelim as char ) {1}.

function str_soundex            returns char ( pcStr as char ) {1}.

function str_HiChar             returns char ( pcStr as char ) {1}.
function str_LoChar             returns char ( pcStr as char ) {1}.
function str_HiCharSensitive    returns char ( pcStr as char ) {1}.
function str_LoCharSensitive    returns char ( pcStr as char ) {1}.

