
#include "DocxFactory/xml/XmlInitializer.h"

#include "xercesc/util/PlatformUtils.hpp"

using namespace DocxFactory;



XmlInitializer::XmlInitializer()
{
	xercesc::XMLPlatformUtils::Initialize();
} // c'tor

XmlInitializer::~XmlInitializer()
{
	xercesc::XMLPlatformUtils::Terminate();
} // d'tor

XmlInitializer& XmlInitializer::getInstance()
{
	static XmlInitializer s_instance;
	return s_instance;
} // getInstance
