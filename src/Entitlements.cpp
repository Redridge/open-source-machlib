#include "Entitlements.hpp"

Entitlements::Entitlements()
{
}

Entitlements::Entitlements(std::string xml_str)
{
	xml = xml_str;
}

std::string Entitlements::getXml()
{
	return xml;
}

