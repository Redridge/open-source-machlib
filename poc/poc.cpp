#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <iostream>
#include "MachO.hpp"
#include "FileReader.hpp"


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

        printf("virtual address: %llu\nsize: %llu\n",
        section->getVirtualAddress(), section->getSize());

        printf("offset: %x\nnumber of relocations: %d\nalign: %d\n" ,
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

        printf("file offset: %llx\nfile size: %lld\n",
                segment->getFileOffset(), segment->getFileSize() );

        printf("init prot: 0x%x\nmax prot: %x\n",
                segment->getInitProtection(), segment->getMaxProtection());

        printf("flags: %x\nnumber of sections: %d\n",
                segment->getFlags(), segment->getNumberSections());

        std::vector<Section *> sections = segment->getSections();
        for(uint32_t i = 0; i < segment->getNumberSections(); i++)
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

        printf("value: 0x%llx\n", entry->getValue());
}
void print_lib(LibraryInfo * lib)
{
        printf("version:%u\n name: %s\n", lib->getCurrentVersion(), lib->getName());

        printf("timestamp: %u\n Comp: %u\n", lib->getTimestamp(), lib->getCompatibilityVersion());
}
/*listing of segments and sections*/
int main(int argc, char *argv[])
{

       // int option;

        if (argc != 3) {
                printf("Usage: %s <binary> <option>\n", argv[0]);
                return -1;
        }
        //sscanf(argv[2], "%d", &option);


        MachO bin(argv[1]);
        if(strstr(argv[2], "head")) {
                MachHeader header = bin.getHeader();
                print_header(header);
        }

        if(strstr(argv[2], "seglc")) {
                std::vector<Segment *> segments = bin.getSegments();

                for(uint32_t i = 0; i < segments.size(); i++) {
                        print_segment(segments[i]);
                }
        }

        if(strstr(argv[2], "symlc")) {
                SymbolTableHeader symbolTableHeader = bin.getSymbolTableHeader();
                print_symbol_header(symbolTableHeader);
        }

        if(strstr(argv[2], "strtab")) {
                StringTable *stringTable = bin.getStringTable();
                for(uint32_t i = 0; i < stringTable->getNumberOfStrings(); i++) {
                        printf("%d---%s\n", i,  stringTable->get(i) );
                }
        }

        if(strstr(argv[2], "symtab")) {
                std::vector<SymbolTableEntry *> symbolTable = bin.getSymbolTable();

                for(uint32_t i = 0; i < symbolTable.size(); i++) {
                        print_symbol(symbolTable[i]);
                        printf("\n\n");
                }
        }

        if(strstr(argv[2], "misc")) {
                LoadDyLinkerCmd *cmd = bin.getLoadDyLinkerCmd();
                if(cmd != NULL)
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
                mainCmd.getStackSize());
        }

        if(strstr(argv[2], "liblc")) {
                std::vector<char *> names = bin.listDynamicLibraries();
                for(uint32_t i = 0; i < names.size(); i++)
                        printf("%s\n", names[i]);
                }

        if(strstr(argv[2], "fc_starts")) {
                LinkEditCmd fcstart = bin.getFunctionStartsCmd();

                printf("Function starts Cmd ------------\n");
                printf("dataOffset: %u\ndataSize: %u\n", fcstart.getDataOffset(),
                fcstart.getDataSize());

                std::map<uint64_t, char *> starts = bin.getFunctionsOffset();
                std::map<uint64_t, char *>::iterator it;

                for(it = starts.begin(); it != starts.end(); ++it)
                        printf("0x%llx ----- %s\n", it->first, it->second);
        }

        if(strstr(argv[2], "dis_all")) {
                FileReader fileReader(&bin);
                fileReader.Disassemble();
        }

        if(strstr(argv[2], "dis_main")) {
                FileReader fileReader(&bin);
                fileReader.Disassemble((char *)"_main");
        }

        if(strstr(argv[2], "dis_offset")) {
                FileReader fileReader(&bin);
                fileReader.Disassemble(7030);
        }

        if(strstr(argv[2], "kexts")) {
                std::vector<std::map<char *, char *, myKextComp> > map = bin.getKextsInfo();
                std::map<char *, char *, myKextComp>::iterator it;
                for (uint32_t i = 0; i < map.size(); i++) {
                        printf("kext %d\n", i);
                        for (it = map[i].begin(); it != map[i].end(); ++it) {
                                printf("%s -- %s\n", it->first, it->second);
                        }
                }
                printf("got %lu kernel extensions\n", map.size() );
        }

        if(strstr(argv[2], "find_kext")) {
                std::map<char *, char *, myKextComp> map2;
                map2 = bin.getKextByBundleId((char *)"com.apple.kpi.mach");
                std::map<char *, char *, myKextComp>::iterator it;
                for (it = map2.begin(); it != map2.end(); ++it) {
                        printf("%s --- %s\n", it->first, it->second);
                }
        }

        if(strstr(argv[2], "dump_section")) {
                FileReader fileReader(&bin);
                uint64_t size;
                char * raw = fileReader.dumpSection((char *)"__TEXT", (char *)"__cstring", &size);
                for (uint64_t i = 0; i < size; i++)
                        printf("%c", raw[i]);
                delete raw;
        }

        if (strstr(argv[2], "dump_kext")) {
                bin.dumpKext((char *)"com.apple.iokit.IOTimeSyncFamily", (char *) "kpi");
        }

        if (strstr(argv[2], "indsymtab")) {
                std::vector<DynamicSymbolTableEntry *> index = bin.getDynamicSymbolTable();
                for (uint32_t  i = 0; i < index.size(); i++) {
                        printf("%u---", index[i]->getIndex());
                        printf("%llx --- ", index[i]->getIndirectAdress() );
                        printf("%s\n", index[i]->getName());
                }

        }

	if (strstr(argv[2], "siglc")) {
		LinkEditCmd codeSignatureCmd = bin.getCodeSignatureCmd();
		puts("LC_CODE_SIGNATURE");
		printf("Offset: %d, Size: %d\n", codeSignatureCmd.getDataOffset(),
			codeSignatureCmd.getDataSize());

		puts("---------------------------");

		SuperBlob sb = bin.getSuperBlob();
		puts("SuperBlob");
		printf("Length: %d, NumBlobs: %d\n", sb.getLength(), sb.getNumBlobs());

		std::vector<struct subblob> sbs = sb.getSubBlobs();
		for (unsigned int i = 0; i < sb.getNumBlobs(); i++) {
			printf("\tType: %d, Offset: %d\n", sbs[i].type, sbs[i].offset);
		}
	}

	if (strstr(argv[2], "ent")) {
		Entitlements ent = bin.getEntitlements();
		puts("Entitlements");
		puts("------------");
		std::cout<<ent.getXml()<<"\n";
	}

	if (strstr(argv[2], "cdb")) {
		CodeDirectoryBlob cdb = bin.getCodeDirectoryBlob();
		puts("Code Directory Blob");
		puts("------------");
		printf("length: %d\n", cdb.getLength());
		printf("version: %#08X\n", cdb.getVersion());
		printf("flags: %#08X\n", cdb.getFlags());
		printf("hashOffset: %d\n", cdb.getHashOffset());
		printf("identOffset: %d\n", cdb.getIdentOffset());
		printf("nSpecialSlots: %d\n", cdb.getNSpecialSlots());
		printf("nCodeSlots: %d\n", cdb.getNCodeSlots());
		printf("codeLimit: %d\n", cdb.getCodeLimit());
		printf("hashType: %d\n", cdb.getHashType());
		printf("hashSize: %d\n", cdb.getHashSize());
		printf("pageSize: %d\n", cdb.getPageSize());

		uint32_t nSpecialSlots = cdb.getNSpecialSlots();
		uint32_t nCodeSlots = cdb.getNCodeSlots();
		uint32_t hashSize = cdb.getHashSize();
		std::vector<char*> hashes = cdb.getHashes();
		for (uint32_t i = 0; i < nSpecialSlots; i++) {
			printf("%d: ", -nSpecialSlots + i);
			for (uint32_t j = 0; j < hashSize; j++)
				printf("%02hhx", hashes[i][j]);
			puts("");
		}
		for (uint32_t i = nSpecialSlots; i < nSpecialSlots + nCodeSlots; i++) {
			printf("%d: ", i - nSpecialSlots);
			for (uint32_t j = 0; j < hashSize; j++)
				printf("%02hhx", hashes[i][j]);
			puts("");
		}
	}
        return 0;
}
