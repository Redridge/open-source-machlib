#include <stdio.h>
#include <stdint.h>
#include "Section.hpp"

char * Section::getSectionName()
{
        return sectionName;
}

char * Section::getSegmentName()
{
        return segmentName;
}

uint32_t Section::getOffset()
{
        return offset;
}

uint32_t Section::getAlign()
{
        return align;
}

uint32_t Section::getRelocationOffset()
{
        return relocationsOffset;
}

uint32_t Section::getNumberRelocations()
{
        return numberRelocations;
}
uint32_t Section::getFlags()
{
        return flags;
}

uint32_t Section::getReserved1()
{
        return reserved1;
}

uint32_t Section::getReserved2()
{
        return reserved2;
}

Section::~Section()
{
}
