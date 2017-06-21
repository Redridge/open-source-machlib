#ifndef __ENTITLEMENTS_HPP
#define __ENTITLEMENTS_HPP

#include <stdint.h>
#include <vector>
#include <string>

#include "FileUtils.hpp"

#define ENTITLEMENTS_VALUE	0x5

class Entitlements
{
private:
	std::string xml;
public:
	Entitlements();
	Entitlements(std::string xml_str);
	std::string getXml();

};

#endif
