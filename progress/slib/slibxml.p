
/**
 * slibxml.p -
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
 *  Contact Information
 *  Email: alonblich@gmail.com
 *  Phone: +972-54-218-8086
 */

{slib/slibxmlfrwd.i "forward"}

{slib/slibos.i}

{slib/slibdate.i}

{slib/slibstr.i}

{slib/slibmath.i}

{slib/slibpro.i}

{slib/sliberr.i}



/***
define var hRawDoc      as handle no-undo.
define var hRawRoot     as handle no-undo.
define var hRawNode     as handle no-undo.
define var iRawStart    as int no-undo.
***/



function getNextNode    returns log private     ( phContainer as handle, phCursorNode as handle ) forward.
function getPrevNode    returns log private     ( phContainer as handle, phCursorNode as handle ) forward.
function drillNode      returns log private     ( phContainer as handle, phCursorNode as handle, pcTagNsList as char, pcTagList as char ) forward.

function unescapeStr    returns char private    ( pcStr as char ) forward.



on "close" of this-procedure do:

    delete widget-pool "wpSlibXml" no-error.

    delete procedure this-procedure.

end. /* on close */

procedure initializeProc:

    /***
    define var ptr as memptr no-undo.

    delete widget-pool "wpSlibXml" no-error.
    create widget-pool "wpSlibXml" persistent.

    create x-document hRawDoc  in widget-pool "wpSlibXml".
    create x-noderef  hRawRoot in widget-pool "wpSlibXml".
    create x-noderef  hRawNode in widget-pool "wpSlibXml".

    hRawDoc:create-node( hRawRoot, "t", "element" ).
    hRawDoc:append-child( hRawRoot ).

    hRawDoc:create-node( hRawNode, "", "text" ).
    hRawRoot:append-child( hRawNode ).

    hRawDoc:save( "memptr", ptr ).
    iRawStart = r-index( get-string( ptr, 1 ), "?>" ) + 2.

    set-size( ptr ) = 0.
    ***/

end procedure. /* initializeProc */



define work-table wtEscape no-undo

    field iPos      as int
    field iBytes    as int
    field cEscape   as char
    field iLen      as int.

procedure xml_loadXml:

    define input param phDoc        as handle no-undo.
    define input param pcFileName   as char no-undo.
    define input param plEscape     as log no-undo.

    define var cFileName    as char no-undo.
    define var iFileSize    as int no-undo.

    define var pSource      as memptr no-undo.
    define var pTarget      as memptr no-undo.

    define var iSize        as int no-undo.
    define var iPos         as int no-undo.
    define var iByte        as int no-undo.
    define var iBytes       as int no-undo.
    define var cEscape      as char no-undo.
    define var iValue       as int no-undo.

    define var ptr          as memptr no-undo.
    define var str          as char no-undo.
    define var i            as int no-undo.
    define var j            as int no-undo.

    file-info:file-name = pcFileName.
    if file-info:full-pathname = ? then

        {slib/err_throw "'file_not_found'" pcFileName}.

    assign
        cFileName = file-info:full-pathname
        iFileSize = file-info:file-size.

    if not valid-handle( phDoc )
    or phDoc:type <> "x-document" then

        {slib/err_throw "'xml_invalid_xdoc'"}.



    if plEscape then do:

        {slib/err_try}:

            for each wtEscape:
                delete wtEscape.
            end.

            set-size( pSource ) = iFileSize.

            input from value( cFileName ) binary no-convert.
            import pSource.
            input close.



            iPos = 1.

            do while iPos <= iFileSize:

                iByte = get-byte( pSource, iPos ).

                if iByte < 192 then iPos = iPos + 1.

                else do:

                         if iByte < 224 then iBytes = 2.
                    else if iByte < 240 then iBytes = 3.
                    else if iByte < 248 then iBytes = 4.
                    else if iByte < 252 then iBytes = 5.
                    else                     iBytes = 6.

                    if codepage-convert( get-string( pSource, iPos, iBytes ), session:cpinternal, "utf-8" ) = "?" then do:

                        iValue = 0.

                        j = 1.
                        do i = iBytes to 2 by -1:

                            put-bits( iValue, j, 6 ) = get-bits( get-byte( pSource, iPos + i - 1 ), 1, 6 ).
                            j = j + 6.

                        end. /* iBytes to 2 */

                        put-bits( iValue, j, 7 - iBytes ) = get-bits( get-byte( pSource, iPos ), 1, 7 - iBytes ).

                        assign
                            cEscape = math_Int2Hex( iValue )
                            cEscape = fill( "0", 4 - length( cEscape ) ) + cEscape
                            cEscape = "__utf8x" + cEscape + "__".

                        create wtEscape.
                        assign
                            wtEscape.iPos       = iPos
                            wtEscape.iBytes     = iBytes
                            wtEscape.cEscape    = cEscape
                            wtEscape.iLen       = length( cEscape ).

                    end. /* codepage-convert = ? */

                    iPos = iPos + iBytes.

                end. /* else */

            end. /* while iPos <= iFileSize */



            if can-find( first wtEscape ) then do:

                {slib/err_try}:

                    iSize = iFileSize.

                    for each wtEscape:

                        iSize = iSize - wtEscape.iBytes + wtEscape.iLen.

                    end. /* each wtEscape */

                    set-size( pTarget ) = iSize.



                    assign
                        iSize   = 0
                        iPos    = 1.

                    for each wtEscape:

                        iBytes = ( wtEscape.iPos - 1 ) - iPos + 1.

                        if iBytes > 0 then do:

                            set-size( ptr ) = iBytes.

                            ptr = get-bytes( pSource, iPos, iBytes ).
                            put-bytes( pTarget, iSize + 1 ) = ptr.

                        end. /* iBytes > 0 */

                        put-string( pTarget, iSize + 1 + iBytes, wtEscape.iLen ) = wtEscape.cEscape.

                        if iBytes > 0 then 

                            set-size( ptr ) = 0.

                        assign
                            iSize   = iSize + iBytes + wtEscape.iLen
                            iPos    = wtEscape.iPos + wtEscape.iBytes.

                    end. /* each wtEscape */

                    iBytes = iFileSize - iPos + 1.

                    if iBytes > 0 then do:

                        set-size( ptr ) = iBytes.

                        ptr = get-bytes( pSource, iPos, iBytes ).
                        put-bytes( pTarget, iSize + 1 ) = ptr.

                        set-size( ptr ) = 0.

                    end. /* iBytes > 0 */

                    phDoc:load( "memptr", pTarget, no ).

                {slib/err_catch}:

                    {slib/err_throw last}.

                {slib/err_finally}:
    
                    set-size( pTarget ) = 0.

                {slib/err_end}.

            end. /* can-find */

            else do:

                phDoc:load( "memptr", pSource, no ).

            end. /* else */

        {slib/err_catch}:

            {slib/err_throw last}.

        {slib/err_finally}:

            set-size( pSource ) = 0.

        {slib/err_end}.

    end. /* plEscape */

    else do:

        phDoc:load( "file", cFileName, no ).

    end. /* else */

    run xml_normalizeXml( phDoc ).

end procedure. /* xml_loadXml */

procedure xml_saveXml:

    define input param phDoc        as handle no-undo.
    define input param pcFileName   as char no-undo.

    define var pIn  as memptr no-undo.
    define var pOut as memptr no-undo.

    define var iLen as int no-undo.
    define var iPos as int no-undo.

    phDoc:save( "memptr", pIn ).

    assign
        iLen    = get-size( pIn )
        iPos    = 1.
    
    if iLen >= 2 and get-string( pIn, 1, 2 ) = "<?" then do:
    
        iPos = 3.
    
        repeat while iPos <= iLen:
        
            if get-string( pIn, iPos, 2 ) = "?>" then do:
            
                iPos = iPos + 2.
                leave.
                
            end. /* get-string() = "?>" */
        
            iPos = iPos + 1.
        
        end. /* repeat */    
    
    end. /* get-string() = "<?" */
    
    pOut = get-bytes( pIn, iPos, iLen - iPos + 1 ).

    output to value( pcFileName ) binary no-convert.
    put unformatted xml_getXmlDeclaration( phDoc ).
    export pOut.
    output close.
    
    set-size( pOut ) = 0.
    set-size( pIn ) = 0.

end procedure. /* xml_saveXml */

/***

procedure xml_saveXml:

    define input param phDoc        as handle no-undo.
    define input param pcFileName   as char no-undo.
    define input param plEscape     as log no-undo.

    define var pSource      as memptr no-undo.
    define var pTarget      as memptr no-undo.

    define var iSize        as int no-undo.
    define var iPos         as int no-undo.
    define var iByte        as int no-undo.
    define var iBytes       as int no-undo.
    define var cEscape      as char no-undo.
    define var iValue       as int no-undo.

    define var ptr          as memptr no-undo.
    define var str          as char no-undo.
    define var i            as int no-undo.
    define var j            as int no-undo.

    if not valid-handle( phDoc )
    or phDoc:type <> "x-document" then

        {slib/err_throw "'xml_invalid_xdoc'"}.



    if plEscape then do:

        {slib/err_try}:

            for each wtEscape:
                delete wtEscape.
            end.

            phDoc:save( "memptr", pSource ).



            iPos = 1.

            do while iPos <= iFileSize:

            end. /* while iPos <= iFileSize */

        {slib/err_catch}:

            {slib/err_throw last}.

        {slib/err_finally}:

            set-size( pSource ) = 0.

        {slib/err_end}.

    end. /* plEscape */

    else do:

        phDoc:save( "file", pcFileName ).

    end. /* else */

end procedure. /* xml_saveXml */

***/

procedure xml_normalizeXml:

    define input param phDoc as handle no-undo.

    define var hRoot as handle no-undo.

    create widget-pool.

    create x-noderef hRoot.

    phDoc:get-document-element( hRoot ).

    run normalizeXmlRecurr( hRoot ).

end procedure. /* xml_normalizeXml */

procedure normalizeXmlRecurr private:

    define input param phParentNode as handle no-undo.

    define var hNode    as handle no-undo.
    define var i        as int no-undo.

    create widget-pool.

    create x-noderef hNode.

    do i = phParentNode:num-children to 1 by -1:

        phParentNode:get-child( hNode, i ).

        case hNode:subtype:

            when "element" then do:

                if hNode:num-children > 0 then
                    run normalizeXmlRecurr( hNode ).

            end. /* element */

            when "text" then do:

                if trim( hNode:node-value ) /* also trims new lines */ = "" then do:

                    if phParentNode:num-children <> 1 then
                    hNode:delete-node( ).

                end. /* trim = "" */

                else
                if index( hNode:node-value, chr(26) ) > 0 then
                    hNode:node-value = replace( hNode:node-value, chr(26), "" ).

                else
                if index( hNode:node-value, chr(127) ) > 0 then
                    hNode:node-value = replace( hNode:node-value, chr(127), "" ).

                /***
                if  session:cpinternal <> "utf-8"
                and session:cpinternal <> "undefined" then
                    run normalizeText( hNode ).
                ***/

            end. /* text */

        end case. /* subtype */

    end. /* num-children to 1 */

