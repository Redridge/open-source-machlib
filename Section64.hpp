#ifndef __SECTION_64_HPP
#define __SECTION_64_HPP

#include <stdio.h>
#include "Section.hpp"


class Section64: public Section
{

private:
        uint64_t virtualAddress;
        uint64_t size;
        uint32_t reserved3;

public:
        Section64(FILE *file);
        virtual uint64_t getVirtualAddress();
        virtual uint64_t getSize();
        virtual uint32_t getReserved3();
};

#endif
