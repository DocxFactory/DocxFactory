    
/**
 * slibdatefrwd.i -
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

{slib/slibpro.i}



function date_Date2Str              returns char        ( ptDate as date, piMTime as int, piTimeZone as int, pcFormat as char ) {1}.

&if {&pro_xProversion} >= "10" &then

function date_Datetime2Str          returns char        ( ptDatetime   as datetime,     pcFormat as char ) {1}.
function date_DatetimeTz2Str        returns char        ( ptDatetimeTz as datetime-tz,  pcFormat as char ) {1}.
function date_Str2Datetime          returns datetime    ( pcStr as char, pcFormat as char ) {1}.
function date_Str2DatetimeTz        returns datetime-tz ( pcStr as char, pcFormat as char ) {1}.

&endif



function date_isDate                returns log         ( pcStr as char ) {1}.

&if {&pro_xProversion} >= "10" &then

function date_isDateTime            returns log         ( pcStr as char ) {1}.
function date_isDateTimeTz          returns log         ( pcStr as char ) {1}.

&endif

function date_isDateTimeByFormat    returns log         ( pcStr as char, pcFormat as char ) {1}.
function date_isTime                returns log         ( pcStr as char ) {1}.



function date_getTimeInterval       returns int         ( ptHiDate as date, piHiTime as int, ptLoDate as date, piLoTime as int ) {1}.
function date_getMTimeInterval      returns int         ( ptHiDate as date, piHiMTime as int, ptLoDate as date, piLoMTime as int ) {1}.
function date_getMonthEnd           returns date        ( piYear as int, piMonth as int ) {1}.
function date_getMonthStart         returns date        ( piYear as int, piMonth as int ) {1}.
function date_getQuarter            returns int         ( ptDate as date ) {1}.
function date_getWeekNum            returns int         ( ptDate as date, piPerWeekDay as int ) {1}.
function date_getDateByWeek         returns date        ( piYear as int, piWeekNum as int, piWeekDay as int, piPerWeekDay as int ) {1}.
function date_getAge                returns dec         ( ptBirthday as date, ptDate as date ) {1}.



function date_LoDate                returns date        ( ptDate as date ) {1}.
function date_HiDate                returns date        ( ptDate as date ) {1}.

&if {&pro_xProversion} >= "10" &then

function date_LoDatetime            returns datetime    ( ptDatetime    as datetime ) {1}.
function date_HiDatetime            returns datetime    ( ptDatetime    as datetime ) {1}.
function date_LoDatetimeTz          returns datetime-tz ( ptDatetimeTz  as datetime-tz ) {1}.
function date_HiDatetimeTz          returns datetime-tz ( ptDatetimeTz  as datetime-tz ) {1}.

&endif



/* deprecated. replaced by date_getMonthEnd */

function date_getEndOfMonth         returns date        ( piMonth as int, piYea~r as int ) {1}.
