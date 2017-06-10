#ifndef __MACHHEADER_HPP
#define __MACHHEADER_HPP

#include <stdint.h>
#include <stdio.h>
#include <stdexcept>

#define MAGIC_32 0xFEEDFACE
#define MAGIC_64 0xFEEDFACF
#define CIGAM_32 0xCEFAEDFE
#define CIGAM_64 0xCFFAEDFE  

class MachHeader
{
private:
        bool is_32;
        uint32_t magic;
        uint32_t cpuType;
        uint32_t cpuSubType;
        uint32_t fileType;
        uint32_t numberCmds;
        uint32_t sizeOfCmds;
        uint32_t flags;
        uint32_t reserved;

public:
        MachHeader(FILE *file);
        MachHeader();
        /****getter****/
        bool getIs32();
        uint32_t getMagic();
        uint32_t getCpuType();
        uint32_t getCpuSubType();
        uint32_t getFileType();
        uint32_t getNumberCmds();
        uint32_t getSizeOfCmds();
        uint32_t getFlags();
        uint32_t getReserved();

};

#endif
