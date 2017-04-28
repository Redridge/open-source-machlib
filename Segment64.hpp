#ifndef __SEGEMENT64_HPP
#define __SEGEMENT64_HPP

#include "Segment.hpp"
#include <stdint.h>

/*class for the load segment command for 64 bit architecture*/
class Segment64: public Segment
{
/*specific fields*/
private:
        uint64_t virtualAddress;
        uint64_t virtualSize;
        uint64_t fileOffset;
        uint64_t fileSize;

public:
        Segment64(FILE *f);
        virtual uint64_t getVirtualAddress();
        virtual uint64_t getVirtualSize();
        virtual uint64_t getFileOffset();
        virtual uint64_t getFileSize();
};

#endif
