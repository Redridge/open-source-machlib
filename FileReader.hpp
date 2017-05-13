#ifndef __FILEREADER_HPP
#define __FILEREADER_HPP

#include <capstone/capstone.h>
#include "MachO.hpp"
#include <inttypes.h>

class FileReader
{
private:
        MachO *binary;
        FILE *file;
        std::map<uint64_t, char *> functionStartsMap;

public:
        FileReader(MachO *binary);
        void Disassemble(uint64_t fileOffset, uint64_t size);
        void Disassemble(char *functionName);
};

#endif
