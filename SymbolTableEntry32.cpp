#include "SymbolTableEntry32.hpp"


SymbolTableEntry32::SymbolTableEntry32(FILE *file, char *strings) : SymbolTableEntry(file, strings)
{
        FileUtils::readUint32(file, &value);
}

uint64_t SymbolTableEntry32::getValue()
{
        return value;
}
