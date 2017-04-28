#ifndef __FILE_UTILS_HPP
#define __FILE_UTILS_HPP

#include <stdint.h>
#include <stdio.h>

class FileUtils
{
public:
        static void readUint32(FILE *file, uint32_t * buff);
        static void readUint64(FILE *file, uint64_t * buff);
        static void readBytes(FILE *file, char * buff, int nr);
};

#endif
