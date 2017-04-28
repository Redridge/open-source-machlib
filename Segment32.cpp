#include "FileUtils.hpp"
#include "Segment32.hpp"

Segment32::Segment32(FILE *file)
{
        uint32_t cmdSize, dummy;

        FileUtils::readUint32(file, &cmdSize);

        FileUtils::readBytes(file, name, 16);

        FileUtils::readUint32(file, &virtualAddress);

        FileUtils::readUint32(file, &virtualSize);

        FileUtils::readUint32(file, &fileOffset);

        FileUtils::readUint32(file, &fileSize);

        FileUtils::readUint32(file, &maxProtection);

        FileUtils::readUint32(file, &initProtection);

        FileUtils::readUint32(file, &numberSections);

        FileUtils::readUint32(file, &flags);


}

uint64_t Segment32::getVirtualAddress()
{
        return virtualAddress;
}

uint64_t Segment32::getVirtualSize()
{
        printf("32 size\n");
        return virtualSize;
}

uint64_t Segment32::getFileOffset()
{
        return fileOffset;
}

uint64_t Segment32::getFileSize()
{
        return fileSize;
}
