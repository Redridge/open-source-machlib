#include "FileUtils.hpp"

void FileUtils::readUint32(FILE *file, uint32_t *buff)
{
        uint32_t rc;

        rc = fread(buff, sizeof(uint32_t), 1, file);
        if (rc != 1)
                throw std::runtime_error("File Read32 Fail");
}

void FileUtils::readUint64(FILE *file, uint64_t *buff)
{
        uint32_t rc;

        rc = fread(buff, sizeof(uint64_t), 1, file);
        if (rc != 1)
                throw std::runtime_error("File Read64 Fail");
}

void FileUtils::readUint8(FILE *file, uint8_t *buff)
{
        uint32_t rc;

        rc = fread(buff, sizeof(uint8_t), 1, file);
        if (rc != 1)
                throw std::runtime_error("File Read8 Fail");
}

void FileUtils::readUint16(FILE *file, uint16_t *buff)
{
        uint32_t rc;

        rc = fread(buff, sizeof(uint16_t), 1, file);
        if (rc != 1)
                throw std::runtime_error("File Read16 Fail");
}
void FileUtils::readBytes(FILE *file, char *buff, int nr)
{
        uint32_t rc;

        rc = fread(buff, nr, 1, file);
        if (rc != 1)
                throw std::runtime_error("File ReadBytes Fail");

}
