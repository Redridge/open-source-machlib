#include "SymbolTableEntry.hpp"

SymbolTableEntry::SymbolTableEntry(FILE *file, char *strings)
{
                FileUtils::readUint32(file, &stringTableIndex);

                FileUtils::readUint8(file, &type);

                FileUtils::readUint8(file, &sectionIndex);

                FileUtils::readUint16(file, &description);

                name = new char[strlen(&strings[stringTableIndex]) + 1];

                strcpy(name, &strings[stringTableIndex]);

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

char *SymbolTableEntry::getName()
{
        return name;
}

SymbolTableEntry::~SymbolTableEntry()
{
        printf("entry dessctructor");
        delete name;
}
