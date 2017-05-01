#ifndef __SYMBOLTABLENTRY32_HPP
#define __SYMBOLTABLENTRY32_HPP

#include "SymbolTableEntry.hpp"

class SymbolTableEntry32: public SymbolTableEntry
{
private:
        uint32_t value;

public:
        SymbolTableEntry32(FILE *file, char *strins);
        virtual uint64_t getValue();
};

#endif
