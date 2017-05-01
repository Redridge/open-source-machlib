#include "SymbolTableEntry64.hpp"


SymbolTableEntry64::SymbolTableEntry64(FILE *file, char *strings) : SymbolTableEntry(file, strings)
{
        FileUtils::readUint64(file, &value);
}

uint64_t SymbolTableEntry64::getValue()
{
        return value;
}
