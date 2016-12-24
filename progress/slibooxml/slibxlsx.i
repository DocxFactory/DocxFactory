
/**
 * slibxlsx.i -
 *
 * (c) Copyright ABC Alon Blich Consulting Tech Ltd.
 *
 * Contact Information
 * Email: alonblich@gmail.com
 * Phone: +972-54-218-8086
 */



&if defined( xSLibXlsx ) = 0 &then

    {slib/start-slib.i "'slibooxml/slibxlsx.p'"}
    
    {slibooxml/slibxlsxfrwd.i "in super"}

    {slibooxml/slibxlsxprop.i}

    &global xSLibXlsx defined

&endif /* defined = 0 */
