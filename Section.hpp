#ifndef __SECTION_HPP
#define __SECTION_HPP

#include <stdint.h>
/*generic class for Sections which the user will use*/
/*this class is needed to abstract the architecture 32 bit 64 bit*/
class Section
{

/*common fields*/
protected:
        char sectionName[16];
        char segmentName[16];
        uint32_t offset;
        uint32_t align;
        uint32_t relocationsOffset;
        uint32_t numberRelocations;
        uint32_t flags;
        uint32_t reserved1;
        uint32_t reserved2;

/*unified interface for both 32 bit and 64 bit*/
public:
        char *getSectionName();
        char *getSegmentName();
        virtual uint64_t getVirtualAddress() = 0;
        virtual uint64_t getSize() = 0;
        uint32_t getOffset();
        uint32_t getAlign();
        uint32_t getRelocationOffset();
        uint32_t getNumberRelocations();
        uint32_t getFlags();
        uint32_t getReserved1();
        uint32_t getReserved2();
        virtual uint32_t getReserved3() = 0;

        virtual ~Section();
};

#endif
