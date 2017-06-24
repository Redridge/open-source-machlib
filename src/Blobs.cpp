#include "Blobs.hpp"

SuperBlob::SuperBlob()
{
}

SuperBlob::SuperBlob(FILE *file, LinkEditCmd sigCmd)
{
	struct subblob sb;
	uint32_t buf;
	uint32_t offset = sigCmd.getDataOffset();
	uint32_t size = sigCmd.getDataSize();

	fseek(file, offset, SEEK_SET);
	FileUtils::readNetworkUint32(file, &buf);		
	if (buf != CSMAGIC_EMBEDDED_SIGNATURE) {
                throw std::runtime_error("Signature not valid.");
	}

	FileUtils::readNetworkUint32(file, &length);		
	FileUtils::readNetworkUint32(file, &numBlobs);		

	for (unsigned int i = 0; i < numBlobs; i++) {
		FileUtils::readNetworkUint32(file, &sb.type);		
		FileUtils::readNetworkUint32(file, &sb.offset);		
		subblobs.push_back(sb);
	}

}

uint32_t SuperBlob::getLength()
{
	return length;
}

uint32_t SuperBlob::getNumBlobs()
{
	return numBlobs;
}

std::vector<struct subblob> SuperBlob::getSubBlobs()
{
	return subblobs;
}

CodeDirectoryBlob::CodeDirectoryBlob()
{
}

CodeDirectoryBlob::CodeDirectoryBlob(FILE *file, LinkEditCmd sigCmd,
				     SuperBlob sb)
{
	char *buf;
	uint32_t magic;
	uint32_t sbOffset = sigCmd.getDataOffset();
	uint32_t size = sigCmd.getDataSize();
	uint32_t cdbOffset = 0;
	std::vector<struct subblob> sbs = sb.getSubBlobs();

	for (unsigned int i = 0; i < sbs.size(); i++) {
		if (sbs[i].type == CODE_DIRECTORY_BLOB) {
			cdbOffset = sbs[i].offset;
			break;
		}
	}

	fseek(file, sbOffset + cdbOffset, SEEK_SET);
	FileUtils::readNetworkUint32(file, &magic);		
	if (magic != CSMAGIC_DIRECTORY_BLOB) {
                throw std::runtime_error("Code Directory Blob wrong magic");
	}

	/* Read metadata */
	FileUtils::readNetworkUint32(file, &length);
	FileUtils::readNetworkUint32(file, &version);
	FileUtils::readNetworkUint32(file, &flags);
	FileUtils::readNetworkUint32(file, &hashOffset);
	FileUtils::readNetworkUint32(file, &identOffset);
	FileUtils::readNetworkUint32(file, &nSpecialSlots);
	FileUtils::readNetworkUint32(file, &nCodeSlots);
	FileUtils::readNetworkUint32(file, &codeLimit);
	FileUtils::readUint8(file, &hashSize);
	FileUtils::readUint8(file, &hashType);
	FileUtils::readUint8(file, &platform);
	FileUtils::readUint8(file, &pageSize);
	FileUtils::readNetworkUint32(file, &spare2);
	FileUtils::readNetworkUint32(file, &scatterOffset);
	FileUtils::readNetworkUint32(file, &teamOffset);

	/* Read hashes */
	uint32_t hashesStart = sbOffset + cdbOffset + hashOffset - hashSize * nSpecialSlots;
	fseek(file, hashesStart, SEEK_SET);
	buf = (char*) malloc(hashSize * (nSpecialSlots + nCodeSlots));
	for (uint32_t i = 0; i < nSpecialSlots; i++) {
		char *cur_buf = buf + i * hashSize;
		FileUtils::readBytes(file, cur_buf, hashSize);
		hashes.push_back(cur_buf);
	}

	for (uint32_t i = 0; i < nCodeSlots; i++) {
		char *cur_buf = buf + i * hashSize + nSpecialSlots * hashSize;
		FileUtils::readBytes(file, cur_buf, hashSize);
		hashes.push_back(cur_buf);
	}
	
}

uint32_t CodeDirectoryBlob::getLength()
{
	return length;
}

uint32_t CodeDirectoryBlob::getVersion()
{
	return version;
}

uint32_t CodeDirectoryBlob::getFlags()
{
	return flags;
}

uint32_t CodeDirectoryBlob::getHashOffset()
{
	return hashOffset;
}

uint32_t CodeDirectoryBlob::getIdentOffset()
{
	return identOffset;
}

uint32_t CodeDirectoryBlob::getNSpecialSlots()
{
	return nSpecialSlots;
}

uint32_t CodeDirectoryBlob::getNCodeSlots()
{
	return nCodeSlots;
}

uint32_t CodeDirectoryBlob::getCodeLimit()
{
	return codeLimit;
}

uint8_t CodeDirectoryBlob::getHashSize()
{
	return hashSize;
}

uint8_t CodeDirectoryBlob::getHashType()
{
	return hashType;
}

uint8_t CodeDirectoryBlob::getPlatform()
{
	return platform;
}

uint8_t CodeDirectoryBlob::getPageSize()
{
	return pageSize;
}

uint32_t CodeDirectoryBlob::getSpare2()
{
	return spare2;
}

uint32_t CodeDirectoryBlob::getScatterOffset()
{
	return scatterOffset;
}

uint32_t CodeDirectoryBlob::getTeamOffset()
{
	return teamOffset;
}

std::vector<char *>
	CodeDirectoryBlob::getHashes()
{
	return hashes;
}