end procedure. /* normalizeXmlRecurr */

/***
procedure normalizeText private:

    define input param phNode as handle no-undo.

    /***
    define var cOldValue as char no-undo.
    define var cNewValue as char no-undo.

    define var ptr          as memptr no-undo.

    hRawNode:delete-node( ) no-error.

    hRawDoc:import-node( hRawNode, phNode, no ).
    hRawRoot:append-child( hRawNode ).

    hRawDoc:save( "memptr", ptr ).

    assign
       cOldValue = phNode:node-value

       cNewValue = get-string( ptr, iRawStart )
       cNewValue = substr( cNewValue, index( cNewValue, "<t>" ) + 3 )
       cNewValue = substr( cNewValue, 1, r-index( cNewValue, "</t>" ) - 1 )
       cNewValue = xml_decodeXml( cNewValue ).

       phNode:node-value =  cNewValue.
    if phNode:node-value <> cNewValue then
       phNode:node-value =  cOldValue.

    set-size( ptr ) = 0.
    ***/

    /***
    assign
       cOldValue = phNode:node-value
       cNewValue = getNodeValue( phNode )
       cNewValue = xml_decodeXml( cNewValue ).

       phNode:node-value =  cNewValue.
    if phNode:node-value <> cNewValue then
       phNode:node-value =  cOldValue.
    ***/

    if index( phNode:node-value, chr(127) ) > 0 then

        phNode:node-value = xml_decodeXml( replace( phNode:node-value, chr(127), "" ) ).

end procedure. /* normalizeText */
***/



function xml_normalizeNsPrefix returns char ( phRootNode as handle, pcNsUri as char, pcNsPrefixDefault as char ):

    define var cAttrName    as char no-undo.
    define var cAttrValue   as char no-undo.
    define var cAttrPrefix  as char no-undo.

    define var RetVal       as char no-undo.
    define var i            as int no-undo.
    define var j            as int no-undo.

    RetVal = ?.

    j = num-entries( phRootNode:attribute-names ).

    do i = 1 to j:

        assign
           cAttrName    = entry( i, phRootNode:attribute-names )
           cAttrValue   = phRootNode:get-attribute( cAttrName ).

        if cAttrName = "xmlns" then
            cAttrPrefix = "".
        else cAttrPrefix = substr( cAttrName, 7 /* length( "xmlns:" ) + 1 */ ).

        if cAttrValue = pcNsUri then do:

            RetVal = cAttrPrefix.
            leave.

        end. /* cAttrValue = pcNsUri */

    end. /* 1 to j */

    if RetVal = ? then do:

        RetVal = pcNsPrefixDefault.
        i = 0.

        do while lookup( "xmlns:" + RetVal, phRootNode:attribute-names ) <> 0:

            i = i + 1.
            RetVal = pcNsPrefixDefault + string( i ).

        end. /* do while */

        phRootNode:set-attribute( "xmlns:" + RetVal, pcNsUri ).

    end. /* RetVal = ? */

    return RetVal.

end function. /* xml_normalizeNsPrefix */



function xml_getElementByAttr returns log ( 

    input phContainer       as handle, 
    input phCursorNode      as handle,
    input pcTagNsList       as char,
    input pcTagList         as char,
    input pcAttrNsList      as char,
    input pcAttrList        as char,
    input pcAttrValueList   as char ):

    define var hDoc         as handle no-undo.
    define var hAttr        as handle no-undo.
    define var hRootNode    as handle no-undo.
    define var lBefore      as log no-undo.
    define var lFound       as log no-undo.
    define var iLen         as int no-undo.

    define var str          as char no-undo.
    define var lOk          as log no-undo.
    define var i            as int no-undo.

    create widget-pool.

    if not valid-handle( phContainer ) 
    or phContainer:type <> "x-noderef" and phContainer:type <> "x-document" then

        {slib/err_throw "'xml_invalid_xnoderef'"}.

    if phContainer:type = "x-document" then do:

        hDoc = phContainer.

        create x-noderef hRootNode.
        create x-noderef phContainer.

        hDoc:get-document-element( hRootNode ).
        hDoc:get-document-element( phContainer ).

    end. /* type = "x-document" */

    else do:

        hDoc = phContainer:owner-document.

        create x-noderef hRootNode.
        hDoc:get-document-element( hRootNode ).

    end. /* else */

    lOk = phContainer:owner-document <> ? no-error.
    if not lOk then

        {slib/err_throw "'xml_invalid_xnoderef_ref'"}.



    if not valid-handle( phCursorNode )
    or phCursorNode:type <> "x-noderef" then

        {slib/err_throw "'xml_invalid_xnoderef'"}.

    lBefore = no.

    lOk = phCursorNode:owner-document <> ? no-error.
    if not lOk then do:

        if phContainer:unique-id = hRootNode:unique-id then
           hDoc:get-document-element( phCursorNode ).

        else do:

           phContainer:get-parent( phCursorNode ).
           phCursorNode:get-child( phCursorNode, phContainer:child-num ).

        end. /* else */

        lBefore = yes.

    end. /* not lOk */

    lOk = phCursorNode:owner-document <> ? no-error.
    if not lOk then

        {slib/err_throw "'xml_invalid_xnoderef_ref'"}.



    if pcTagNsList = ? or pcTagNsList = "" then
       pcTagNsList = "*".

    if pcTagList = ? or pcTagList = "" then
       pcTagList = "*".

    if pcAttrNsList = ? or pcAttrNsList = "" then
       pcAttrNsList = "*".

    if pcAttrList = ? or pcAttrList = "" then
       pcAttrList = "*".

    if pcAttrValueList = ? or pcAttrValueList = "" then
       pcAttrValueList = "*".



    create x-noderef hAttr.

    lFound = no.

    _Element:

    repeat:

        if lBefore then 
           lBefore = no.

        else do:

            lOk = getNextNode( phContainer, phCursorNode ).
            if not lOk then leave _Element.

        end. /* else */

        if  phCursorNode:subtype = "text"
        and pcTagList = "%text%" then do:

            lFound = yes.
            leave _Element.

        end. /*tag = "%text%" */

        else
        if  phCursorNode:subtype = "element" 
        and can-do( pcTagList, ( if phCursorNode:namespace-uri <> "" then phCursorNode:local-name else phCursorNode:name ) )
        and can-do( pcTagNsList,    phCursorNode:namespace-uri ) then do:

            if  pcAttrList      = "*"
            and pcAttrNsList    = "*"
            and pcAttrValueList = "*" then

                lOk = yes.

            else do:

                lOk = no.

                iLen = num-entries( phCursorNode:attribute-names ). 

                _Attr:

                do i = 1 to iLen:

                    phCursorNode:get-attribute-node( hAttr, entry( i, phCursorNode:attribute-names ) ).

                    &if {&pro_xProversion} <= "09.1d" &then

                        /* 9.1d xml attribute namespace bug */

                        if  hAttr:namespace-uri = ""
                        and index( hAttr:name, ":" ) > 0 then do:

                            if  can-do( pcAttrList,         substr( hAttr:name, index( hAttr:name, ":" ) + 1 ) ) 
                            and can-do( pcAttrValueList,    hAttr:node-value ) then do:

                                lOk = yes.
                                leave _Attr.

                            end. /* can-do( cAttr ) */

                        end. /* namespace-uri = "" */

                        else do:

                            if  can-do( pcAttrList,    ( if hAttr:namespace-uri <> "" then hAttr:local-name else hAttr:name ) ) 
                            and can-do( pcAttrNsList,       hAttr:namespace-uri )
                            and can-do( pcAttrValueList,    hAttr:node-value ) then do:

                                lOk = yes.
                                leave _Attr.

                            end. /* can-do( cAttr ) */

                        end. /* else */

                    &else

                        if  can-do( pcAttrList,    ( if hAttr:namespace-uri <> "" then hAttr:local-name else hAttr:name ) ) 
                        and can-do( pcAttrNsList,       hAttr:namespace-uri )
                        and can-do( pcAttrValueList,    hAttr:node-value ) then do:

                            lOk = yes.
                            leave _Attr.

                        end. /* can-do( cAttr ) */

                    &endif

                end. /* do i */

            end. /* else */

            if lOk then do:

                lFound = yes.
                leave _Element.

            end. /* lOk */

        end. /* subtype = "element" */

    end. /* repeat */

    return lFound.

end function. /* xml_getElementByAttr */

