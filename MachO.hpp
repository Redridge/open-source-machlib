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
#include "SymbolTableHeader.hpp"
#include "StringTable.hpp"
#include "SymbolTableEntry.hpp"
#include "SymbolTableEntry32.hpp"
#include "SymbolTableEntry64.hpp"
#include "LoadDyLinkerCmd.hpp"

#define LC_SEGMENT32            0x01
#define LC_SEGMENT64            0x19
#define LC_SYMTAB               0x02
#define LC_LOAD_DYLINKER        0x0E

#define LC_UUID                 0x1B
#define UUID_SIZE               16

/*high level class*/
/*entry point of the library*/
class MachO
{
private:
        FILE *file;
        MachHeader header;
        std::vector<Segment *> segments;
        SymbolTableHeader symbolTableHeader;
        /*flags to avoid recomputing*/
        bool stringTableComputed;
        bool symbolTableComputed;
        /*the string table*/
        StringTable *stringTable;
        /*entries in the symbol table*/
        std::vector<SymbolTableEntry *> symbolTable;
        /*dinmaic linker load command*/
        LoadDyLinkerCmd *loadDyLinkerCmd;

        /*the uuid is a128-bit unique random number that*/
        /*identifies an object produced by the static link editor*/
        uint8_t uuid[UUID_SIZE];

public:
        MachO(char  *fileName);
        MachHeader getHeader();
        std::vector<Segment *> getSegments();
        SymbolTableHeader getSymbolTableHeader();
        StringTable *getStringTable();
        std::vector<SymbolTableEntry *> getSymbolTable();
        LoadDyLinkerCmd *getLoadDyLinkerCmd();
        uint8_t *getUUID();


        ~MachO();
};


#endif
