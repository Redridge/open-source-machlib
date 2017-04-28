#ifndef __SECTION_32_HPP
#define __SECTION_32_HPP

#include <stdio.h>
#include "Section.hpp"


class Section32: public Section
{

private:
        uint32_t virtualAddress;
        uint32_t size;

public:
        Section32(FILE *file);
        virtual uint64_t getVirtualAddress();
        virtual uint64_t getSize();
        virtual uint32_t getReserved3();
};

#endif
