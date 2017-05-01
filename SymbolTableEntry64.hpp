#ifndef __SYMBOLTABLENTRY64_HPP
#define __SYMBOLTABLENTRY64_HPP

#include "SymbolTableEntry.hpp"

class SymbolTableEntry64: public SymbolTableEntry
{
private:
        uint64_t value;

public:
        SymbolTableEntry64(FILE *file, char *strings);
        virtual uint64_t getValue();
};

#endif