function xml_drillElementByAttr returns log ( 

    input phContainer       as handle, 
    input phCursorNode      as handle,
    input pcDrillTagNsList  as char,
    input pcDrillTagList    as char,
    input pcTagNsList       as char,
    input pcTagList         as char,
    input pcAttrNsList      as char,
    input pcAttrList        as char,
    input pcAttrValueList   as char ):

    define var hDoc         as handle no-undo.
    define var hAttr        as handle no-undo.
    define var hRootNode    as handle no-undo.
    define var lBefore      as log no-undo.
    define var lFound       as log no-undo.
    define var iLen         as int no-undo.

    define var str          as char no-undo.
    define var lOk          as log no-undo.
    define var i            as int no-undo.

    create widget-pool.

    if not valid-handle( phContainer ) 
    or phContainer:type <> "x-noderef" and phContainer:type <> "x-document" then

        {slib/err_throw "'xml_invalid_xnoderef'"}.

    if phContainer:type = "x-document" then do:

        hDoc = phContainer.

        create x-noderef hRootNode.
        create x-noderef phContainer.

        hDoc:get-document-element( hRootNode ).
        hDoc:get-document-element( phContainer ).

    end. /* type = "x-document" */

    else do:

        hDoc = phContainer:owner-document.

        create x-noderef hRootNode.
        hDoc:get-document-element( hRootNode ).

    end. /* else */

    lOk = phContainer:owner-document <> ? no-error.
    if not lOk then

        {slib/err_throw "'xml_invalid_xnoderef_ref'"}.



    if not valid-handle( phCursorNode )
    or phCursorNode:type <> "x-noderef" then

        {slib/err_throw "'xml_invalid_xnoderef'"}.

    lBefore = no.

    lOk = phCursorNode:owner-document <> ? no-error.
    if not lOk then do:

        if phContainer:unique-id = hRootNode:unique-id then
           hDoc:get-document-element( phCursorNode ).

        else do:

           phContainer:get-parent( phCursorNode ).
           phCursorNode:get-child( phCursorNode, phContainer:child-num ).

        end. /* else */

        lBefore = yes.

    end. /* not lOk */

    lOk = phCursorNode:owner-document <> ? no-error.
    if not lOk then

        {slib/err_throw "'xml_invalid_xnoderef_ref'"}.



    if pcDrillTagNsList = ? or pcDrillTagNsList = "" then
       pcDrillTagNsList = "*".

    if pcDrillTagList = ? or pcDrillTagList = "" then
       pcDrillTagList = "*".

    if pcTagNsList = ? or pcTagNsList = "" then
       pcTagNsList = "*".

    if pcTagList = ? or pcTagList = "" then
       pcTagList = "*".

    if pcAttrNsList = ? or pcAttrNsList = "" then
       pcAttrNsList = "*".

    if pcAttrList = ? or pcAttrList = "" then
       pcAttrList = "*".

    if pcAttrValueList = ? or pcAttrValueList = "" then
       pcAttrValueList = "*".



    create x-noderef hAttr.

    lFound = no.

    _Element:

    repeat:

        if lBefore then 
           lBefore = no.

        else do:

            lOk = drillNode( phContainer, phCursorNode, pcDrillTagNsList, pcDrillTagList ).
            if not lOk then leave _Element.

        end. /* else */

        if  phCursorNode:subtype = "text"
        and pcTagList = "%text%" then do:

            lFound = yes.
            leave _Element.

        end. /* tag = "%text%" */

        else
        if  phCursorNode:subtype = "element" 
        and can-do( pcTagList, ( if phCursorNode:namespace-uri <> "" then phCursorNode:local-name else phCursorNode:name ) )
        and can-do( pcTagNsList,    phCursorNode:namespace-uri ) then do:

            if  pcAttrList       = "*"
            and pcAttrNsList     = "*"
            and pcAttrValueList  = "*" then

                lOk = yes.

            else do:

                lOk = no.

                iLen = num-entries( phCursorNode:attribute-names ). 

                _Attr:

                do i = 1 to iLen:

                    phCursorNode:get-attribute-node( hAttr, entry( i, phCursorNode:attribute-names ) ).

                    &if {&pro_xProversion} <= "09.1d" &then

                        /* 9.1d xml attribute namespace bug */

                        if  hAttr:namespace-uri = ""
                        and index( hAttr:name, ":" ) > 0 then do:

                            if  can-do( pcAttrList,         substr( hAttr:name, index( hAttr:name, ":" ) + 1 ) ) 
                            and can-do( pcAttrValueList,    hAttr:node-value ) then do:

                                lOk = yes.
                                leave _Attr.

                            end. /* can-do( cAttr ) */

                        end. /* namespace-uri = "" */

                        else do:

                            if  can-do( pcAttrList,    ( if hAttr:namespace-uri <> "" then hAttr:local-name else hAttr:name ) ) 
                            and can-do( pcAttrNsList,       hAttr:namespace-uri )
                            and can-do( pcAttrValueList,    hAttr:node-value ) then do:

                                lOk = yes.
                                leave _Attr.

                            end. /* can-do( cAttr ) */

                        end. /* else */

                    &else

                        if  can-do( pcAttrList,    ( if hAttr:namespace-uri <> "" then hAttr:local-name else hAttr:name ) ) 
                        and can-do( pcAttrNsList,       hAttr:namespace-uri )
                        and can-do( pcAttrValueList,    hAttr:node-value ) then do:

                            lOk = yes.
                            leave _Attr.

                        end. /* can-do( cAttr ) */

                    &endif

                end. /* do i */

            end. /* else */

            if lOk then do:

                lFound = yes.
                leave _Element.

            end. /* lOk */

        end. /* subtype = "element" */

    end. /* repeat */

    return lFound.

end function. /* xml_drillElementByAttr */

function xml_getChildByAttr returns log ( 

    input phParent          as handle, 
    input phCursorNode      as handle,
    input pcTagNsList       as char,
    input pcTagList         as char,
    input pcAttrNsList      as char,
    input pcAttrList        as char,
    input pcAttrValueList   as char ):

    define var cTagList     as char no-undo.
    define var cTagNsList   as char no-undo.

    define var hDoc         as handle no-undo.
    define var hAttr        as handle no-undo.
    define var hRootNode    as handle no-undo.
    define var lBefore      as log no-undo.
    define var lFound       as log no-undo.
    define var iLen         as int no-undo.

    define var str          as char no-undo.
    define var lOk          as log no-undo.
    define var i            as int no-undo.

    create widget-pool.

    if not valid-handle( phParent ) 
    or phParent:type <> "x-noderef" and phParent:type <> "x-document" then

        {slib/err_throw "'xml_invalid_xnoderef'"}.

    if not valid-handle( phCursorNode )
    or phCursorNode:type <> "x-noderef" then

        {slib/err_throw "'xml_invalid_xnoderef'"}.



    lBefore = no.

    lOk = phCursorNode:owner-document <> ? no-error.
    if lOk then do:

        hDoc = phCursorNode:owner-document.

        create x-noderef hRootNode.
        hDoc:get-document-element( hRootNode ).

        if phCursorNode:unique-id <> hRootNode:unique-id then do:

            create x-noderef phParent.
            phCursorNode:get-parent( phParent ).

        end. /* phCursorNode <> hRootNode */

    end. /* lOk */

    else do:

        if phParent:type = "x-document" then do:

            hDoc = phParent.

            create x-noderef phParent.
            hDoc:get-document-element( phParent ).

        end. /* type = "x-document" */

        lOk = phParent:owner-document <> ? no-error.
        if not lOk then

            {slib/err_throw "'xml_invalid_xnoderef_ref'"}.

        if phParent:num-children = 0 then return no.
        phParent:get-child( phCursorNode, 1 ).

        lBefore = yes.

    end. /* else */



    if pcTagNsList = ? or pcTagNsList = "" then
       pcTagNsList = "*".

    if pcTagList = ? or pcTagList = "" then
       pcTagList = "*".

    if pcAttrNsList = ? or pcAttrNsList = "" then
       pcAttrNsList = "*".

    if pcAttrList = ? or pcAttrList = "" then
       pcAttrList = "*".

    if pcAttrValueList = ? or pcAttrValueList = "" then
       pcAttrValueList = "*".



    create x-noderef hAttr.

    lFound = no.

    _Child:

    repeat:

        if lBefore then
           lBefore = no.

        else do:

            if phCursorNode:unique-id = phParent:unique-id then leave.
            if phCursorNode:child-num = phParent:num-children then leave.

            phParent:get-child( phCursorNode, phCursorNode:child-num + 1 ).

        end. /* else */



        if  phCursorNode:subtype = "text"
        and pcTagList = "%text%" then do:

            lFound = yes.
            leave _Child.

        end. /* tag = "%text%" */

        else
        if  phCursorNode:subtype = "element" 
        and can-do( pcTagList, ( if phCursorNode:namespace-uri <> "" then phCursorNode:local-name else phCursorNode:name ) )
        and can-do( pcTagNsList,    phCursorNode:namespace-uri ) then do:

            if  pcAttrList       = "*"
            and pcAttrNsList     = "*"
            and pcAttrValueList  = "*" then

                lOk = yes.

            else do:

                lOk = no.

                iLen = num-entries( phCursorNode:attribute-names ).

                _Attr:

                do i = 1 to iLen:

                    phCursorNode:get-attribute-node( hAttr, entry( i, phCursorNode:attribute-names ) ).

                    &if {&pro_xProversion} <= "09.1d" &then

                        /* 9.1d xml attribute namespace bug */

                        if  hAttr:namespace-uri = ""
                        and index( hAttr:name, ":" ) > 0 then do:

                            if  can-do( pcAttrList,         substr( hAttr:name, index( hAttr:name, ":" ) + 1 ) ) 
                            and can-do( pcAttrValueList,    hAttr:node-value ) then do:

                                lOk = yes.
                                leave _Attr.

                            end. /* can-do( cAttr ) */

                        end. /* namespace-uri = "" */

                        else do:

                            if  can-do( pcAttrList,    ( if hAttr:namespace-uri <> "" then hAttr:local-name else hAttr:name ) ) 
                            and can-do( pcAttrNsList,       hAttr:namespace-uri )
                            and can-do( pcAttrValueList,    hAttr:node-value ) then do:

                                lOk = yes.
                                leave _Attr.

                            end. /* can-do( cAttr ) */

                        end. /* else */

                    &else

                        if  can-do( pcAttrList,    ( if hAttr:namespace-uri <> "" then hAttr:local-name else hAttr:name ) ) 
                        and can-do( pcAttrNsList,       hAttr:namespace-uri )
                        and can-do( pcAttrValueList,    hAttr:node-value ) then do:

                            lOk = yes.
                            leave _Attr.

                        end. /* can-do( cAttr ) */

                    &endif

                end. /* do i */

            end. /* else */

            if lOk then do:

                lFound = yes.
                leave _Child.

            end. /* lOk */

        end. /* subtype = "element" */

    end. /* repeat */

    return lFound.

end function. /* xml_getChildByAttr */



