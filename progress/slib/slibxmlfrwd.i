
/** 
 * slibxmlfrwd.i
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



function xml_normalizeNsPrefix      returns char        ( phRootNode as handle, pcNsUri as char, pcNsPrefixDefault as char ) {1}.
function xml_getElementByAttr       returns log         ( phContainer as handle, phCursorNode as handle, pcTagNsList as char, pcTagList as char, pcAttrNsList as char, pcAttrList as char, pcAttrValueList as char ) {1}.
function xml_drillElementByAttr     returns log         ( phContainer as handle, phCursorNode as handle, pcDrillTagNsList as char, pcDrillTagList as char, pcTagNsList as char, pcTagList as char, pcAttrNsList as char, pcAttrList as char, pcAttrValueList as char ) {1}.
function xml_getChildByAttr         returns log         ( phParent as handle, phCursorNode as handle, pcTagNsList as char, pcTagList as char, pcAttrNsList as char, pcAttrList as char, pcAttrValueList as char ) {1}.
function xml_getAttrByNamespace     returns char        ( phNode as handle, pcAttrNs as char, pcAttr as char ) {1}.
function xml_getAttrNodeByNamespace returns log         ( phNode as handle, pcAttrNs as char, pcAttr as char, phAttr as handle ) {1}.

function xml_getNextNode            returns log         ( phContainer as handle, phCursorNode as handle ) {1}.
function xml_getPrevNode            returns log         ( phContainer as handle, phCursorNode as handle ) {1}.
function xml_drillNode              returns log         ( phContainer as handle, phCursorNode as handle, pcTagNsList as char, pcTagList as char ) {1}.

function xml_getXmlDeclaration      returns char        ( phDoc as handle ) {1}.

function xml_getNextTag             returns log         ( phContainer as handle, phCursorNode as handle, input-output pcCursorPos as char, output pcStringValue as char ) {1}.
function xml_getNextTagSkipBlanks   returns log         ( phContainer as handle, phCursorNode as handle, input-output pcCursorPos as char, output pcStringValue as char ) {1}.

function xml_compareNode            returns log         ( phSourceNode as handle, phTargetNode as handle ) {1}.
function xml_findChild              returns int         ( phSourceParent as handle, phTargetNode as handle ) {1}.

function xml_encodeXml              returns char        ( pcStr as char ) {1}.
function xml_decodeXml              returns char        ( pcStr as char ) {1}.
function xml_encodeHtml             returns char        ( pcStr as char ) {1}.
function xml_decodeHtml             returns char        ( pcStr as char ) {1}.



function xml_Xml2Date               returns date        ( pcXDate       as char ) {1}.

&if {&pro_xProversion} >= "10" &then

function xml_Xml2Datetime           returns datetime    ( pcXDatetime   as char ) {1}.
function xml_Xml2DatetimeTz         returns datetime-tz ( pcXDatetimeTz as char ) {1}.

&endif

function xml_Date2Xml               returns char        ( ptDate        as date ) {1}.

&if {&pro_xProversion} >= "10" &then

function xml_Datetime2Xml           returns char        ( ptDatetime    as datetime ) {1}.
function xml_DatetimeTz2Xml         returns char        ( ptDatetimeTz  as datetime-tz ) {1}.

&endif

function xml_Xml2Dec                returns dec         ( pcXDec    as char ) {1}.
function xml_Dec2Xml                returns char        ( pdDec     as dec ) {1}.

function xml_Xml2Log                returns log         ( pcXLog    as char ) {1}.
function xml_Log2Xml                returns char        ( plLog     as log ) {1}.
