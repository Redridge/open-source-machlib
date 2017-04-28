#include "FileUtils.hpp"
#include "Segment32.hpp"
#include "Section32.hpp"

Segment32::Segment32(FILE *file)
{
        uint32_t cmdSize, index;

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

        for(index = 0; index < numberSections; index++) {
                sections.push_back(new Section32(file));
        }


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
