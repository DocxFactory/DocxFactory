
/**
 * slibmathfrwd.i -
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



function math_getShortNum   returns char            ( pdNum as dec, pcFormat as char ) {1}.

function math_calc          returns dec             ( pcExpression as char ) {1}.

function math_tan           returns dec             ( dRadian as dec ) {1}.
function math_atan          returns dec             ( dRadian as dec ) {1}.
function math_sin           returns dec             ( dRadian as dec ) {1}.
function math_sinh          returns dec             ( dRadian as dec ) {1}.
function math_asin          returns dec             ( dRadian as dec ) {1}.
function math_cos           returns dec             ( dRadian as dec ) {1}.
function math_cosh          returns dec             ( dRadian as dec ) {1}.
function math_acos          returns dec             ( dRadian as dec ) {1}.


function math_tan           returns dec             ( pdRadian as dec ) {1}.
function math_sin           returns dec             ( pdRadian as dec ) {1}.
function math_cos           returns dec             ( pdRadian as dec ) {1}.
function math_deg2rad       returns dec             ( pdDegree as dec ) {1}.

/***
function math_tanByDeg      returns dec             ( dDegree as dec ) {1}.
function math_atanByDeg     returns dec             ( dDegree as dec ) {1}.
function math_sinByDeg      returns dec             ( dDegree as dec ) {1}.
function math_sinhByDeg     returns dec             ( dDegree as dec ) {1}.
function math_asinByDeg     returns dec             ( dDegree as dec ) {1}.
function math_cosByDeg      returns dec             ( dDegree as dec ) {1}.
function math_coshByDeg     returns dec             ( dDegree as dec ) {1}.
function math_acosByDeg     returns dec             ( dDegree as dec ) {1}.
***/

function math_ceil          returns int             ( pdValue as dec ) {1}.
function math_floor         returns int             ( pdValue as dec ) {1}.

function math_and           returns {&pro_xBigInt}  ( piValue1 as {&pro_xBigInt}, piValue2 as {&pro_xBigInt} ) {1}.
function math_or            returns {&pro_xBigInt}  ( piValue1 as {&pro_xBigInt}, piValue2 as {&pro_xBigInt} ) {1}.
function math_xor           returns {&pro_xBigInt}  ( piValue1 as {&pro_xBigInt}, piValue2 as {&pro_xBigInt} ) {1}.
function math_not           returns {&pro_xBigInt}  ( piValue  as {&pro_xBigInt} ) {1}.

function math_Hex2Int       returns {&pro_xBigInt}  ( pcHex as char ) {1}.
function math_Int2Hex       returns char            ( piInt as {&pro_xBigInt} ) {1}.
function math_Int642Dec     returns dec             ( ppInt64 as memptr ) {1}.

function math_isNaN         returns log             ( pcValue as char ) {1}.
function math_isNum         returns log             ( pcValue as char ) {1}.
function math_isDec         returns log             ( pcValue as char ) {1}.
function math_isInt         returns log             ( pcValue as char ) {1}.
function math_isAllNumChars returns log             ( pcValue as char ) {1}.
function math_isLog         returns log             ( pcValue as char ) {1}.

function math_HiInt         returns int             ( piValue as int ) {1}.
function math_LoInt         returns int             ( piValue as int ) {1}.
function math_HiDec         returns dec             ( pdValue as dec ) {1}.
function math_LoDec         returns dec             ( pdValue as dec ) {1}.

&if {&pro_xProversion} >= "10.1b" &then

function math_HiInt64       returns int64           ( piValue as int64 ) {1}.
function math_LoInt64       returns int64           ( piValue as int64 ) {1}.

&endif
