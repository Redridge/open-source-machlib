#ifndef __SEGEMENT_HPP
#define __SEGEMENT_HPP

#include <stdint.h>
#include <vector>
#include "Section.hpp"

class Segment
{

protected:
        char name[16];
        uint32_t maxProtection;
        uint32_t initProtection;
        uint32_t numberSections;
        uint32_t flags;
        std::vector<Section *> sections;

public:
        char *getName();
        virtual uint64_t getVirtualAddress() = 0;
        virtual uint64_t getVirtualSize() = 0;
        virtual uint64_t getFileOffset() = 0;
        virtual uint64_t getFileSize() = 0;
        uint32_t getMaxProtection();
        uint32_t getInitProtection();
        uint32_t getNumberSections();
        uint32_t getFlags();
        std::vector<Section *> getSections();
};

#endif
