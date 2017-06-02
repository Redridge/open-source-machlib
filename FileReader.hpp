#ifndef __FILEREADER_HPP
#define __FILEREADER_HPP

#include <capstone/capstone.h>
#include "MachO.hpp"
#include <inttypes.h>
#include <stdexcept>

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
        /*maps for functions offset and names*/
        std::map<uint64_t, char *> functionStartsMap;
        std::map<char *, uint64_t, myComp> functionNamesMap;
        std::vector<uint64_t> functionsOffset;
        /*capstone variables*/
        cs_arch  capstoneArchOption;
        cs_mode capstoneModeOption;
        csh capstoneHandle;

        static std::map<uint32_t, cs_arch> capstoneArch;
        static std::map<uint32_t, cs_arch>makeCapstoneArch();

        /*helper functions*/
        void DisassembleAll(uint64_t fileOffset, uint64_t size);

        void Disassemble(uint64_t fileOffset, uint64_t codeSize);

        void Disassemblex86(const uint8_t **code,
                uint64_t size, uint64_t startAddress, bool print);

        void DisassembleARM(const uint8_t **code,
                uint64_t size, uint64_t startAddress);

        bool getFunctionBoundaries(uint64_t offset, uint64_t *begin,
                uint64_t *end);

        cs_mode getCapstoneMode(uint64_t fileOffset);
        uint64_t getNextOffset(uint64_t currentOffset);


public:
        FileReader(MachO *binary);
        void Disassemble();
        void Disassemble(char *functionName);
        void Disassemble(uint64_t fileOffset);
        char * dumpSection(char * segmentName, char *sectionName, uint64_t *size);

        ~FileReader();
};

#endif
