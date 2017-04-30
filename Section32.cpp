#include "Section32.hpp"
#include "FileUtils.hpp"

Section32::Section32(FILE *file)
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

uint64_t Section32::getVirtualAddress()
{
        return virtualAddress;
}

uint64_t Section32::getSize()
{
        return size;
}

uint32_t Section32::getReserved3()
{
        //TODO throw exception
        return 0;
}