procedure xml_setAttrByNamespace:

    define input param phNode       as handle no-undo.
    define input param pcAttrNs     as char no-undo.
    define input param pcAttrNsPre  as char no-undo.
    define input param pcAttr       as char no-undo.
    define input param pcValue      as char no-undo.

    define var hDoc     as handle no-undo.
    define var hNode    as handle no-undo.
    define var lOk      as log no-undo.
    define var i        as int no-undo.
    define var j        as int no-undo.

    create widget-pool.

    create x-noderef hNode.

    lOk = valid-handle( phNode ) and phNode:type = "x-noderef" and phNode:owner-document <> ? no-error.
    if not lOk then 

        {slib/err_throw "'xml_invalid_xnoderef_ref'"}.



    if pcAttrNs = "" then
       pcAttrNs = ?.

    if pcAttrNsPre = "" then
       pcAttrNsPre = ?.

    if pcAttr = "" then
       pcAttr = ?.

    if pcAttr = ? then

        {slib/err_throw "'xml_blank_attr'"}.



    lOk = no.

    j = num-entries( phNode:attribute-names ).

    do i = 1 to j:

        phNode:get-attribute-node( hNode, entry( i, phNode:attribute-names ) ).

        &if {&pro_xProversion} <= "09.1d" &then

            /* 9.1d xml attribute namespace bug */

            if  hNode:namespace-uri = ""
            and index( hNode:name, ":" ) > 0 then do:

                if ( pcAttr = ? or pcAttr = substr( hNode:name, index( hNode:name, ":" ) + 1 ) ) then do:

                    lOk = yes.
                    leave.

                end. /* pcAttr */

            end. /* namespace-uri = "" */

            else do:

                if  ( pcAttr    = ? or pcAttr   = ( if hNode:namespace-uri <> "" then hNode:local-name else hNode:name ) ) 
                and ( pcAttrNs  = ? or pcAttrNs =      hNode:namespace-uri ) then do:

                    lOk = yes.
                    leave.

                end. /* pcAttr */

            end. /* else */

        &else

            if  ( pcAttr    = ? or pcAttr   = ( if hNode:namespace-uri <> "" then hNode:local-name else hNode:name ) ) 
            and ( pcAttrNs  = ? or pcAttrNs =      hNode:namespace-uri ) then do:

                lOk = yes.
                leave.

            end. /* pcAttr */

        &endif

    end. /* 1 to j */



    if lOk then

        hNode:node-value = pcValue.

    else do:

        if pcAttrNs = ?
        or pcAttrNsPre = ? then

            phNode:set-attribute( pcAttr, pcValue ).

        else do:

            hDoc = phNode:owner-document.
            hDoc:create-node-namespace( hNode, pcAttrNs, pcAttr, "attribute" ).

            phNode:set-attribute-node( hNode ).
            hNode:namespace-prefix = pcAttrNsPre.
            hNode:node-value = pcValue.

        end. /* else */

    end. /* else */

end procedure. /* xml_setAttrByNamespace */

function xml_getAttrByNamespace returns char ( phNode as handle, pcAttrNs as char, pcAttr as char ):

    define var hNode    as handle no-undo.
    define var lOk      as log no-undo.
    define var i        as int no-undo.
    define var j        as int no-undo.

    create widget-pool.

    create x-noderef hNode.

    lOk = valid-handle( phNode ) and phNode:type = "x-noderef" and phNode:owner-document <> ? no-error.
    if not lOk then 

        {slib/err_throw "'xml_invalid_xnoderef_ref'"}.

    if pcAttrNs = "" then
       pcAttrNs = ?.

    if pcAttr = "" then
       pcAttr = ?.



    lOk = no.

    j = num-entries( phNode:attribute-names ).

    do i = 1 to j:

        phNode:get-attribute-node( hNode, entry( i, phNode:attribute-names ) ).

        &if {&pro_xProversion} <= "09.1d" &then

            /* 9.1d xml attribute namespace bug */

            if  hNode:namespace-uri = ""
            and index( hNode:name, ":" ) > 0 then do:

                if ( pcAttr = ? or pcAttr = substr( hNode:name, index( hNode:name, ":" ) + 1 ) ) then do:

                    lOk = yes.
                    leave.

                end. /* pcAttr */

            end. /* namespace-uri = "" */

            else do:

                if  ( pcAttr    = ? or pcAttr   = ( if hNode:namespace-uri <> "" then hNode:local-name else hNode:name ) ) 
                and ( pcAttrNs  = ? or pcAttrNs =      hNode:namespace-uri ) then do:

                    lOk = yes.
                    leave.

                end. /* pcAttr */

            end. /* else */

        &else

            if  ( pcAttr    = ? or pcAttr   = ( if hNode:namespace-uri <> "" then hNode:local-name else hNode:name ) ) 
            and ( pcAttrNs  = ? or pcAttrNs =      hNode:namespace-uri ) then do:

                lOk = yes.
                leave.

            end. /* pcAttr */

        &endif

    end. /* 1 to j */

    if lOk then
        return hNode:node-value.

    return ?.

end function. /* xml_getAttrByNamespace */

function xml_getAttrNodeByNamespace returns log ( phNode as handle, pcAttrNs as char, pcAttr as char, phAttr as handle ):

    define var lOk  as log no-undo.
    define var i    as int no-undo.
    define var j    as int no-undo.

    create widget-pool.

    lOk = valid-handle( phNode ) and phNode:type = "x-noderef" and phNode:owner-document <> ? no-error.
    if not lOk then 

        {slib/err_throw "'xml_invalid_xnoderef_ref'"}.

    lOk = valid-handle( phAttr ) and phAttr:type = "x-noderef" no-error.
    if not lOk then 

        {slib/err_throw "'xml_invalid_xnoderef'"}.

    if pcAttrNs = "" then
       pcAttrNs = ?.

    if pcAttr = "" then
       pcAttr = ?.



    lOk = no.

    j = num-entries( phNode:attribute-names ).

    do i = 1 to j:

        phNode:get-attribute-node( phAttr, entry( i, phNode:attribute-names ) ).

        &if {&pro_xProversion} <= "09.1d" &then

            /* 9.1d xml attribute namespace bug */

            if  phAttr:namespace-uri = ""
            and index( phAttr:name, ":" ) > 0 then do:

                if ( pcAttr = ? or pcAttr = substr( phAttr:name, index( phAttr:name, ":" ) + 1 ) ) then do:

                    lOk = yes.
                    leave.

                end. /* pcAttr */

            end. /* namespace-uri = "" */

            else do:

                if  ( pcAttr    = ? or pcAttr   = ( if phAttr:namespace-uri <> "" then phAttr:local-name else phAttr:name ) ) 
                and ( pcAttrNs  = ? or pcAttrNs =      phAttr:namespace-uri ) then do:

                    lOk = yes.
                    leave.

                end. /* pcAttr */

            end. /* else */

        &else

            if  ( pcAttr    = ? or pcAttr   = ( if phAttr:namespace-uri <> "" then phAttr:local-name else phAttr:name ) ) 
            and ( pcAttrNs  = ? or pcAttrNs =      phAttr:namespace-uri ) then do:

                lOk = yes.
                leave.

            end. /* pcAttr */

        &endif

    end. /* 1 to j */

    return lOk.

end function. /* xml_getAttrNodeByNamespace */

/***
procedure breakTagExp private:

    define input    param pcTagExp      as char no-undo.
    define output   param pcTagList     as char no-undo.
    define output   param pcTagNsList   as char no-undo.

    define var cTagExp as char no-undo.

    if pcTagExp = ? then
       pcTagExp = "".

    pcTagExp = trim( pcTagExp ).

    if pcTagExp = "%text%" then do:

        assign
            pcTagList    = "%text%"
            pcTagNsList  = "".

    end. /* pcTagExp = "%text%" */

    else do:

        assign
            cTagExp     = pro_parseWordList( pcTagExp, yes, no )
            cTagExp     = replace( replace( replace( cTagExp,

                chr(1) + "," + chr(1),  "," ),
                chr(1) + ",",           "," ),
                         "," + chr(1),  "," )

            pcTagList   = ""
            pcTagNsList = "".
    
        case num-entries( cTagExp, chr(1) ):
        
            when 0 then do:

                assign
                    pcTagList   = "*"
                    pcTagNsList = "*".

            end. /* when 0 */

            /* [1] */
            /* tag */

            when 1 then do:

                assign
                    pcTagList   = pro_unquote( cTagExp )
                    pcTagNsList = "*".

            end. /* when 1 */

            /* [-1-][------2------]3[4] */
            /* xmlns( "namespace" ):tag */

            when 4 then do:

                if not ( entry( 1, cTagExp, chr(1) ) = "xmlns"
                     and entry( 2, cTagExp, chr(1) ) matches "(*)"
                     and entry( 3, cTagExp, chr(1) ) = ":" ) then

                    {slib/err_throw "'xml_invalid_tag_exp'" pcTagExp}.

                assign
                    pcTagList   = pro_unquote       ( entry( 4, cTagExp, chr(1) ) )
                    pcTagNsList = pro_unparenthesis ( entry( 2, cTagExp, chr(1) ) ).

                if num-entries( pro_parseWordList( pcTagNsList, yes, no ), chr(1) ) <> 1 then

                    {slib/err_throw "'xml_invalid_tag_exp'" pcTagExp}.

                assign
                    pcTagNsList = pro_unquote( pcTagNsList ).

            end. /* when 6 */

            otherwise

                {slib/err_throw "'xml_invalid_tag_exp'" pcTagExp}.

        end case. /* num-entries */

    end. /* else */

end procedure. /* breakTagExp */

procedure breakAttrExp private:

    define input    param pcAttrExp     as char no-undo.
    define output   param pcAttrList    as char no-undo.
    define output   param pcAttrNsList  as char no-undo.

    define var cAttrExp as char no-undo.

    if pcAttrExp = ? then
       pcAttrExp = "".

    pcAttrExp = trim( pcAttrExp ).

    assign
         cAttrExp = pro_parseWordList( pcAttrExp, yes, no )
         cAttrExp = replace( replace( replace( cAttrExp,

                chr(1) + "," + chr(1),  "," ),
                chr(1) + ",",           "," ),
                         "," + chr(1),  "," ).

    case num-entries( cAttrExp, chr(1) ):

        when 0 then do:

            assign
                pcAttrList      = "*"
                pcAttrNsList    = "*".

        end. /* when 1 */

        when 1 then do:

            assign
                pcAttrList      = pro_unquote( cAttrExp )
                pcAttrNsList    = "*".

        end. /* when 1 */
no
        when 4 then do:

            if not ( entry( 1, cAttrExp, chr(1) ) = "xmlns"
                 and entry( 2, cAttrExp, chr(1) ) matches "(*)"
                 and entry( 3, cAttrExp, chr(1) ) = ":" ) then

                {slib/err_throw "'xml_invalid_attr_exp'" pcAttrExp}.

            assign
                pcAttrList      = pro_unquote       ( entry( 4, cAttrExp, chr(1) ) )
                pcAttrNsList    = pro_unparenthesis ( entry( 2, cAttrExp, chr(1) ) ).

            if num-entries( pro_parseWordList( pcAttrNsList, yes, no ), chr(1) ) <> 1 then

                {slib/err_throw "'xml_invalid_attr_exp'" pcAttrExp}.

            assign
                pcAttrNsList    = pro_unquote( pcAttrNsList ).

        end. /* when 4 */

        otherwise 

            {slib/err_throw "'xml_invalid_attr_exp'" pcAttrExp}.

    end case. /* num-entries */

end procedure. /* breakAttrExp */

