#ifndef __SYMBOLTABLEHEADER_HPP
#define __SYMBOLTABLEHEADER_HPP

#include <stdint.h>
#include <stdio.h>
#include "FileUtils.hpp"

class SymbolTableHeader
{
private:
        uint32_t tableOffset;
        uint32_t numberSymbols;
        uint32_t stringTableOffset;
        uint32_t stringTableSize;

public:
        SymbolTableHeader(FILE *file);
        SymbolTableHeader();
        uint32_t getTableOffset();
        uint32_t getNumberofSymbols();
        uint32_t getStringTableOffset();
        uint32_t getStringTableSize();
};

#endif
