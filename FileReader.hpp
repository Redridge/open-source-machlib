#ifndef __FILEREADER_HPP
#define __FILEREADER_HPP

#include <capstone/capstone.h>
#include "MachO.hpp"
#include <inttypes.h>

#define CPU_ARCH_ABI64                  0x1000000

#define CPU_TYPE_I386                   7
#define CPU_TYPE_X86_64                 (CPU_TYPE_I386 | CPU_ARCH_ABI64)


#define CPU_TYPE_ARM                    12
#define CPU_TYPE_ARM64                 (CPU_TYPE_ARM | CPU_ARCH_ABI64)



struct myComp {
         bool operator()(const char *a, const char *b) const {
                 return std::strcmp(a, b) < 0;
         }
};

class FileReader
{
private:
        MachO *binary;
        FILE *file;
        std::map<uint64_t, char *> functionStartsMap;
        std::map<char *, uint64_t, myComp> functionNamesMap;
        std::vector<uint64_t> functionsOffset;
        cs_arch  capstoneArchOption;
        cs_mode capstoneModeOption;
        csh capstoneHandle;

        /*std::map<uint64_t, SymbolTableEntry *> symbolsMap;
        std::vector<uint64_t> symbolsOffset;*/

        static std::map<uint32_t, cs_arch> capstoneArch;
        static std::map<uint32_t, cs_arch>makeCapstoneArch();

        //void makeSymbolsMap();
        void Disassemble(uint64_t fileOffset, uint64_t size);

        void Disassemblex86( const uint8_t **code,
                uint64_t size, uint64_t startAddress);

        void DisassembleARM( const uint8_t **code,
                uint64_t size, uint64_t startAddress);

        cs_mode getCapstoneMode(uint64_t fileOffset);
        uint64_t getNextSymbolOffset(uint64_t currentOffset);


public:
        FileReader(MachO *binary);
        void Disassemble();
        void Disassemble(char *functionName);

        ~FileReader();
};

#endif
