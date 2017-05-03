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



#endif
