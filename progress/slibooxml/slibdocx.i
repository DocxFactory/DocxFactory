
/**
 * slibdocx.i -
 *
 * (c) Copyright DocxFactory Ltd.
 *
 * docxfactory.com
 * alonb@docxfactory.com
 */



&if defined( xSLibDocx ) = 0 &then

    {slib/start-slib.i "'slibooxml/slibdocx.p'"}
    
    {slibooxml/slibdocxfrwd.i "in super"}

    {slibooxml/slibdocxprop.i}
    
    &global xSLibDocx defined

&endif /* defined = 0 */
