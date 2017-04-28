#include "Segment.hpp"

uint32_t Segment::getMaxProtection()
{
        return maxProtection;
}

uint32_t Segment::getInitProtection()
{
        return initProtection;
}

uint32_t Segment::getNumberSections()
{
        return numberSections;
}

uint32_t Segment::getFlags()
{
        return flags;
}
