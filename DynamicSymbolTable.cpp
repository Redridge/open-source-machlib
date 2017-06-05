#include "DynamicSymbolTable.hpp"

DynamicSymbolTableHeader::DynamicSymbolTableHeader(FILE *file)
{
        uint32_t cmdSize;

        FileUtils::readUint32(file, &cmdSize);

        FileUtils::readUint32(file, &indexLocalSym);
        FileUtils::readUint32(file, &numberLocalSym);

        FileUtils::readUint32(file, &indexExtDefSym);
        FileUtils::readUint32(file, &numberExtDefSym);

        FileUtils::readUint32(file, &indexUndefSym);
        FileUtils::readUint32(file, &numberUndefSym);

        FileUtils::readUint32(file, &tocOff);
        FileUtils::readUint32(file, &numberTocEntries);

        FileUtils::readUint32(file, &modTabOff);
        FileUtils::readUint32(file, &numberModTabEntries);

        FileUtils::readUint32(file, &extRefSymOff);
        FileUtils::readUint32(file, &numberExtRefSyms);

        FileUtils::readUint32(file, &tableOffset);
        FileUtils::readUint32(file, &numberEntries);

        FileUtils::readUint32(file, &extRelOff);
        FileUtils::readUint32(file, &numberExtRel);

        FileUtils::readUint32(file, &locRelOff);
        FileUtils::readUint32(file, &numberLocRel);
}

DynamicSymbolTableHeader::DynamicSymbolTableHeader()
{

}
uint32_t DynamicSymbolTableHeader::getTableOffset()
{
        return tableOffset;
}

uint32_t DynamicSymbolTableHeader::getNumberEntries()
{
        return numberEntries;
}

DynamicSymbolTableEntry::DynamicSymbolTableEntry(uint32_t index,
                uint64_t indirectAddress, char *name, Section *section)
{
        this->index = index;
        this->indirectAddress = indirectAddress;
        this->name = new char[strlen(name)+ 1];
        strcpy(this->name, name);
        this->section = section;
}

uint32_t DynamicSymbolTableEntry::getIndex()
{
        return index;
}

uint64_t DynamicSymbolTableEntry::getIndirectAdress()
{
        return indirectAddress;
}

char * DynamicSymbolTableEntry::getName()
{
        return name;
}
Section *DynamicSymbolTableEntry::getSection()
{
        return section;
}
DynamicSymbolTableEntry::~DynamicSymbolTableEntry()
{
        delete name;
}
