#include "SymbolTableEntry32.hpp"


SymbolTableEntry32::SymbolTableEntry32(FILE *file) : SymbolTableEntry(file)
{
        FileUtils::readUint32(file, &value);
}

uint64_t SymbolTableEntry32::getValue()
{
        return value;
}
