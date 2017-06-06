#include "MachHeader.hpp"
#include "FileUtils.hpp"

MachHeader::MachHeader(FILE *file)
{
        FileUtils::readUint32(file, &magic);

        if(magic != MAGIC_32 && magic != MAGIC_64) {
                throw std::runtime_error("File probably not MachO");
        }

        if(magic == MAGIC_32)
                is_32 = true;
        else
                is_32 = false;

        FileUtils::readUint32(file, &cpuType);
        FileUtils::readUint32(file, &cpuSubType);
        FileUtils::readUint32(file, &fileType);
        FileUtils::readUint32(file, &numberCmds);
        FileUtils::readUint32(file, &sizeOfCmds);
        FileUtils::readUint32(file, &flags);

        if(!is_32)
                FileUtils::readUint32(file, &reserved);

}

MachHeader::MachHeader()
{

}

bool MachHeader::getIs32()
{
        return is_32;
}

uint32_t MachHeader::getMagic()
{
        return magic;
}

uint32_t MachHeader::getCpuType()
{
        return cpuType;
}

uint32_t MachHeader::getCpuSubType()
{
        return cpuSubType;
}

uint32_t MachHeader::getFileType()
{
        return fileType;
}

uint32_t MachHeader::getNumberCmds()
{
        return numberCmds;
}

uint32_t MachHeader::getSizeOfCmds()
{
        return sizeOfCmds;
}

uint32_t MachHeader::getFlags()
{
        return flags;
}

uint32_t MachHeader::getReserved()
{
        if(!is_32)
                return reserved;

        //TODO - throws exception when not 32 bit
        return -1;
}
