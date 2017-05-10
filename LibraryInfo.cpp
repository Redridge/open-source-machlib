#include "LibraryInfo.hpp"

LibraryInfo::LibraryInfo(FILE *file)
{
        uint32_t cmdSize, nameOffset;
        uint64_t cmdOffset;

        cmdOffset = ftell(file) - sizeof(uint32_t);

        FileUtils::readUint32(file, &cmdSize);

        FileUtils::readUint32(file, &nameOffset);

        FileUtils::readUint32(file, &timestamp);

        FileUtils::readUint32(file, &currentVersion);

        FileUtils::readUint32(file, &compatibilityVersion);

        fseek(file, cmdOffset + nameOffset, SEEK_SET);
        name = new char[cmdSize - nameOffset];
        FileUtils::readBytes(file, name, cmdSize - nameOffset);

}

uint32_t LibraryInfo::getTimestamp()
{
        return timestamp;
}

uint32_t LibraryInfo::getCurrentVersion()
{
        return currentVersion;
}

uint32_t LibraryInfo::getCompatibilityVersion()
{
        return compatibilityVersion;
}

char *LibraryInfo::getName()
{
        return name;
}

LibraryInfo::~LibraryInfo()
{
        delete name;
}