procedure breakAttrValueExp private:

    define input    param pcAttrValueExp    as char no-undo.
    define output   param pcAttrList        as char no-undo.
    define output   param pcAttrNsList      as char no-undo.
    define output   param pcAttrValueList   as char no-undo.

    define var cAttrValueExp    as char no-undo.
    define var iLen             as int no-undo.
    define var i                as int no-undo.

    if pcAttrValueExp = ? then
       pcAttrValueExp = "".

    pcAttrValueExp = trim( pcAttrValueExp ).

    assign
        cAttrValueExp    = pro_parseWordList( pcAttrValueExp, yes, no )
        cAttrValueExp    = replace( replace( replace( cAttrValueExp,

            chr(1) + "," + chr(1),  "," ),
            chr(1) + ",",           "," ),
                     "," + chr(1),  "," )

        pcAttrList      = ""
        pcAttrNsList    = ""
        pcAttrValueList = "".

    case num-entries( cAttrValueExp, chr(1) ):

        when 0 then do:

            assign
                pcAttrList      = "*"
                pcAttrNsList    = "*"
                pcAttrValueList = "*".

        end. /* when 1 */

        /* [-1] */
        /* attr */

        when 1 then do:

            assign
                pcAttrList      = pro_unquote( cAttrValueExp )
                pcAttrNsList    = "*"
                pcAttrValueList = "*".

        end. /* when 1 */

        /* [-1] 2 [--3--] */
        /* attr = "value" */

        when 3 then do:

            if not entry( 2, cAttrValueExp, chr(1) ) = "=" then

                {slib/err_throw "'xml_invalid_attr_val_exp'" pcAttrValueExp}.

            assign
                pcAttrList      = pro_unquote( entry( 1, cAttrValueExp, chr(1) ) )
                pcAttrNsList    = "*"
                pcAttrValueList = pro_unquote( entry( 3, cAttrValueExp, chr(1) ) ).

        end. /* when 3 */

        /* [-1-][------2------]3[-4] */
        /* xmlns( "namespace" ):attr */
    
        when 4 then do:

            if not ( entry( 1, cAttrValueExp, chr(1) ) = "xmlns"
                 and entry( 2, cAttrValueExp, chr(1) ) matches "(*)"
                 and entry( 3, cAttrValueExp, chr(1) ) = ":" ) then

                {slib/err_throw "'xml_invalid_attr_val_exp'" pcAttrValueExp}.

            assign
                pcAttrList      = pro_unquote       ( entry( 4, cAttrValueExp, chr(1) ) )
                pcAttrNsList    = pro_unparenthesis ( entry( 2, cAttrValueExp, chr(1) ) )
                pcAttrValueList = "*".
            
            if num-entries( pro_parseWordList( pcAttrNsList, yes, no ), chr(1) ) <> 1 then

                {slib/err_throw "'xml_invalid_attr_val_exp'" pcAttrValueExp}.

            assign
                pcAttrNsList    = pro_unquote( pcAttrNsList ).

        end. /* when 4 */

        /* [-1-][------2------]3[-4] 5 [--6--] */
        /* xmlns( "namespace" ):attr = "value" */
    
        when 6 then do:

            if not ( entry( 1, cAttrValueExp, chr(1) ) = "xmlns"
                 and entry( 2, cAttrValueExp, chr(1) ) matches "(*)"
                 and entry( 3, cAttrValueExp, chr(1) ) = ":"
                 and entry( 5, cAttrValueExp, chr(1) ) = "=" ) then

                {slib/err_throw "'xml_invalid_attr_val_exp'" pcAttrValueExp}.

            assign
                pcAttrList      = pro_unquote       ( entry( 4, cAttrValueExp, chr(1) ) )
                pcAttrNsList    = pro_unparenthesis ( entry( 2, cAttrValueExp, chr(1) ) )
                pcAttrValueList = pro_unquote       ( entry( 6, cAttrValueExp, chr(1) ) ).
            
            if num-entries( pro_parseWordList( pcAttrNsList, yes, no ), chr(1) ) <> 1 then

                {slib/err_throw "'xml_invalid_attr_val_exp'" pcAttrValueExp}.

            assign
                pcAttrNsList = pro_unquote( pcAttrNsList ).

        end. /* when 6 */

        otherwise

            {slib/err_throw "'xml_invalid_attr_val_exp'" pcAttrValueExp}.

    end case. /* num-entries */

end procedure. /* breakAttrValueExp */
***/



function xml_getNextNode returns log ( phContainer as handle, phCursorNode as handle ):

    define var hDoc         as handle no-undo.
    define var hRootNode    as handle no-undo.
    define var lOk          as log no-undo.

    if not valid-handle( phContainer ) 
    or phContainer:type <> "x-noderef" and phContainer:type <> "x-document" then

        {slib/err_throw "'xml_invalid_xnoderef'"}.

    if phContainer:type = "x-document" then do:

        hDoc = phContainer.

        create x-noderef hRootNode.
        create x-noderef phContainer.

        hDoc:get-document-element( hRootNode ).
        hDoc:get-document-element( phContainer ).

    end. /* type = "x-document" */

    else do:

        hDoc = phContainer:owner-document.

        create x-noderef hRootNode.
        hDoc:get-document-element( hRootNode ).

    end. /* else */

    lOk = phContainer:owner-document <> ? no-error.
    if not lOk then

        {slib/err_throw "'xml_invalid_xnoderef_ref'"}.



    if not valid-handle( phCursorNode )
    or phCursorNode:type <> "x-noderef" then

        {slib/err_throw "'xml_invalid_xnoderef'"}.

    lOk = phCursorNode:owner-document <> ? no-error.
    if not lOk then do:

        if phContainer:unique-id = hRootNode:unique-id then
           hDoc:get-document-element( phCursorNode ).

        else do:

           phContainer:get-parent( phCursorNode ).
           phCursorNode:get-child( phCursorNode, phContainer:child-num ).

        end. /* else */

    end. /* not lOk */

    lOk = phCursorNode:owner-document <> ? no-error.
    if not lOk then

        {slib/err_throw "'xml_invalid_xnoderef_ref'"}.

    return getNextNode( phContainer, phCursorNode ).

end function. /* xml_getNextNode */

function getNextNode returns log private ( phContainer as handle, phCursorNode as handle ):

    define var iLastChild   as int no-undo.
    define var lOffEnd      as log no-undo.

    lOffEnd = no.

    if phCursorNode:num-children > 0 then
       phCursorNode:get-child( phCursorNode, 1 ).

    else do:

        repeat:

            if phCursorNode:unique-id = phContainer:unique-id then do:

                lOffEnd = yes.
                leave.

            end. /* cursor = container */

            iLastChild = phCursorNode:child-num.
            phCursorNode:get-parent( phCursorNode ).

            if iLastChild < phCursorNode:num-children then do:

                phCursorNode:get-child( phCursorNode, iLastChild + 1 ).
                leave.

            end. /* iLastChild < num-children */

        end. /* repeat */

    end. /* else */

    if lOffEnd then
        return no.

    return yes.

end function. /* getNextNode */



function xml_getPrevNode returns log ( phContainer as handle, phCursorNode as handle ):

    define var hDoc         as handle no-undo.
    define var hRootNode    as handle no-undo.
    define var lOk          as log no-undo.

    if not valid-handle( phContainer ) 
    or phContainer:type <> "x-noderef" and phContainer:type <> "x-document" then

        {slib/err_throw "'xml_invalid_xnoderef'"}.

    if phContainer:type = "x-document" then do:

        hDoc = phContainer.

        create x-noderef hRootNode.
        create x-noderef phContainer.

        hDoc:get-document-element( hRootNode ).
        hDoc:get-document-element( phContainer ).

    end. /* type = "x-document" */

    else do:

        hDoc = phContainer:owner-document.

        create x-noderef hRootNode.
        hDoc:get-document-element( hRootNode ).

    end. /* else */

    lOk = phContainer:owner-document <> ? no-error.
    if not lOk then

        {slib/err_throw "'xml_invalid_xnoderef_ref'"}.



    if not valid-handle( phCursorNode )
    or phCursorNode:type <> "x-noderef" then

        {slib/err_throw "'xml_invalid_xnoderef'"}.

    lOk = phCursorNode:owner-document <> ? no-error.
    if not lOk then do:

        if phContainer:unique-id = hRootNode:unique-id then
           hDoc:get-document-element( phCursorNode ).

        else do:

           phContainer:get-parent( phCursorNode ).
           phCursorNode:get-child( phCursorNode, phContainer:child-num ).

        end. /* else */

    end. /* not lOk */

    lOk = phCursorNode:owner-document <> ? no-error.
    if not lOk then

        {slib/err_throw "'xml_invalid_xnoderef_ref'"}.

    return getPrevNode( phContainer, phCursorNode ).

end function. /* xml_getPrevNode */

function getPrevNode returns log private ( phContainer as handle, phCursorNode as handle ):

    define var iLastChild   as int no-undo.
    define var lOffEnd      as log no-undo.

    lOffEnd = no.

    if phCursorNode:unique-id = phContainer:unique-id then
        lOffEnd = yes.

    else
    if phCursorNode:child-num = 1 then
       phCursorNode:get-parent( phCursorNode ).

    else do:

        iLastChild = phCursorNode:child-num.
        phCursorNode:get-parent( phCursorNode ).
        phCursorNode:get-child( phCursorNode, iLastChild - 1 ).

        do while phCursorNode:num-children > 0:
            phCursorNode:get-child( phCursorNode, phCursorNode:num-children ).
        end.

    end. /* else */

    if lOffEnd then
        return no.

    return yes.

end function. /* getPrevNode */



