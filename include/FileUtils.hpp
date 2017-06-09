#ifndef __FILE_UTILS_HPP
#define __FILE_UTILS_HPP

#include <stdint.h>
#include <stdio.h>
#include <stdexcept>
#include <arpa/inet.h>
class FileUtils
{
public:
        static void readUint32(FILE *file, uint32_t * buff);
        static void readNetworkUint32(FILE *file, uint32_t * buff);
        static void readUint64(FILE *file, uint64_t * buff);
        static void readUint8(FILE *file, uint8_t * buff);
        static void readUint16(FILE *file, uint16_t * buff);
        static void readBytes(FILE *file, char * buff, int nr);
};

#endif
