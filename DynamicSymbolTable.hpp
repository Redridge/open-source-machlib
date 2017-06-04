#ifndef __DYNAMICSYMBOLTABLE_HPP
#define __DYNAMICSYMBOLTABLE_HPP

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "FileUtils.hpp"
#include "Section.hpp"

/* An indirect symbol table entry is simply a 32bit index into the symbol table
* to the symbol that the pointer or stub is refering to.  Unless it is for a
* non-lazy symbol pointer section for a defined symbol which strip(1) as
* removed.  In which case it has the value INDIRECT_SYMBOL_LOCAL.  If the
* symbol was also absolute INDIRECT_SYMBOL_ABS is or'ed with that.
*/
#define INDIRECT_SYMBOL_LOCAL	0x80000000
#define INDIRECT_SYMBOL_ABS	0x40000000


class DynamicSymbolTableHeader {
private:
        /* index to local symbols */
        uint32_t indexLocalSym;
        /* number of local symbols */
        uint32_t numberLocalSym;

        /* index to externally defined symbols */
        uint32_t indexExtDefSym;
        /* number of externally defined symbols */
        uint32_t numberExtDefSym;

        /* index to undefined symbols */
        uint32_t indexUndefSym;
        /* number of undefined symbols */
        uint32_t numberUndefSym;

       /*
        * For the dynamic binding process to find which module a symbol
        * is defined in, the table of contents is used (analogous to the ranlib
        * structure in an archive) which maps defined external symbols to modules
        * they are defined in.  This exists only in a dynamically linked shared
        * library file.
        */

        /* file offset to table of contents */
        uint32_t tocOff;
        /* number of entries in table of contents */
        uint32_t numberTocEntries;

        /*
        * To support dynamic binding of "modules" (whole object files) the symbol
        * table must reflect the modules that the file was created from.  This is
        * done by having a module table that has indexes and counts into the merged
        * tables for each module.  The module structure that these two entries
        * refer to is described below.  This exists only in a dynamically linked
        * shared library file.
        */

        /* file offset to module table */
        uint32_t modTabOff;
        /* number of module table entries */
        uint32_t numberModTabEntries;

        /*
        * To support dynamic module binding the module structure for each module
        * indicates the external references (defined and undefined) each module
        * makes.  For each module there is an offset and a count into the
        * reference symbol table for the symbols that the module references.
        * This exists only in a dynamically linked shared library file.  For
        * executable and object modules the defined external symbols and the
        * undefined external symbols indicates the external references.
        */

        /* offset to referenced symbol table */
        uint32_t extRefSymOff;
        /* number of referenced symbol table entries */
        uint32_t numberExtRefSyms;

        /* file offset to the indirect symbol table */
        uint32_t tableOffset;
        /* number of indirect symbol table entries */
        uint32_t numberEntries;

        /* offset to external relocation entries */
        uint32_t extRelOff;
        /* number of external relocation entries */
        uint32_t numberExtRel;

        /*
        * All the local relocation entries are grouped together (they are not
        * grouped by their module since they are only used if the object is moved
        * from it staticly link edited address).
        */

        /* offset to local relocation entries */
        uint32_t locRelOff;
        /* number of local relocation entries */
        uint32_t numberLocRel;

public:
        DynamicSymbolTableHeader();
        DynamicSymbolTableHeader(FILE *file);
        uint32_t getTableOffset();
        uint32_t getNumberEntries();

};

class DynamicSymbolTableEntry
{
private:
        uint32_t index;
        uint64_t indirectAddress;
        char *name;
        Section *section;

public:
        DynamicSymbolTableEntry(uint32_t index, uint64_t indirectAddress,
                char *name, Section *section);
        uint32_t getIndex();
        uint64_t getIndirectAdress();
        char *getName();
        Section *getSection();

        ~DynamicSymbolTableEntry();
};

#endif
