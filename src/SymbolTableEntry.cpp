#include "SymbolTableEntry.hpp"

SymbolTableEntry::SymbolTableEntry(FILE *file, char *strings)
{
                FileUtils::readUint32(file, &stringTableIndex);

                FileUtils::readUint8(file, &type);

                FileUtils::readUint8(file, &sectionIndex);

                FileUtils::readUint16(file, &description);

                /*get the name from string table*/
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
        //printf("entry dessctructor");
        delete name;
}

bool SymbolTableEntry::isDebug()
{
        return (type & STAB_MASK);
}

bool SymbolTableEntry::isPrivateExternal()
{
        return (type & PEXT_MASK);
}

bool SymbolTableEntry::isExternal()
{
        return (type & EXT_MASK);
}

bool SymbolTableEntry::isUndefined()
{
        return ((type & TYPE_MASK) == UNDEFINED);
}

bool SymbolTableEntry::isAbsolute()
{
        return ((type & TYPE_MASK) == ABSOLUTE);
}

bool SymbolTableEntry::isDefinedInSection()
{
        return ((type & TYPE_MASK) == DEFINED_IN_SECT);
}

bool SymbolTableEntry::isPrebound()
{
        return ((type & TYPE_MASK) == PREBOUND);
}

bool SymbolTableEntry::isIndirect()
{
        return ((type & TYPE_MASK) == INDIRECT);
}

bool SymbolTableEntry::isReferenceUndefinedNonLazy()
{
        if(isUndefined()) {
                return ((description & REFERENCE_MASK) == REFERENCE_FLAG_UNDEFINED_NON_LAZY);
        }

        return false;
}

bool SymbolTableEntry::isReferenceUndefinedLazy()
{
        if(isUndefined()) {
                return ((description & REFERENCE_MASK) == REFERENCE_FLAG_UNDEFINED_LAZY);
        }

        return false;
}

bool SymbolTableEntry::isReferenceDefined()
{
        if(isUndefined()) {
                return ((description & REFERENCE_MASK) == REFERENCE_FLAG_DEFINED);
        }

        return false;
}

bool SymbolTableEntry::isReferencePrivateDefined()
{
        if(isUndefined()) {
                return ((description & REFERENCE_MASK) == REFERENCE_FLAG_PRIVATE_DEFINED);
        }

        return false;
}

bool SymbolTableEntry::isReferencePrivateUndefinedNonLazy()
{
        if(isUndefined()) {
                return ((description & REFERENCE_MASK) == REFERENCE_FLAG_PRIVATE_UNDEFINED_NON_LAZY);
        }

        return false;
}

bool SymbolTableEntry::isReferencePrivateUndefinedLazy()
{
        if(isUndefined()) {
                return ((description & REFERENCE_MASK) == REFERENCE_FLAG_PRIVATE_UNDEFINED_LAZY);
        }

        return false;
}

bool SymbolTableEntry::isReferenceDynamically()
{
        if(isUndefined()) {
                return ((description & REFERENCE_MASK) == REFERENCED_DYNAMICALLY);
        }

        return false;
}

uint8_t SymbolTableEntry::getLibraryOrdinal()
{
        if(isUndefined())
                return ((description >> 8) & 0xFF);
        else
                return 0;
        //TODO throw exception
}

bool SymbolTableEntry::isWeakReferenced()
{
        return (description & WEAK_REF_MASK);
}

bool SymbolTableEntry::isWeakDefined()
{
        return (description & WEAK_DEF_MASK);
}

bool SymbolTableEntry::isRefToWeakSymbol()
{
        return (description & REF_TO_WEAK_MASK);
}

bool SymbolTableEntry::isSymbolResolver()
{
        return (description & SYMBOL_RESOLVER_MASK);
}

bool SymbolTableEntry::isArmThumb()
{
        return (description & ARM_THUMB_MASK);
}
