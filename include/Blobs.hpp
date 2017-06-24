#ifndef __BLOBS_HPP
#define __BLOBS_HPP

#include <stdint.h>
#include <stdlib.h>
#include <vector>

#include "FileUtils.hpp"
#include "SimpleLoadCommands.hpp"

#define CSMAGIC_EMBEDDED_SIGNATURE 	0xFADE0CC0
#define CSMAGIC_DIRECTORY_BLOB		0xFADE0C02

#define CODE_DIRECTORY_BLOB		0x0

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

class CodeDirectoryBlob
{
private:
	uint32_t length;
	uint32_t version;
	uint32_t flags;
	uint32_t hashOffset;
	uint32_t identOffset;
	uint32_t nSpecialSlots;
	uint32_t nCodeSlots;
	uint32_t codeLimit;
	uint8_t hashSize;
	uint8_t hashType;
	uint8_t platform;
	uint8_t pageSize;
	uint32_t spare2;
	uint32_t scatterOffset;
	uint32_t teamOffset;
	std::vector<char *> hashes;
public:
	CodeDirectoryBlob(FILE *file, LinkEditCmd sigCmd, SuperBlob sb);
	CodeDirectoryBlob();
	uint32_t getLength();
	uint32_t getVersion();
	uint32_t getFlags();
	uint32_t getHashOffset();
	uint32_t getIdentOffset();
	uint32_t getNSpecialSlots();
	uint32_t getNCodeSlots();
	uint32_t getCodeLimit();
	uint8_t getHashSize();
	uint8_t getHashType();
	uint8_t getPlatform();
	uint8_t getPageSize();
	uint32_t getSpare2();
	uint32_t getScatterOffset();
	uint32_t getTeamOffset();
	std::vector<char *> getHashes();

};


#endif
