
/**
 * slibwinfrwd.i -
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



function win_getHostName            returns char {1}. 
function win_getUserName            returns char {1}.

function win_isFileLocked           returns log  ( pcFileName as char ) {1}.
function win_isPathOnLocalHardDisk  returns log  ( pcPath as char ) {1}.
function win_getDriveType           returns char ( pcDrive as char ) {1}.
function win_getSpecialFolder       returns char ( piCSIDL as int ) {1}.
function win_getBigFileSize         returns dec  ( pcFileName as char ) {1}.
function win_getShortPathName       returns char ( pcLongPathName as char ) {1}. 
function win_getSubPath             returns char ( pcPath as char, pcFrom as char, pcTo as char ) {1}.
function win_getRelativePath        returns char ( pcSourceDir as char, pcPath as char ) {1}.
function win_isRelativePath         returns log  ( pcPath as char ) {1}.
function win_isUncPath              returns log  ( pcPath as char ) {1}.
function win_normalizePath          returns char ( pcPath as char ) {1}.

function win_getTempFile            returns char ( pcUserChars as char, pcExt as char ) {1}.
function win_getTempFileByDir       returns char ( pcDir as char, pcUserChars as char, pcExt as char ) {1}.

function win_getProcessName         returns char ( piPid as int ) {1}.
function win_isProcessRunning       returns log  ( phProcess as int ) {1}.

function win_createProcess          returns int  ( pcCommandLine as char, pcWorkDir as char, piShowWindow as int ) {1}.

function win_isMutexExists          returns log  ( plGlobal as log, pcMutex as char ) {1}.

function win_isKeyToggled           returns log  ( piVirtKey as int ) {1}.
function win_isKeyPressed           returns log  ( piVirtKey as int ) {1}.

function win_getSystemMetrics       returns int  ( nIndex as int ) {1}.

function win_queryPerfCounter       returns dec {1}.
function win_queryPerfFrequency     returns dec {1}.

function win_getLangId              returns char {1}.
