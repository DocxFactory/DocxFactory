
/**
 * sliberrfrwd.i -
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



function err_isProcUsesSlibErr  returns log     ( input phProc as handle ) {1}.

function err_catchQuit          returns log {1}.
function err_catchStop          returns log {1}.
function err_quit               returns log {1}.
function err_stop               returns log {1}.

function err_catch              returns log     ( output pcError as char, output pcErrorMsg as char, output pcStackTrace as char ) {1}.
function err_throw              returns log     ( input pcError as char, input pcParam as char, input pcFileName as char ) {1}.
function err_throwExact         returns log     ( input pcError as char, input pcErrorMsg as char, pcStackTrace as char ) {1}.

function err_catchReturn        returns log     ( output pcReturn as char ) {1}.
function err_catchReturnError   returns log     ( output pcReturn as char ) {1}.
function err_catchReturnNoApply returns log     ( ) {1}.

function err_return             returns log     ( input pcReturn as char ) {1}.
function err_returnDyn          returns char    ( input pcReturn as char ) {1}.
function err_returnError        returns log     ( input pcReturn as char ) {1}.
function err_returnNoApply      returns log     ( ) {1}.
