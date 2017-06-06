#ifndef __LOADSIMPLECOMMANDS_HPP
#define __LOADSIMPLECOMMANDS_HPP

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

/*It is used for main executables to specify the location (file offset)
 * of main().  If -stack_size was used at link time, the stacksize
 * field will contain the stack size need for the main thread.
 */
class LoadMainCmd {

private:
        uint64_t entryOffset;
        uint64_t stackSize;

public:
        LoadMainCmd(FILE *file);
        LoadMainCmd();
        uint64_t getEntryOffset();
        uint64_t getStackSize();
};


/*It's used by tools that need to symbolicate addresses in crash logs, 
samples, spindumps, etc. to determine
if a given address falls inside a function*/
class LinkEditCmd {

private:
        uint32_t dataOffset;
        uint32_t dataSize;

public:
        LinkEditCmd(FILE *file);
        LinkEditCmd();
        uint32_t getDataOffset();
        uint32_t getDataSize();

};

#endif
