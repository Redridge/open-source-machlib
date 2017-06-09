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
	lcSigCmd = sigCmd;	

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
