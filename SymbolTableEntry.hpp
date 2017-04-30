#ifndef __SYMBOLTABLENTRY_HPP
#define __SYMBOLTABLENTRY_HPP

#include <stdint.h>
#include <stdio.h>
#include "FileUtils.hpp"

class SymbolTableEntry
{
protected:
        uint32_t stringTableIndex;
        uint8_t type;
        uint8_t sectionIndex;
        uint16_t description;

public:
        SymbolTableEntry(FILE *file);
        SymbolTableEntry();
        uint32_t getStringTableIndex();
        uint8_t getType();
        uint8_t getSectionIndex();
        uint16_t getDescription();
        virtual uint64_t getValue() = 0;

};


#endif
