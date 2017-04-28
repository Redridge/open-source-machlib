#include "FileUtils.hpp"
#include "Segment64.hpp"
#include "Section64.hpp"

Segment64::Segment64(FILE *file)
{
        uint32_t cmdSize, index;

        FileUtils::readUint32(file, &cmdSize);

        FileUtils::readBytes(file, name, 16);

        FileUtils::readUint64(file, &virtualAddress);

        FileUtils::readUint64(file, &virtualSize);

        FileUtils::readUint64(file, &fileOffset);

        FileUtils::readUint64(file, &fileSize);

        FileUtils::readUint32(file, &maxProtection);

        FileUtils::readUint32(file, &initProtection);

        FileUtils::readUint32(file, &numberSections);

        FileUtils::readUint32(file, &flags);

        for(index = 0; index < numberSections; index++) {
                sections.push_back(new Section64(file));
        }

}

uint64_t Segment64::getVirtualAddress()
{
        return virtualAddress;
}

uint64_t Segment64::getVirtualSize()
{
        return virtualSize;
}

uint64_t Segment64::getFileOffset()
{
        return fileOffset;
}

uint64_t Segment64::getFileSize()
{
        return fileSize;
}
