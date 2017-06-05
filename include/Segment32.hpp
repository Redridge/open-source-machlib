#ifndef __SEGEMENT32_HPP
#define __SEGEMENT32_HPP

#include "Segment.hpp"
#include <stdint.h>

/*class for the load segment command for 32 bit architecture*/
class Segment32: public Segment
{
/*specific fields*/
private:
        uint32_t virtualAddress;
        uint32_t virtualSize;
        uint32_t fileOffset;
        uint32_t fileSize;

public:
        Segment32(FILE *f);
        virtual uint64_t getVirtualAddress();
        virtual uint64_t getVirtualSize();
        virtual uint64_t getFileOffset();
        virtual uint64_t getFileSize();


};
#endif
