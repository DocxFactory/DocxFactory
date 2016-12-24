
#ifndef __DOCXFACTORY_XML_INITIALIZER_H__
#define __DOCXFACTORY_XML_INITIALIZER_H__

namespace DocxFactory
{
	class XmlInitializer
	{
	public:
		static XmlInitializer& getInstance();

		virtual ~XmlInitializer();

	protected:

	private:
		XmlInitializer();
		XmlInitializer( const XmlInitializer& p_other );
		XmlInitializer operator = ( const XmlInitializer& p_other );

	};
};

#endif
