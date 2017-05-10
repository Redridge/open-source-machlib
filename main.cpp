#include <stdio.h>
#include <stdint.h>
#include <vector>
#include "MachO.hpp"


void print_header(MachHeader header)
{
        if (header.getMagic() == MAGIC_32)
                printf("MACH-O 32\n");
        else
                printf("MACH-O 64\n");

        printf("cputype:%d\n subtype:%x\nfiletype:%x\nncmds:%d\nsize:%d\nflags:%x\n", header.getCpuType(),
                        header.getCpuSubType(), header.getFileType(), header.getNumberCmds(), header.getSizeOfCmds(), header.getFlags());

        if (header.getMagic() == MAGIC_64)
                printf("reserved:%x\n", header.getReserved());
}
void print_section(Section *section)
{
        printf("Segment name: %s\nSection name %s\n",
        section->getSegmentName(), section->getSectionName());

        printf("virtual address: %lld\nsize: %lld\n",
        section->getVirtualAddress(), section->getSize());

        printf("offset: %d\nnumber of relocations: %d\nalign: %d\n" ,
        section->getOffset(), section->getNumberRelocations(), section->getAlign());

        printf("relocation offset: %d\nflags: %u\n",
        section->getRelocationOffset(), section->getFlags());

        printf("reserved1: %d\nreserved2: %d\n",
        section->getReserved1(), section->getReserved2());
}

void print_segment(Segment *segment)
{
        printf("Segment Name: %s\n", segment->getName());

        printf("virtual address: %lld\nvirtual size: %lld\n",
                segment->getVirtualAddress(), segment->getVirtualSize());

        printf("file offset: %lld\nfile size: %lld\n",
                segment->getFileOffset(), segment->getFileSize() );

        printf("init prot: 0x%x\nmax prot: %x\n",
                segment->getInitProtection(), segment->getMaxProtection());

        printf("flags: %x\nnumber of sections: %d\n",
                segment->getFlags(), segment->getNumberSections());

        std::vector<Section *> sections = segment->getSections();
        for(int i = 0; i < segment->getNumberSections(); i++)
                print_section(sections[i]);
}

void print_symbol_header(SymbolTableHeader header)
{
        printf("tableOffset: %u\nnumber symbols: %u\n",
        header.getTableOffset(), header.getNumberofSymbols());

        printf("string table offset: %u\nstring table size: %u\n",
        header.getStringTableOffset(), header.getStringTableSize());
}

void print_symbol(SymbolTableEntry *entry)
{
        printf("string table index: %u\n", entry->getStringTableIndex());

        printf("symbol name: %s\n", entry->getName());

        printf("type: 0x%x Debug:%d PrivateExternal:%d External:%d\nUndefined:%d Absolute: %d DefinSection: %d Prebound:%d Indirect %d\n",
        entry->getType(), entry->isDebug(), entry->isPrivateExternal(), entry->isExternal(),
        entry->isUndefined(), entry->isAbsolute(), entry->isDefinedInSection(),
        entry->isPrebound(), entry->isIndirect());

        printf("section index: %u\n", entry->getSectionIndex());

        printf("description: %u\n", entry->getDescription());
        if(entry->isUndefined())
        printf("library ordinal: %d\n", entry->getLibraryOrdinal());

        printf("value: %llu\n", entry->getValue());
}

void print_lib(LibraryInfo * lib)
{
        printf("version:%u\n name: %s\n", lib->getCurrentVersion(), lib->getName());

        printf("timestamp: %u\n Comp: %u\n", lib->getTimestamp(), lib->getCompatibilityVersion());
}
/*listing of segments and sections*/
int main(int argc, char *argv[])
{
        FILE *file;

        if (argc != 2) {
                printf("Usage: %s <binary>\n", argv[0]);
                return -1;
        }
        MachO bin(argv[1]);
        MachHeader header = bin.getHeader();
        //print_header(header);

        std::vector<Segment *> segments = bin.getSegments();

        for(int i = 0; i < segments.size(); i++) {
               //print_segment(segments[i]);
        }

        SymbolTableHeader symbolTableHeader = bin.getSymbolTableHeader();
        //print_symbol_header(symbolTableHeader);

        StringTable *stringTable = bin.getStringTable();
        for(int i = 0; i < stringTable->getNumberOfStrings(); i++) {
                //printf("%d---%s\n", i,  stringTable->get(i) );
        }

        std::vector<SymbolTableEntry *> symbolTable = bin.getSymbolTable();

        for(int i = 0; i < symbolTable.size(); i++) {
                //print_symbol(symbolTable[i]);
        }

        /*LoadDyLinkerCmd *cmd = bin.getLoadDyLinkerCmd();
        printf(" the linker name is %s\n", cmd->getLinkerName() );

        uint8_t *uuid = bin.getUUID();
        printf("the uuid is:\n");
        for(int i = 0; i < 16; i++) {
                printf("%x", uuid[i]);
                if((i + 1) % 4 == 0)
                        printf("-");
        }

        LoadMainCmd mainCmd = bin.getLoadMainCmd();

        printf("\nthe main command\n");
        printf ("entryOffset: %llu, stacksize: %llu", mainCmd.getEntryOffset(),
mainCmd.getStackSize());*/

        /*std::vector<LibraryInfo *> libs = bin.getDynamicLibrariesInfo();
        for(int i = 0; i < libs.size(); i++)
                print_lib(libs[i]);*/

        std::vector<char *> names = bin.listDynamicLibraries();
        for(int i = 0; i < names.size(); i++)
                printf("%s\n", names[i]);

        return 0;
}