function xml_drillNode returns log ( phContainer as handle, phCursorNode as handle, pcTagNsList as char, pcTagList as char ):

    define var hDoc         as handle no-undo.
    define var hRootNode    as handle no-undo.
    define var lOk          as log no-undo.

    if not valid-handle( phContainer ) 
    or phContainer:type <> "x-noderef" and phContainer:type <> "x-document" then

        {slib/err_throw "'xml_invalid_xnoderef'"}.

    if phContainer:type = "x-document" then do:

        hDoc = phContainer.

        create x-noderef hRootNode.
        create x-noderef phContainer.

        hDoc:get-document-element( hRootNode ).
        hDoc:get-document-element( phContainer ).

    end. /* type = "x-document" */

    else do:

        hDoc = phContainer:owner-document.

        create x-noderef hRootNode.
        hDoc:get-document-element( hRootNode ).

    end. /* else */

    lOk = phContainer:owner-document <> ? no-error.
    if not lOk then

        {slib/err_throw "'xml_invalid_xnoderef_ref'"}.



    if not valid-handle( phCursorNode )
    or phCursorNode:type <> "x-noderef" then

        {slib/err_throw "'xml_invalid_xnoderef'"}.

    lOk = phCursorNode:owner-document <> ? no-error.
    if not lOk then do:

        if phContainer:unique-id = hRootNode:unique-id then
           hDoc:get-document-element( phCursorNode ).

        else do:

           phContainer:get-parent( phCursorNode ).
           phCursorNode:get-child( phCursorNode, phContainer:child-num ).

        end. /* else */

    end. /* not lOk */

    lOk = phCursorNode:owner-document <> ? no-error.
    if not lOk then

        {slib/err_throw "'xml_invalid_xnoderef_ref'"}.

    return drillNode( phContainer, phCursorNode, pcTagNsList, pcTagList ).

end function. /* xml_drillNode */

function drillNode returns log private ( phContainer as handle, phCursorNode as handle, pcTagNsList as char, pcTagList as char ):

    define var iLastChild   as int no-undo.
    define var lOffEnd      as log no-undo.

    lOffEnd = no.

    if  phCursorNode:num-children > 0

    and ( can-do( pcTagList,   ( if phCursorNode:namespace-uri <> "" then phCursorNode:local-name else phCursorNode:name ) )
      and can-do( pcTagNsList,      phCursorNode:namespace-uri )

       or phCursorNode:unique-id = phContainer:unique-id ) then

        phCursorNode:get-child( phCursorNode, 1 ).

    else do:

        repeat:

            if phCursorNode:unique-id = phContainer:unique-id then do:

                lOffEnd = yes.
                leave.

            end. /* cursor = container */

            iLastChild = phCursorNode:child-num.
            phCursorNode:get-parent( phCursorNode ).

            if iLastChild < phCursorNode:num-children then do:

                phCursorNode:get-child( phCursorNode, iLastChild + 1 ).
                leave.

            end. /* iLastChild < num-children */

        end. /* repeat */

    end. /* else */

    if lOffEnd then
        return no.

    return yes.

end function. /* drillNode */



function xml_getNextTagSkipBlanks returns log (

    input           phContainer     as handle,
    input           phCursorNode    as handle,
    input-output    pcCursorPos     as char,
    output          pcStringValue   as char ):

    define var lOk as log no-undo.

    lOk = xml_getNextTag( phContainer, phCursorNode, input-output pcCursorPos, output pcStringValue ).

    if not lOk then

        return lOk.

    if phCursorNode:subtype = "text" and trim( pcStringValue ) /* also trims new line */ = "" then

        lOk = xml_getNextTag( phContainer, phCursorNode, input-output pcCursorPos, output pcStringValue ).

    return lOk.

end function. /* xml_getNextTagSkipBlanks */

function xml_getNextTag returns log (

    /* the container unlike the cursor is not moved. if the container is an x-document or null a new x-noderef is created internally */
    
    input           phContainer     as handle,
    input           phCursorNode    as handle,
    input-output    pcCursorPos     as char,
    output          pcStringValue   as char ):

    define var hDoc         as handle no-undo.
    define var hRootNode    as handle no-undo.
    define var iLastChild   as int no-undo.

    define var lOk          as log no-undo.
    define var str          as char no-undo.
    define var ptr          as memptr no-undo.
    define var i            as int no-undo.
    define var j            as int no-undo.

    if not valid-handle( phContainer ) 
    or phContainer:type <> "x-noderef" and phContainer:type <> "x-document" then

        {slib/err_throw "'xml_invalid_xnoderef'"}.

    if phContainer:type = "x-document" then do:

        hDoc = phContainer.

        create x-noderef hRootNode.
        create x-noderef phContainer.

        hDoc:get-document-element( hRootNode ).
        hDoc:get-document-element( phContainer ).

    end. /* type = "x-document" */

    else do:

        hDoc = phContainer:owner-document.

        create x-noderef hRootNode.
        hDoc:get-document-element( hRootNode ).

    end. /* else */

    lOk = phContainer:owner-document <> ? no-error.
    if not lOk then

        {slib/err_throw "'xml_invalid_xnoderef_ref'"}.



    if not valid-handle( phCursorNode )
    or phCursorNode:type <> "x-noderef" then

        {slib/err_throw "'xml_invalid_xnoderef'"}.

    lOk = phCursorNode:owner-document <> ? no-error.
    if not lOk then do:

        if phContainer:unique-id = hRootNode:unique-id then
           hDoc:get-document-element( phCursorNode ).

        else do:

           phContainer:get-parent( phCursorNode ).
           phCursorNode:get-child( phCursorNode, phContainer:child-num ).

        end. /* else */

        pcCursorPos = "before".

    end. /* not lOk */

    lOk = phCursorNode:owner-document <> ? no-error.
    if not lOk then

        {slib/err_throw "'xml_invalid_xnoderef_ref'"}.



    if phCursorNode:subtype = "element" then do:

        if pcCursorPos = "" then
           pcCursorPos = ?.

        if pcCursorPos = ? then
           pcCursorPos = "before".

        if phCursorNode:num-children = 0 and not ( pcCursorPos = "before" or pcCursorPos = "after" ) then
           pcCursorPos = "empty".

    end. /* element */

    else
    if phCursorNode:subtype = "text" then do:

        if pcCursorPos = "" then
           pcCursorPos = ?.

        if not ( pcCursorPos = "before" or pcCursorPos = "after" ) then
           pcCursorPos = ?.

    end. /* text */



    /* move cursor one tag forward */

    if pcCursorPos = "before" then do:

        if phCursorNode:subtype = "element" then
           pcCursorPos = ( if phCursorNode:num-children = 0 then "empty" else "start" ).
        
        else
        if phCursorNode:subtype = "text" then
           pcCursorPos = ?.

    end. /* before */
    
    else
    if  phCursorNode:subtype = "element"
    and pcCursorPos = "start" then do:
                        
        phCursorNode:get-child( phCursorNode, 1 ).

        if phCursorNode:subtype = "element" then
           pcCursorPos = ( if phCursorNode:num-children = 0 then "empty" else "start" ).
        
        else
        if phCursorNode:subtype = "text" then
           pcCursorPos = ?.
        
    end. /* start */

    else
    if phCursorNode:subtype = "element" and ( pcCursorPos = "end" or pcCursorPos = "empty" or pcCursorPos = "after" )
    or phCursorNode:subtype = "text" then do:

        if phCursorNode:unique-id = phContainer:unique-id then
           pcCursorPos = "after".

        else do:

            iLastChild = phCursorNode:child-num.
            phCursorNode:get-parent( phCursorNode ).

            if iLastChild < phCursorNode:num-children then do:

                phCursorNode:get-child( phCursorNode, iLastChild + 1 ). 

                if phCursorNode:subtype = "element" then
                   pcCursorPos = ( if phCursorNode:num-children = 0 then "empty" else "start" ).

                else
                if phCursorNode:subtype = "text" then
                   pcCursorPos = ?.

            end. /* child-num < num-children */

            else do:

                pcCursorPos = "end".

            end. /* else */

        end. /* else */

    end. /* end or empty */



    /* draw string value */
    
    if phCursorNode:subtype = "element" then do:

        if pcCursorPos = "start" 
        or pcCursorPos = "empty" then do:

            pcStringValue = "<" + phCursorNode:name.

            j = num-entries( phCursorNode:attribute-names ).

            do i = 1 to j:

                str = entry( i, phCursorNode:attribute-names ).

                pcStringValue = pcStringValue
                    + ' '
                    + str
                    + '=' 
                    + '"'
                    + ( if  session:cpinternal <> "utf-8"
                        and session:cpinternal <> "undefined" then

                             xml_encodeXml( unescapeStr( codepage-convert( phCursorNode:get-attribute( str ), "utf-8" ) ) )
                        else xml_encodeXml( unescapeStr( phCursorNode:get-attribute( str ) ) ) )
                    + '"'.

            end. /* do j */

            pcStringValue = pcStringValue + ( if pcCursorPos = "start" then ">" else "/>" ).

        end. /* start/empty */ 
        
        else
        if pcCursorPos = "end" then
           pcStringValue = "</" + phCursorNode:name + ">".

        else
        if pcCursorPos = "before"
        or pcCursorPos = "after" then
           pcStringValue = "".

    end. /* element */

    else
    if phCursorNode:subtype = "text" then do:

        /* because getnexttag is almost always used to selectively draw xml parts
           the procedure already convert the string value from cpinternal to utf-8
           and encodes special characters. */

        if  session:cpinternal <> "utf-8"
        and session:cpinternal <> "undefined" then

             pcStringValue = xml_encodeXml( unescapeStr( codepage-convert( phCursorNode:node-value, "utf-8" ) ) ).
        else pcStringValue = xml_encodeXml( unescapeStr( phCursorNode:node-value ) ).

    end. /* subtype = "text" */

    if pcCursorPos = "after" then
         return no.
    else return yes.

end function. /* xml_getNextTag */

function unescapeStr returns char private ( pcStr as char ):

    define var iPos     as int no-undo.
    define var iValue   as int no-undo.
    define var iBytes   as int no-undo.
    define var iAsc     as int no-undo.

    define var str      as char no-undo.
    define var i        as int no-undo.
    define var j        as int no-undo.

    iPos = 1.

    _utf8:

    repeat:

        iPos = index( pcStr, "__utf8x", iPos /*** ( if iPos = 0 then 1 else iPos + 7 /* length( "__utf8x" ) */ ) ***/ ).
        if iPos = 0 then leave _utf8.

        i = index( pcStr, "__", iPos + 7 ).
        if i = 0 then next _utf8.

        if ( i - 1 ) - ( iPos + 7 ) + 1 <> 4 then
            next _utf8.



        iValue = math_Hex2Int( substr( pcStr, iPos + 7, 4 ) ).
        if iValue = ? then next _utf8.

             if iValue < 2047       then iBytes = 2.
        else if iValue < 65355      then iBytes = 3.
        else if iValue < 2097151    then iBytes = 4.
        else if iValue < 67108863   then iBytes = 5.
        else                             iBytes = 6.



        str = "".

        j = 1.

        do i = 1 to iBytes - 1:

            iAsc = 0.

            put-bits( iAsc, 8, 1 ) = 1.
            put-bits( iAsc, 1, 6 ) = get-bits( iValue, j, 6 ).

            str = chr( iAsc ) + str.

            j = j + 6.

        end. /* 1 to iBytes */

        iAsc = 0.

        do i = 8 - iBytes + 1 to 8:
            put-bits( iAsc, i, 1 ) = 1.
        end.

        put-bits( iAsc, 1, 7 - iBytes ) = get-bits( iValue, j, 7 - iBytes ).

        str = chr( iAsc ) + str.

        assign
            substr( pcStr, iPos, ( iPos + 12 /* length( "__utf8x0000__" ) - 1 */ ) - iPos + 1 ) = str
            iPos = iPos + iBytes.

    end. /* repeat */

    return pcStr.

