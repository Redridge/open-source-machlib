#ifndef __LOADDYLINKERCMD_HPP
#define __LOADDYLINKERCMD_HPP

#include <stdint.h>
#include <stdio.h>
#include "FileUtils.hpp"

class LoadDyLinkerCmd
{
private:
        uint32_t nameOffset;
        char * linkerName;

public:
        LoadDyLinkerCmd(FILE *file);
        uint32_t getNameOffset();
        char *getLinkerName();

        ~LoadDyLinkerCmd();

};
#endif
