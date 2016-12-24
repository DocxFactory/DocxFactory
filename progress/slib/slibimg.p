
/**
 * slibimg.p -
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
 *  along with this program. If not, see <http:/www.gnu.org/licenses/>.
 *
 *  Contact information
 *  Email: alonblich@gmail.com
 *  Phone: +972-54-218-8086
 */

{slib/slibos.i}

{slib/sliberr.i}



on "close" of this-procedure do:

    delete procedure this-procedure.

end. /* close */

procedure initializeProc:

end procedure. /* initializeProc */



procedure img_getImageSize:

    define input    param pcFileName    as char no-undo.
    define output   param piWidth       as int no-undo.
    define output   param piHeight      as int no-undo.
    define output   param pdHorizDpi    as dec no-undo.
    define output   param pdVertDpi     as dec no-undo.

    define var cFileExt as char no-undo.

    cFileExt = os_getSubPath( pcFileName, "ext", ? ).

    case cFileExt:

        when ".gif" then do:

            run img_getGifSize(
                input   pcFileName,
                output  piWidth,
                output  piHeight ).

            assign
                pdHorizDpi  = ?
                pdVertDpi   = ?.

        end. /* when ".gif" */

        when ".jpg" or when ".jpeg" then

            run img_getJpgSize(
                input   pcFileName,
                output  piWidth,
                output  piHeight,
                output  pdHorizDpi,
                output  pdVertDpi ).

        when ".png" then

            run img_getPngSize(
                input   pcFileName,
                output  piWidth,
                output  piHeight,
                output  pdHorizDpi,
                output  pdVertDpi ).

        otherwise

            {slib/err_throw "'img_unsupported_image_type'"}.

    end case. /* cFileExt */

end procedure. /* img_getImageSize */

procedure img_getGifSize:

    define input    param pcFileName    as char no-undo.
    define output   param piWidth       as int no-undo.
    define output   param piHeight      as int no-undo.

    define var iFileSize    as int no-undo.
    define var ptr          as memptr no-undo.

    file-info:file-name = pcFileName.
    if file-info:full-pathname = ? then

        {slib/err_throw "'file_not_found'" pcFileName}.

    assign
        pcFileName  = file-info:full-pathname
         iFileSize  = file-info:file-size.



    {slib/err_try}:

        if iFileSize < 10 then
            {slib/err_throw "'img_invalid_gif_file'" pcFileName}.

        set-size( ptr ) = 10.

        input from value( pcFileName ) binary no-convert.
        import ptr.
        input close.

        if get-byte( ptr, 1 ) <> 71
        or get-byte( ptr, 2 ) <> 73
        or get-byte( ptr, 3 ) <> 70 then
            {slib/err_throw "'img_invalid_gif_file'" pcFileName}.

        assign
            piWidth     = get-byte( ptr,  8 ) * 256 + get-byte( ptr, 7 )
            piHeight    = get-byte( ptr, 10 ) * 256 + get-byte( ptr, 9 ).

    {slib/err_catch}:

        {slib/err_throw last}.

    {slib/err_finally}:

        set-size( ptr ) = 0.

    {slib/err_end}.

end procedure. /* img_getGifSize */

