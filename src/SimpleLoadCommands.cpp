#include "SimpleLoadCommands.hpp"

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


LoadMainCmd::LoadMainCmd(FILE *file)
{
        uint32_t cmdSize;

        FileUtils::readUint32(file, &cmdSize);

        FileUtils::readUint64(file, &entryOffset);

        FileUtils::readUint64(file, &stackSize);
}

LoadMainCmd::LoadMainCmd()
{

}

uint64_t LoadMainCmd::getEntryOffset()
{
        return entryOffset;
}

uint64_t LoadMainCmd::getStackSize()
{
        return stackSize;
}


LinkEditCmd::LinkEditCmd()
{
        dataOffset = 0;
        dataSize = 0;
}

LinkEditCmd::LinkEditCmd(FILE *file)
{
        uint32_t cmdSize;

        FileUtils::readUint32(file, &cmdSize);

        FileUtils::readUint32(file, &dataOffset);

        FileUtils::readUint32(file, &dataSize);
}

uint32_t LinkEditCmd::getDataOffset()
{
        return dataOffset;
}

uint32_t LinkEditCmd::getDataSize()
{
        return dataSize;
}
