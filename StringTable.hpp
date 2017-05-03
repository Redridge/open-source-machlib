#ifndef __STRINGTABLE_HPP
#define __STRINGTABLE_HPP

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "SymbolTableHeader.hpp"
#include "FileUtils.hpp"

/*string table referenced by the symbol table*/
class StringTable
{
private:
        char *raw;
        uint32_t numberOfStrings;
        std::vector<char *> table;

public:
        StringTable(FILE *file, SymbolTableHeader tableHeader);
        StringTable();

        char *getRaw();
        char *get(uint32_t index);
        uint32_t getNumberOfStrings();

        ~StringTable();


};

#endif
