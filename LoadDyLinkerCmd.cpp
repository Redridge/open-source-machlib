#include "LoadDyLinkerCmd.hpp"

LoadDyLinkerCmd::LoadDyLinkerCmd(FILE *file)
{
        uint32_t cmdSize;
        uint64_t cmdOffset;

        cmdOffset = ftell(file) - sizeof(uint32_t);

        FileUtils::readUint32(file, &cmdSize);

        FileUtils::readUint32(file, &nameOffset);

        fseek(file, cmdOffset + nameOffset, SEEK_SET);
        linkerName = new char[cmdSize - nameOffset];
        FileUtils::readBytes(file, linkerName, cmdSize - nameOffset);

}

uint32_t LoadDyLinkerCmd::getNameOffset()
{
        return nameOffset;
}

char * LoadDyLinkerCmd::getLinkerName()
{
        return linkerName;
}

LoadDyLinkerCmd::~LoadDyLinkerCmd()
{
        delete linkerName;
}
