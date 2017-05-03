#include "SymbolTableHeader.hpp"

SymbolTableHeader::SymbolTableHeader(FILE *file)
{
        uint32_t cmdSize;

        FileUtils::readUint32(file, &cmdSize);

        FileUtils::readUint32(file, &tableOffset);

        FileUtils::readUint32(file, &numberSymbols);

        FileUtils::readUint32(file, &stringTableOffset);

        FileUtils::readUint32(file, &stringTableSize);
}

SymbolTableHeader::SymbolTableHeader()
{

}
uint32_t SymbolTableHeader::getTableOffset()
{
        return tableOffset;
}

uint32_t SymbolTableHeader::getNumberofSymbols()
{
        return numberSymbols;
}

uint32_t SymbolTableHeader::getStringTableOffset()
{
        return stringTableOffset;
}

uint32_t SymbolTableHeader::getStringTableSize()
{
        return stringTableSize;
}
