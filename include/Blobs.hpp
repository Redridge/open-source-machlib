#ifndef __BLOBS_HPP
#define __BLOBS_HPP

#include <stdint.h>
#include <vector>

#include "FileUtils.hpp"
#include "SimpleLoadCommands.hpp"

#define CSMAGIC_EMBEDDED_SIGNATURE 	0xFADE0CC0

struct subblob {
	uint32_t type;
	uint32_t offset;
};

class SuperBlob
{
private:
	uint32_t length;
	uint32_t numBlobs;
	std::vector<struct subblob> subblobs;
public:
	SuperBlob(FILE *file, LinkEditCmd sigCmd);
	SuperBlob();
	uint32_t getLength();
	uint32_t getNumBlobs();
	std::vector<struct subblob> getSubBlobs();
	

};

#endif
