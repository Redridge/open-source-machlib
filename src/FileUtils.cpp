#include "FileUtils.hpp"

bool FileUtils::isSwap = false;

void FileUtils::readUint32(FILE *file, uint32_t *buff)
{
        uint32_t rc;

        rc = fread(buff, sizeof(uint32_t), 1, file);
        if (rc != 1)
                throw std::runtime_error("File Read32 Fail");
        if (isSwap)
                *buff = swapUint32(*buff);
}

void FileUtils::readNetworkUint32(FILE *file, uint32_t *buff)
{
        uint32_t rc;
	uint32_t buf;

        rc = fread(&buf, sizeof(uint32_t), 1, file);
        if (rc != 1)
                throw std::runtime_error("File ReadNetwork32 Fail");
	*buff = ntohl(buf);
}


void FileUtils::readUint64(FILE *file, uint64_t *buff)
{
        uint32_t rc;

        rc = fread(buff, sizeof(uint64_t), 1, file);
        if (rc != 1)
                throw std::runtime_error("File Read64 Fail");
         if (isSwap)
                *buff = swapUint64(*buff);
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
        
         if (isSwap)
                *buff = swapUint16(*buff);
}

void FileUtils::readBytes(FILE *file, char *buff, int nr)
{
        uint32_t rc;

        rc = fread(buff, nr, 1, file);
        if (rc != 1)
                throw std::runtime_error("File ReadBytes Fail");
        if (isSwap)
                swapBytes(buff, nr);

}

void FileUtils::setSwap()
{
        isSwap = true;
}

uint32_t FileUtils::swapUint32(uint32_t value)
{
        uint32_t Byte0 = value & 0x000000FF;
        uint32_t Byte1 = value & 0x0000FF00;
        uint32_t Byte2 = value & 0x00FF0000;
        uint32_t Byte3 = value & 0xFF000000;
        return (Byte0 << 24) | (Byte1 << 8) | (Byte2 >> 8) | (Byte3 >> 24);
}

uint64_t FileUtils::swapUint64(uint64_t value)
{
        uint64_t Hi = swapUint32(uint32_t(value));
        uint32_t Lo = swapUint32(uint32_t(value >> 32));
        return (Hi << 32) | Lo;
}

uint16_t FileUtils::swapUint16(uint16_t value)
{
        uint16_t Hi = value << 8;
        uint16_t Lo = value >> 8;
        return Hi | Lo;
}

void FileUtils::swapBytes(char *buff, int size)
{
        uint32_t index;
        uint8_t temp;

        for (index = 0; index < size / 2; index++) {
                temp = buff[index];
                buff[index] = buff[size - index - 1];
                buff[size = index - 1] = temp;
        }
}
