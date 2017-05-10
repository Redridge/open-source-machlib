#ifndef __LIBRARYINFO_HPP
#define __LIBRARYINFO_HPP

#include <stdint.h>
#include <stdio.h>
#include "FileUtils.hpp"

/*Dynamicly linked shared libraries are identified by two things.  The
* pathname (the name of the library as found for execution), and the
* compatibility version number.  The pathname must match and the compatibility
* number in the user of the library must be greater than or equal to the
* library being used.  The time stamp is used to record the time a library was
* built and copied into user so it can be use to determined if the library used
* at runtime is exactly the same as used to built the program.
*/

class LibraryInfo
{
private:
        uint32_t timestamp;
        uint32_t currentVersion;
        uint32_t compatibilityVersion;
        char *name;

public:

        LibraryInfo(FILE * file);
        uint32_t getTimestamp();
        uint32_t getCurrentVersion();
        uint32_t getCompatibilityVersion();
        char * getName();

        ~LibraryInfo();

};

#endif