procedure img_getJpgSize:

    define input    param pcFileName    as char no-undo.
    define output   param piWidth       as int no-undo.
    define output   param piHeight      as int no-undo.
    define output   param pdHorizDpi    as dec no-undo.
    define output   param pdVertDpi     as dec no-undo.

    define var iFileSize    as int no-undo.
    define var iBlockLength as int no-undo.

    define var iOffset      as int no-undo.
    define var ptr          as memptr no-undo.

    assign
        piWidth     = ?
        piHeight    = ?
        pdHorizDpi  = ?
        pdVertDpi   = ?.

    file-info:file-name = pcFileName.
    if file-info:full-pathname = ? then

        {slib/err_throw "'file_not_found'" pcFileName}.

    assign
        pcFileName  = file-info:full-pathname
         iFileSize  = file-info:file-size.



    {slib/err_try}:

        if iFileSize < 4 then
            {slib/err_throw "'img_invalid_jpg_file'" pcFileName}.

        set-size( ptr ) = iFileSize.

        input from value( pcFileName ) binary no-convert.
        import ptr.
        input close.

        if get-byte( ptr,  1 ) <> 255
        or get-byte( ptr,  2 ) <> 216
        or get-byte( ptr,  3 ) <> 255
        or get-byte( ptr,  4 ) <> 224 
        
        or get-byte( ptr,  7 ) <> 74
        or get-byte( ptr,  8 ) <> 70
        or get-byte( ptr,  9 ) <> 73
        or get-byte( ptr, 10 ) <> 70
        or get-byte( ptr, 11 ) <> 0 then
            {slib/err_throw "'img_invalid_jpg_file'" pcFileName}.

        assign
            pdHorizDpi  = get-byte( ptr, 15 ) * 256 + get-byte( ptr, 16 )
            pdVertDpi   = get-byte( ptr, 17 ) * 256 + get-byte( ptr, 18 ).

        if get-byte( ptr, 14 ) = 2 then

        assign
            pdHorizDpi  = 2.54 * pdHorizDpi
            pdVertDpi   = 2.54 * pdVertDpi.



        assign
            iOffset = 5
            iOffset = iOffset + get-byte( ptr, iOffset ) * 256 + get-byte( ptr, iOffset + 1 ).

        _block:

        repeat:

            if iOffset + 1 > iFileSize then
                {slib/err_throw "'img_invalid_jpg_file'" pcFileName}.

            if get-byte( ptr, iOffset ) <> 255 then
                {slib/err_throw "'img_invalid_jpg_file'" pcFileName}.

            case get-byte( ptr, iOffset + 1 ):
            
                when 192 or
                when 194 then do:

                    if iOffset + 8 > iFileSize then
                        {slib/err_throw "'img_invalid_jpg_file'" pcFileName}.

                    assign
                        piWidth     = get-byte( ptr, iOffset + 7 ) * 256 + get-byte( ptr, iOffset + 8 )
                        piHeight    = get-byte( ptr, iOffset + 5 ) * 256 + get-byte( ptr, iOffset + 6 ).

                    leave _block.

                end. /* when 192 */

                when 218 then do:

                    iOffset = iOffset + 2.

                    repeat:
                    
                        if iOffset + 1 > iFileSize then
                            {slib/err_throw "'img_invalid_jpg_file'" pcFileName}.

                        if  get-byte( ptr, iOffset )      = 255
                        and get-byte( ptr, iOffset + 1 ) <> 0 then

                            leave.

                        iOffset = iOffset + 1.

                    end. /* repeat */

                end. /* when 218 */

                when 196 or
                when 219 or
                when 224 or when 225 or when 226 or when 227 or when 228 or when 229 or when 230 or when 231 or when 232 or when 233 or when 234 or when 235 or when 236 or when 237 or when 238 or when 239 or when 254 then do:

                    if iOffset + 3 > iFileSize then
                        {slib/err_throw "'img_invalid_jpg_file'" pcFileName}.

                    assign
                        iOffset = iOffset + 2
                        iOffset = iOffset + get-byte( ptr, iOffset ) * 256 + get-byte( ptr, iOffset + 1 ).

                end. /* when 194 */

                when 222 then

                    iOffset = iOffset + 2 + 2.

                when 208 or
                when 209 or when 210 or when 211 or when 212 or when 213 or when 214 or when 215 or when 216 then

                    iOffset = iOffset + 2.

                when 217 then

                    leave _block.

                otherwise

                    {slib/err_throw "'img_invalid_jpg_file'" pcFileName}.

            end case. /* get-byte( iOffset + 1 ) */

        end. /* repeat */

        if piWidth  = ?
        or piHeight = ? then

            {slib/err_throw "'img_invalid_jpg_file'" pcFileName}.

    {slib/err_catch}:

        {slib/err_throw last}.

    {slib/err_finally}:

        set-size( ptr ) = 0.

    {slib/err_end}.

