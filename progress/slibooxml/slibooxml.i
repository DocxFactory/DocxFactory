
/**
 * slibooxml.i -
 *
 * (c) Copyright ABC Alon Blich Consulting Tech Ltd.
 *
 * Contact Information
 * Email: alonblich@gmail.com
 * Phone: +972-54-218-8086
 */



&if defined( xSlibOoXml ) = 0 &then

    {slib/start-slib.i "'slibooxml/slibooxml.p'"}

    {slibooxml/slibooxmlprop.i}

    run initializeConvertPrint (
        input {&ooxml_xOpenOfficeDir}, {&ooxml_xOpenOfficePort},
        input {&ooxml_xOdfConverterExec},
        input {&ooxml_xPortraitTemplate},
        input {&ooxml_xLandscapeTemplate} ).
    
    &global xSlibOoXml defined

&endif /* defined = 0 */