end function. /* unescapeStr */



function xml_compareNode returns log ( phSourceNode as handle, phTargetNode as handle ):

    define var lOk as log no-undo.

    if not valid-handle( phSourceNode )
    or phSourceNode:type <> "x-noderef" then

        {slib/err_throw "'xml_invalid_xnoderef'"}.

    lOk = phSourceNode:owner-document <> ? no-error.
    if not lOk then

        {slib/err_throw "'xml_invalid_xnoderef_ref'"}.

    if not valid-handle( phTargetNode )
    or phTargetNode:type <> "x-noderef" then

        {slib/err_throw "'xml_invalid_xnoderef'"}.

    lOk = phTargetNode:owner-document <> ? no-error.
    if not lOk then

        {slib/err_throw "'xml_invalid_xnoderef_ref'"}.



    run compareNode(
        input   phSourceNode,
        input   phTargetNode,
        output  lOk ).

    return lOk.

end function. /* compareNode */

function xml_findChild returns int ( phSourceParent as handle, phTargetNode as handle ):

    define var hSourceNode  as handle no-undo.
    define var iIndex       as int no-undo.

    define var hDoc         as handle no-undo.
    define var lOk          as log no-undo.
    define var i            as int no-undo.

    create widget-pool.

    if not valid-handle( phSourceParent ) 
    or phSourceParent:type <> "x-noderef" and phSourceParent:type <> "x-document" then

        {slib/err_throw "'xml_invalid_xnoderef'"}.

    if phSourceParent:type = "x-document" then do:

        hDoc = phSourceParent.

        create x-noderef phSourceParent.
        hDoc:get-document-element( phSourceParent ).

    end. /* type = "x-document" */

    lOk = phSourceParent:owner-document <> ? no-error.
    if not lOk then

        {slib/err_throw "'xml_invalid_xnoderef_ref'"}.

    if not valid-handle( phTargetNode )
    or phTargetNode:type <> "x-noderef" then

        {slib/err_throw "'xml_invalid_xnoderef'"}.

    lOk = phTargetNode:owner-document <> ? no-error.
    if not lOk then

        {slib/err_throw "'xml_invalid_xnoderef_ref'"}.



    iIndex = ?.

    create x-noderef hSourceNode.

    do i = 1 to phSourceParent:num-children:

        phSourceParent:get-child( hSourceNode, i ).

        run compareNode(
            input    hSourceNode,
            input   phTargetNode,
            output  lOk ).

        if lOk then do:

            iIndex = i.
            leave.
        
        end. /* lOk */

    end. /* 1 to num-children */

    return iIndex.

end function. /* xml_findChild */

procedure compareNode private:

    define input    param phSourceNode  as handle no-undo.
    define input    param phTargetNode  as handle no-undo.
    define output   param plMatch       as log no-undo.

    define var cSourceAttrList  as char no-undo.
    define var iSourceAttrLen   as int no-undo.
    define var hSourceAttrNode  as handle no-undo.

    define var cTargetAttrList  as char no-undo.
    define var iTargetAttrLen   as int no-undo.
    define var hTargetAttrNode  as handle no-undo.

    define var hSourceChildNode as handle no-undo.
    define var hTargetChildNode as handle no-undo.

    define var lOk              as log no-undo.
    define var str              as char no-undo.
    define var i                as int no-undo.

    create widget-pool.

    create x-noderef hSourceAttrNode.
    create x-noderef hTargetAttrNode.

    create x-noderef hSourceChildNode.
    create x-noderef hTargetChildNode.

    plMatch = no.

    case phSourceNode:subtype:

        when "text" then do:

            if  phSourceNode:subtype    = phTargetNode:subtype
            and phSourceNode:node-value = phTargetNode:node-value then

                plMatch = yes.

        end. /* when "text" */

        when "element" then do:

            if  phSourceNode:subtype        = phTargetNode:subtype
            and phSourceNode:name           = phTargetNode:name
            and phSourceNode:local-name     = phTargetNode:local-name
            and phSourceNode:namespace-uri  = phTargetNode:namespace-uri then do:

                assign
                   iSourceAttrLen = num-entries( phSourceNode:attribute-names )
                   iTargetAttrLen = num-entries( phTargetNode:attribute-names ).
                if iTargetAttrLen = iSourceAttrLen then do:

                    assign
                       cSourceAttrList = str_sortEntries( phSourceNode:attribute-names, no, ? )
                       cTargetAttrList = str_sortEntries( phTargetNode:attribute-names, no, ? ).
                    if cTargetAttrList = cSourceAttrList then do:

                        lOk = yes.

                        do i = 1 to iTargetAttrLen:

                            str = entry( i, cTargetAttrList ).

                            phSourceNode:get-attribute-node( hSourceAttrNode, str ).
                            phTargetNode:get-attribute-node( hTargetAttrNode, str ).

                            if hTargetAttrNode:namespace-uri    <> hSourceAttrNode:namespace-uri
                            or hTargetAttrNode:node-value       <> hSourceAttrNode:node-value then do:

                                lOk = no.
                                leave.

                            end. /* target:node-value <> source:node-value */

                        end. /* 1 to len */

                        if lOk then do:

                            if phSourceNode:num-children = phTargetNode:num-children then do:

                                if phSourceNode:num-children = 0 then 

                                    plMatch = yes.

                                else do:

                                    lOk = yes.

                                    do i = 1 to phSourceNode:num-children:

                                        phSourceNode:get-child( hSourceChildNode, i ).
                                        phTargetNode:get-child( hTargetChildNode, i ).

                                        run compareNode(
                                            input   hSourceChildNode,
                                            input   hTargetChildNode,
                                            output  lOk ).

                                        if not lOk then
                                            leave.

                                    end. /* 1 to num-children */

                                    if lOk then
                                        plMatch = yes.

                                end. /* else */

                            end. /* source:num-children = target:num-children */

                        end. /* lOk */

                    end. /* cSourceAttrList = cTargetAttrList */

                end. /* iSourceAttrLen = iTargetAttrLen */

            end. /* source = target */

        end. /* when "element" */

    end case. /* source:subtype */

end procedure. /* compareNode */



procedure xml_indentXml:

    define input param phDoc    as handle no-undo.
    define input param piIndent as int no-undo.

    define var hRoot as handle no-undo.

    create widget-pool.

    create x-noderef hRoot.

    phDoc:get-document-element( hRoot ).

    run removeBlanksRecurr( hRoot ).

    run indentXmlRecurr( phDoc, hRoot, 1, piIndent ).

end procedure. /* xml_indexXml */

procedure indentXmlRecurr private:

    define input param phDoc        as handle no-undo.
    define input param phParentNode as handle no-undo.
    define input param piLevel      as int no-undo.
    define input param piIndent     as int no-undo.

    define var hBlankNode   as handle no-undo.
    define var hNode        as handle no-undo.
    define var lOk          as log no-undo.
    define var i            as int no-undo.

    create widget-pool.

    create x-noderef hNode.
    create x-noderef hBlankNode.

    do i = 1 to phParentNode:num-children by 2:

        phParentNode:get-child( hNode, i ).

        if  hNode:subtype = "element"
        and hNode:num-children > 0 then do:

            lOk = yes.

            if hNode:num-children = 1 then do:

                hNode:get-child( hBlankNode, 1 ).
                if hBlankNode:subtype = "text" then

                    lOk = no.

            end. /* num-children = 1 */

            if lOk then
                run indentXmlRecurr( phDoc, hNode, piLevel + 1, piIndent ).

        end. /* num-children > 0 */



        phDoc:create-node( hBlankNode, "", "text" ).

        phParentNode:insert-before( hBlankNode, hNode ).

        hBlankNode:node-value = "~n" + fill( " ", piLevel * piIndent ).

    end. /* 1 to num-children */



    phDoc:create-node( hBlankNode, "", "text" ).

    phParentNode:append-child( hBlankNode ).

    hBlankNode:node-value = "~n" + fill( " ", ( piLevel - 1 ) * piIndent ).

end procedure. /* indentXmlRecurr */



procedure xml_removeBlanks:

    define input param phDoc as handle no-undo.

    define var hRoot as handle no-undo.

    create widget-pool.

    create x-noderef hRoot.

    phDoc:get-document-element( hRoot ).

    run removeBlanksRecurr( hRoot ).

end procedure. /* xml_removeBlanks */

procedure removeBlanksRecurr private:

    define input param phParentNode as handle no-undo.

    define var hNode    as handle no-undo.
    define var i        as int no-undo.

    create widget-pool.

    create x-noderef hNode.

    do i = phParentNode:num-children to 1 by -1:

        phParentNode:get-child( hNode, i ).

        case hNode:subtype:

            when "element" then do:

                if hNode:num-children > 0 then
                    run removeBlanksRecurr( hNode ).

            end. /* element */

            when "text" then do:

                if trim( hNode:node-value ) /* also trims new lines */ = "" then do:

                    if phParentNode:num-children <> 1 then
                    hNode:delete-node( ).

                end. /* trim = "" */

                /***
                if  session:cpinternal <> "utf-8"
                and session:cpinternal <> "undefined" then
                    run normalizeText( hNode ).
                ***/

            end. /* text */

        end case. /* subtype */

    end. /* num-children to 1 */

end procedure. /* removeBlanksRecurr */

