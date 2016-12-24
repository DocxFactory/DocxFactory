
/**
 * slibinfozipprop.i -
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



&if "{&opsys}" begins "win" &then

    &global infozip_xUtilZip    'slib~\bin~\zip.exe'
    &global infozip_xUtilUnZip  'slib~\bin~\unzip.exe'

    &global infozip_xCmdAdd     '"%zip%" -S -r -D %archive% %files%'
    &global infozip_xCmdDel     '"%zip%" -d %archive% %files%'
    &global infozip_xCmdList    '"%unzip%" -Z -T %archive% %files%'
    &global infozip_xCmdExtract '"%unzip%" -o %archive% %files% -d %outdir%'

&else

    &global infozip_xUtilZip    '/usr/bin/zip'
    &global infozip_xUtilUnZip  '/usr/bin/unzip'

    &global infozip_xCmdAdd     '"%zip%" -r -D %archive% %files%'
    &global infozip_xCmdDel     '"%zip%" -d %archive% %files%'
    &global infozip_xCmdList    '"%unzip%" -Z -T %archive% %files%'
    &global infozip_xCmdExtract '"%unzip%" -o %archive% %files% -d %outdir%'

&endif



define temp-table infozip_ttFile no-undo

    field cPath     as char
    field tDate     as date
    field iTime     as int
    field dLength   as dec

    index cPath is primary unique
          cPath.
