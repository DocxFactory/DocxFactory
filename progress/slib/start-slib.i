
/**
 * start-slib.i -
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



&if defined( xStartSLib ) = 0 &then

    define var slib_cStartSLibFile  as char no-undo.
    define var slib_hStartSLibHndl  as handle no-undo.

    define var slib_hProcHndl       as handle no-undo.
    define var slib_lInitializeProc as log no-undo.



    if      search( "slib/start-slib.r" ) <> ?  then slib_cStartSLibFile = "slib/start-slib.r".
    else if search( "slib/start-slib.p" ) <> ?  then slib_cStartSLibFile = "slib/start-slib.p".

    /***
    else if search( "start-slib.r" ) <> ?       then slib_cStartSLibFile = "start-slib.r".
    else if search( "start-slib.p" ) <> ?       then slib_cStartSLibFile = "start-slib.p".

    else if search( "xx/start-slib.r" ) <> ?    then slib_cStartSLibFile = "xx/start-slib.r".
    else if search( "xx/start-slib.p" ) <> ?    then slib_cStartSLibFile = "xx/start-slib.p".

    else if search( "us/xx/start-slib.r" ) <> ? then slib_cStartSLibFile = "us/xx/start-slib.r".
    else if search( "us/xx/start-slib.p" ) <> ? then slib_cStartSLibFile = "us/xx/start-slib.p".
    ***/



    slib_hStartSLibHndl = session:first-procedure.

    do while valid-handle( slib_hStartSLibHndl ) and slib_hStartSLibHndl:file-name <> slib_cStartSLibFile:
        slib_hStartSLibHndl = slib_hStartSLibHndl:next-sibling.
    end.

    if not valid-handle( slib_hStartSLibHndl ) then
        run value( slib_cStartSLibFile ) persistent set slib_hStartSLibHndl.

    &global xStartSLib defined

&endif /* defined( ) = 0 */



run startSLib in slib_hStartSLibHndl (

    input   {1},
    input   &if "{3}" <> "" &then {3} &else ? &endif,
    output  slib_hProcHndl,
    output  slib_lInitializeProc ).

if slib_lInitializeProc then do:

    &if "{2}" <> "" &then
        run initializeProc in slib_hProcHndl {2}.
    &else
        run initializeProc in slib_hProcHndl.
    &endif

end. /* lInitializeProc */
