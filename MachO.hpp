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

#define LC_SEGMENT32    0x01
#define LC_SEGMENT64    0x19
#define LC_SYMTAB       0x02

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

public:
        MachO(char  *fileName);
        MachHeader getHeader();
        std::vector<Segment *> getSegments();
        SymbolTableHeader getSymbolTableHeader();
        StringTable *getStringTable();
        std::vector<SymbolTableEntry *> getSymbolTable();

        ~MachO();
};


#endif
