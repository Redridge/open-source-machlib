#ifndef __FILE_UTILS_HPP
#define __FILE_UTILS_HPP

#include <stdint.h>
#include <stdio.h>
#include <stdexcept>
class FileUtils
{
private:
        static bool isSwap;
        static uint32_t swapUint32(uint32_t value);
        static uint64_t swapUint64(uint64_t value);
        static uint16_t swapUint16(uint16_t value);
        static void swapBytes(char *buff, int size);
public:
        static void readUint32(FILE *file, uint32_t * buff);
        static void readUint64(FILE *file, uint64_t * buff);
        static void readUint8(FILE *file, uint8_t * buff);
        static void readUint16(FILE *file, uint16_t * buff);
        static void readBytes(FILE *file, char * buff, int nr);
        static void setSwap();
};

#endif
