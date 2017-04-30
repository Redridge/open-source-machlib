#include "SymbolTableEntry.hpp"

SymbolTableEntry::SymbolTableEntry(FILE *file)
{
                FileUtils::readUint32(file, &stringTableIndex);

                FileUtils::readUint8(file, &type);

                FileUtils::readUint8(file, &sectionIndex);

                FileUtils::readUint16(file, &description);

}

SymbolTableEntry::SymbolTableEntry()
{

}

uint32_t SymbolTableEntry::getStringTableIndex()
{
        return stringTableIndex;
}

uint8_t SymbolTableEntry::getType()
{
        return type;
}

uint8_t SymbolTableEntry::getSectionIndex()
{
        return sectionIndex;
}

uint16_t SymbolTableEntry::getDescription()
{
        return description;
}