end procedure. /* img_getJpgSize */

procedure img_getPngSize:

    define input    param pcFileName    as char no-undo.
    define output   param piWidth       as int no-undo.
    define output   param piHeight      as int no-undo.
    define output   param pdHorizDpi    as dec no-undo.
    define output   param pdVertDpi     as dec no-undo.

    define var iFileSize    as int no-undo.
    define var ptr          as memptr no-undo.

    define var cChunkType   as char no-undo.
    define var iChunkLength as int no-undo.
    define var iOffset      as int no-undo.

    assign
        piWidth     = ?
        piHeight    = ?
        pdHorizDpi  = ?
        pdVertDpi   = ?.

    file-info:file-name = pcFileName.
    if file-info:full-pathname = ? then

        {slib/err_throw "'file_not_found'" pcFileName}.

    assign
        pcFileName  = file-info:full-pathname
         iFileSize  = file-info:file-size.



    {slib/err_try}:

        if iFileSize < 9 then
            {slib/err_throw "'img_invalid_png_file'" pcFileName}.

        set-size      ( ptr ) = iFileSize.
        set-byte-order( ptr ) = big-endian.

        input from value( pcFileName ) binary no-convert.
        import ptr.
        input close.

        if get-byte( ptr, 1 ) <> 137
        or get-byte( ptr, 2 ) <> 80
        or get-byte( ptr, 3 ) <> 78
        or get-byte( ptr, 4 ) <> 71
        or get-byte( ptr, 5 ) <> 13
        or get-byte( ptr, 6 ) <> 10
        or get-byte( ptr, 7 ) <> 26
        or get-byte( ptr, 8 ) <> 10 then

            {slib/err_throw "'img_invalid_png_file'" pcFileName}.



        iOffset = 9.

        assign
           iChunkLength = get-long  ( ptr, iOffset ) + 4
           cChunkType   = get-string( ptr, iOffset + 4, 4 ).

        if cChunkType <> "IHDR" then
            {slib/err_throw "'img_invalid_png_file'" pcFileName}.

        if iOffset + 8 + iChunkLength - 1 > iFileSize then
            {slib/err_throw "'img_invalid_png_file'" pcFileName}.

        iOffset = iOffset + 8.

        assign
            piWidth     = get-long( ptr, iOffset )
            piHeight    = get-long( ptr, iOffset + 4 ).

        iOffset = iOffset + iChunkLength.



        _block:

        repeat while iOffset + 8 - 1 <= iFileSize:

            assign
               iChunkLength = get-long  ( ptr, iOffset ) + 4
               cChunkType   = get-string( ptr, iOffset + 4, 4 ).

            if cChunkType = "IEND" then
                leave _block.

            if iOffset + 8 + iChunkLength - 1 > iFileSize then
                leave _block.

            iOffset = iOffset + 8.

            case cChunkType:

                when "pHYs" then do:

                    assign
                        pdHorizDpi  = get-long( ptr, iOffset )
                        pdVertDpi   = get-long( ptr, iOffset + 4 ).

                    if get-byte( ptr, iOffset + 8 ) = 1 then

                    assign
                        pdHorizDpi  = 0.0254 * pdHorizDpi
                        pdVertDpi   = 0.0254 * pdVertDpi.

                    leave _block.

                end. /* when "pHYs" */

            end case. /* cChunkType */

            iOffset = iOffset + iChunkLength.

        end. /* repeat */

    {slib/err_catch}:

        {slib/err_throw last}.

    {slib/err_finally}:

        set-size( ptr ) = 0.

    {slib/err_end}.

end procedure. /* img_getPngSize */
