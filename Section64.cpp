#include "Section64.hpp"
#include "FileUtils.hpp"

Section64::Section64(FILE *file)
{
        FileUtils::readBytes(file, sectionName, 16);

        FileUtils::readBytes(file, segmentName, 16);

        FileUtils::readUint32(file, &virtualAddress);

        FileUtils::readUint32(file, &size);

        FileUtils::readUint32(file, &offset);

        FileUtils::readUint32(file, &align);

        FileUtils::readUint32(file, &relocationsOffset);

        FileUtils::readUint32(file, &numberRelocations);

        FileUtils::readUint32(file, &flags);

        FileUtils::readUint32(file, &reserved1);

        FileUtils::readUint32(file, &reserved2);
}

uint64_t Section64::getVirtualAddress()
{
        return virtualAddress;
}

uint64_t Section64::getSize()
{
        return size;
}

uint32_t Section64::getReserved3()
{
        return reserved3;
}
