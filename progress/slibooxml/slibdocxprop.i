
/**
 * slibdocxprop.i -
 *
 * (c) Copyright DocxFactory Ltd.
 *
 * docxfactory.com
 * alonb@docxfactory.com
 */

 
 
&if "{&opsys}" begins "win" &then

&global docx_xDocxFactoryLib    'DocxFactory.dll'

&else

&global docx_xDocxFactoryLib    'libDocxFactory.so'

&endif
