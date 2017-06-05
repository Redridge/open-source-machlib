#include "FileUtils.hpp"

void FileUtils::readUint32(FILE *file, uint32_t *buff)
{
        fread(buff, sizeof(uint32_t), 1, file);
        //TODO - add exeception when reading fails
}

void FileUtils::readUint64(FILE *file, uint64_t *buff)
{
        fread(buff, sizeof(uint64_t), 1, file);
        //TODO - add exeception when reading fails
}

void FileUtils::readUint8(FILE *file, uint8_t *buff)
{
        fread(buff, sizeof(uint8_t), 1, file);
        //TODO - add exeception when reading fails
}

void FileUtils::readUint16(FILE *file, uint16_t *buff)
{
        fread(buff, sizeof(uint16_t), 1, file);
        //TODO - add exeception when reading fails
}
void FileUtils::readBytes(FILE *file, char *buff, int nr)
{
        fread(buff, nr, 1, file);
        //TODO - add exception when reading fails
}