procedure xml_removeAttrByNamespace:

    define input param phNode       as handle no-undo.
    define input param pcAttrNsList as char no-undo.
    define input param pcAttrList   as char no-undo.

    define var hNode    as handle no-undo.
    define var lOk      as log no-undo.
    define var str      as char no-undo.
    define var i        as int no-undo.
    define var j        as int no-undo.

    create widget-pool.

    create x-noderef hNode.

    lOk = valid-handle( phNode ) and phNode:type = "x-noderef" and phNode:owner-document <> ? no-error.
    if not lOk then 

        {slib/err_throw "'xml_invalid_xnoderef'"}.



    assign
       str  = phNode:attribute-names
       j    = num-entries( str ).

    do i = j to 1 by -1:

        phNode:get-attribute-node( hNode, entry( i, str ) ).

        &if {&pro_xProversion} <= "09.1d" &then

            /* 9.1d xml attribute namespace bug */

            if  hNode:namespace-uri = ""
            and index( hNode:name, ":" ) > 0 then do:

                if can-do( pcAttrList, substr( hNode:name, index( hNode:name, ":" ) + 1 ) ) then

                    phNode:remove-attribute( entry( i, str ) ).

            end. /* namespace-uri = "" */

            else do:

                if  can-do( pcAttrList,    ( if hNode:namespace-uri <> "" then hNode:local-name else hNode:name ) ) 
                and can-do( pcAttrNsList,       hNode:namespace-uri ) then

                    phNode:remove-attribute( entry( i, str ) ).

            end. /* else */

        &else

            if  can-do( pcAttrList,    ( if hNode:namespace-uri <> "" then hNode:local-name else hNode:name ) ) 
            and can-do( pcAttrNsList,       hNode:namespace-uri ) then

                phNode:remove-attribute( entry( i, str ) ).

        &endif

    end. /* 1 to j */

end procedure. /* xml_removeAttrByNamespace */



/* there is a bug with the import-node method in some progress versions.

   setting an existing attribute value with set-attribute on a node created with import-node
   adds another attribute with the same name instead of changing the existing attribute. 

   xml_importNode is a replacement to the import-node method. */

procedure xml_importNode:

    define input param phDoc        as handle no-undo.
    define input param phTargetNode as handle no-undo.
    define input param phSourceNode as handle no-undo.
    define input param plDeep       as log no-undo.

    define var lOk  as log no-undo.
    define var str  as char no-undo.
    define var i    as int no-undo.
    define var j    as int no-undo.

    &if {&pro_xProVersion} = "09.1D" &then

        lOk = valid-handle( phDoc ) and phDoc:type = "x-document" no-error.
        if not lOk then
    
            {slib/err_throw "'xml_invalid_xdoc'"}.
    
        lOk = valid-handle( phTargetNode ) and phTargetNode:type = "x-noderef" no-error.
        if not lOk then
    
            {slib/err_throw "'xml_invalid_xnoderef'"}.
    
        lOk = valid-handle( phSourceNode ) and phSourceNode:type = "x-noderef" and phSourceNode:owner-document <> ? no-error.
        if not lOk then
    
            {slib/err_throw "'xml_invalid_xnoderef_ref'"}.
    
    
    
        if phSourceNode:subtype = "element" then do:

            if phSourceNode:namespace-uri = "" then

            phDoc:create-node( 

                phTargetNode,
                phSourceNode:name,
                phSourceNode:subtype ).

            else

            phDoc:create-node-namespace( 
    
                phTargetNode,
                phSourceNode:namespace-uri,
                phSourceNode:name,
                phSourceNode:subtype ).

            j = num-entries( phSourceNode:attribute-names ).

            do i = 1 to j:

                str = entry( i, phSourceNode:attribute-names ).
    
                phTargetNode:set-attribute( str, phSourceNode:get-attribute( str ) ).
    
            end. /* 1 to j */

            if plDeep then
    
            run importNodeDeep(
                input phDoc,
                input phTargetNode,
                input phSourceNode ).
    
        end. /* element */
    
        else do:

            phDoc:create-node( phTargetNode, ?, "text" ).
    
            phTargetNode:node-value = phSourceNode:node-value.
    
        end. /* else */

    &else

        phDoc:import-node( phTargetNode, phSourceNode, plDeep ).

    &endif

end procedure. /* xml_importNode */

procedure importNodeDeep:

    define input param phDoc        as handle no-undo.
    define input param phTargetNode as handle no-undo.
    define input param phSourceNode as handle no-undo.

    define var hTargetNode  as handle no-undo.
    define var hSourceNode  as handle no-undo.

    define var iChild   as int no-undo.
    define var str      as char no-undo.
    define var i        as int no-undo.
    define var j        as int no-undo.

    create widget-pool.

    create x-noderef hTargetNode.
    create x-noderef hSourceNode.

    do iChild = 1 to phSourceNode:num-children:

        phSourceNode:get-child( hSourceNode, iChild ).

        if hSourceNode:subtype = "element" then do:

            if hSourceNode:namespace-uri = "" then

            phDoc:create-node( 

                hTargetNode,
                hSourceNode:name,
                hSourceNode:subtype ).

            else

            phDoc:create-node-namespace( 
    
                hTargetNode,
                hSourceNode:namespace-uri,
                hSourceNode:name,
                hSourceNode:subtype ).

            j = num-entries( hSourceNode:attribute-names ).

            do i = 1 to j:
    
                str = entry( i, hSourceNode:attribute-names ).
    
                hTargetNode:set-attribute( str, hSourceNode:get-attribute( str ) ).
    
            end. /* 1 to j */

            if hSourceNode:num-children > 0 then

            run importNodeDeep(
                input phDoc,
                input hTargetNode,
                input hSourceNode ).

        end. /* element */

        else do:

            phDoc:create-node( hTargetNode, ?, "text" ).

            hTargetNode:node-value = hSourceNode:node-value.

        end. /* else */

        phTargetNode:append-child( hTargetNode ).

    end. /* 1 to num-children */

end procedure. /* importNodeDeep */



function xml_getXmlDeclaration returns char ( phDoc as handle ):

    return '<?xml version="1.0"'
         + ( if phDoc:encoding <> "" then ' encoding="' + phDoc:encoding + '"' else '' )
         + ' standalone="yes"?>'.

end function. /* xml_getXmlDeclaration */

function xml_encodeXml returns char ( pcStr as char ):

    if pcStr = ? then
        return ?.

    return replace( replace( replace( replace( replace( pcStr, 

        '&',    '&amp;' ), 
        '"',    '&quot;' ), 
        '~'',   '&apos;' ), 
        '<',    '&lt;' ), 
        '>',    '&gt;' ).

end function. /* xml_encodeXml */

function xml_decodeXml returns char ( pcStr as char ):

    define var iPos as int no-undo.
    define var iAsc as int no-undo.

    define var i    as int no-undo.

    if pcStr = ? then
        return ?.

    pcStr = replace( replace( replace( replace( replace( pcStr, 

        '&amp;',    '&' ), 
        '&quot;',   '"' ),
        '&apos;',   '~'' ),
        '&lt;',     '<' ), 
        '&gt;',     '>' ).



    iPos = 0.

    repeat:

        iPos = index( pcStr, "&#", iPos + 1 ).
        if iPos = 0 then leave.

        i = index( pcStr, ";", iPos + 2 ).
        if i = 0 then next.

        iAsc = math_hex2int( substr( pcStr, iPos + 2, ( i - 1 ) - ( iPos + 2 ) + 1 ) ).
        if iAsc = ? then next.

        substr( pcStr, iPos, i - iPos + 1 ) = chr( iAsc ).

    end. /* repeat */

    return pcStr.

end function. /* xml_decodeXml */

function xml_encodeHtml returns char ( pcStr as char ):

    pcStr = xml_encodeXml( pcStr ).

    if pcStr = "" 
    or pcStr = ? then
       pcStr = "&nbsp;".

    return pcStr.

end function. /* xml_encodeHtml */

function xml_decodeHtml returns char ( pcStr as char ):

    if pcStr = ? then
        return ?.

    assign
        pcStr = replace( pcStr, "&nbsp;", " " )
        pcStr = xml_encodeXml( pcStr ).

    return pcStr.

end function. /* xml_encodeHtml */



function xml_Xml2Date returns date ( pcXDate as char ):

    define var tDate        as date no-undo.
    define var iMTime       as int no-undo.
    define var iTimeZone    as int no-undo.

    run date_Str2Date(

        input   pcXDate,
        input   "x-date",

        output  tDate,
        output  iMTime,
        output  iTimeZone ).

    return tDate.

end function. /* xml_Xml2Date */

&if {&pro_xProversion} >= "10" &then

function xml_Xml2Datetime returns datetime ( pcXDatetime as char ):

    return date_Str2Datetime( pcXDatetime, "x-datetime" ).

end function. /* xml_Xml2Datetime */

function xml_Xml2DatetimeTz returns datetime-tz ( pcXDatetimeTz as char ):

    return date_Str2DatetimeTz( pcXDatetimeTz, "x-datetimetz" ).

end function. /* xml_Xml2DatetimeTz */

&endif /* proversion >= 10 */



function xml_Date2Xml returns char ( ptDate as date ):

    return date_Date2Str( ptDate, 0, 0, "x-date" ).

end function. /* xml_Date2Xml */

&if {&pro_xProversion} >= "10" &then

function xml_Datetime2Xml returns char ( ptDatetime as datetime ):

    return date_Datetime2Str( ptDatetime, "x-datetime" ).

end function. /* xml_Datetime2Xml */

function xml_DatetimeTz2Xml returns char ( ptDatetimeTz as datetime-tz ):

    return date_DatetimeTz2Str( ptDatetimeTz, "x-datetimetz" ).

end function. /* xml_DatetimeTz2Xml */

&endif /* proversion >= 10 */



function xml_Xml2Dec returns dec ( pcXDec as char ):

    define var i as dec no-undo.
    define var j as dec no-undo.

    if pcXDec = "INF" then return ?.

    else
    if pcXDec = "NAN" then return ?.

    if r-index( pcXDec, "E" ) > 0 then do:

        assign
            i = ?
            j = ?

            i = decimal( entry( 1, pcXDec, "E" ) )
            j = decimal( entry( 2, pcXDec, "E" ) ) no-error.

        if  i = ?
        or  j = ? then
             return ?.
        else return i * exp( 10, j ).

    end. /* r-index( "E" ) > 0 */

    else do:

        assign
            i = ?
            i = decimal( pcXDec ) no-error.

        if  i = ? then
             return ?.
        else return i.

    end. /* else */

end function. /* xml_Xml2Dec */

function xml_Dec2Xml returns char ( pdDec as dec ):

    if pdDec = ? then return "NaN".

    return string( pdDec ).

end function. /* xml_Dec2Xml */




function xml_Xml2Log returns log ( pcXLog as char ):

    case pcXLog:

        when "true" or
        when "yes" or
        when "1" then
        return yes.

        when "false" or 
        when "no" or
        when "0" then
        return no.

        otherwise
        return ?.

    end case. /* pcXLog */

end function. /* xml_Xml2Log */

function xml_Log2Xml returns char ( plLog as log ):

    return string( plLog, "true/false" ).

end function. /* xml_Log2Xml */

