
/**
 * slibooxmlprop.i -
 *
 * (c) Copyright ABC Alon Blich Consulting Tech Ltd.
 *
 * Contact Information
 * Email: alonblich@gmail.com
 * Phone: +972-54-218-8086
 */



&if "{&opsys}" begins "win" &then

&global ooxml_xOpenOfficeDir        'C:~\Program Files (x86)~\OpenOffice.org 3'

&else

&global ooxml_xOpenOfficeDir        '/opt/OpenOffice.org 3'

&endif

&global ooxml_xOpenOfficePort       '8100'



&if "{&opsys}" begins "win" &then

&global ooxml_xOdfConverterExec     'C:~\Program Files (x86)~\OpenXML-ODF Translator~\OdfConverter.exe'

&else

&global ooxml_xOdfConverterExec     '/usr/bin/odf-converter-integrator'

&endif



&global ooxml_xPortraitTemplate     'slibooxml/portrait.docx'
&global ooxml_xLandscapeTemplate    'slibooxml/landscape.docx'



define temp-table ooxml_ttOption no-undo

    field cOption   as char label "Option"  format "x(32)"
    field cValue    as char label "Value"   format "x(32)"

    index cOption is primary unique
          cOption.
