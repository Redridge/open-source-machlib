#ifndef __SYMBOLTABLENTRY_HPP
#define __SYMBOLTABLENTRY_HPP

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "FileUtils.hpp"

/*masks for type bits*/
#define STAB_MASK                       0xe0    /* if any of these bits set, a symbolic debugging entry */
#define PEXT_MASK                       0x10    /* private external symbol bit */
#define TYPE_MASK                       0x0e    /* mask for the N_TYPE bits */
#define EXT_MASK                        0x01    /* external symbol bit, set for external symbols */

/*values for N_TYPE*/
#define	UNDEFINED                       0x0     /* undefined */
#define	ABSOLUTE                        0x2     /* absolute */
#define	DEFINED_IN_SECT                 0xe     /* defined in section number sectionIndex */
#define	PREBOUND                        0xc     /* prebound undefined (defined in a dylib) */
#define INDIRECT                        0xa     /* indirect */

/*mask for the description field in case of undefined symbols*/
#define REFERENCE_MASK                  0xf

/*type of references*/
#define REFERENCE_FLAG_UNDEFINED_NON_LAZY               0
#define REFERENCE_FLAG_UNDEFINED_LAZY                   1
#define REFERENCE_FLAG_DEFINED                          2
#define REFERENCE_FLAG_PRIVATE_DEFINED                  3
#define REFERENCE_FLAG_PRIVATE_UNDEFINED_NON_LAZY       4
#define REFERENCE_FLAG_PRIVATE_UNDEFINED_LAZY           5


/*
 * To simplify stripping of objects that use are used with the dynamic link
 * editor, the static link editor marks the symbols defined an object that are
 * referenced by a dynamicly bound object (dynamic shared libraries, bundles).
 * With this marking strip knows not to strip these symbols.
 */
#define REFERENCED_DYNAMICALLY	0x0010

class SymbolTableEntry
{
protected:
        uint32_t stringTableIndex;
        uint8_t type;
        uint8_t sectionIndex;
        uint16_t description;
        char *name;

public:
        SymbolTableEntry(FILE *file, char *strings);
        SymbolTableEntry();
        uint32_t getStringTableIndex();
        uint8_t getType();
        uint8_t getSectionIndex();
        uint16_t getDescription();
        char *getName();
        virtual uint64_t getValue() = 0;

        bool isDebug();
        bool isPrivateExternal();
        bool isExternal();
        bool isUndefined();
        bool isAbsolute();
        bool isDefinedInSection();
        bool isPrebound();
        bool isIndirect();

        bool isReferenceUndefinedNonLazy();
        bool isReferenceUndefinedLazy();
        bool isReferenceDefined();
        bool isReferencePrivateDefined();
        bool isReferencePrivateUndefinedNonLazy();
        bool isReferencePrivateUndefinedLazy();
        bool isReferenceDynamically();

        uint8_t getLibraryOrdinal();

        virtual ~SymbolTableEntry();

};


#endif
