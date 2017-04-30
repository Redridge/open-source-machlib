#ifndef __MACHO_HPP
#define __MACHO_HPP

#include <vector>
#include <stdio.h>
#include "FileUtils.hpp"
#include "MachHeader.hpp"
#include "Section.hpp"
#include "Segment.hpp"
#include "Section32.hpp"
#include "Segment32.hpp"
#include "Section64.hpp"
#include "Segment64.hpp"

#define LC_SEGMENT32    0x01
#define LC_SEGMENT64    0x19

/*high level class*/
/*entry point of the library*/
class MachO
{
private:
        MachHeader header;
        std::vector<Segment *> segments;

public:
        MachO(char  *fileName);
        MachHeader getHeader();
        std::vector<Segment *> getSegments();

        ~MachO();
};


#endif
