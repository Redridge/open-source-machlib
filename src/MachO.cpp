#include "MachO.hpp"
#include "pugixml.hpp"

MachO::MachO(char *fileName)
{
        uint32_t command, index, size;

        this->fileName = fileName;
        file = fopen(fileName, "rb");
        /*parse header*/
        header = MachHeader(file);

        loadDyLinkerCmd = NULL;
        symbolTableHeaderPresent = false;
        loadMainCmdPresent = false;
        functionStartsCmdPresent = false;

        /*parse load commands*/
        for (index = 0; index < header.getNumberCmds(); index++) {
                FileUtils::readUint32(file, &command);

                switch(command) {

                        case LC_SEGMENT32:
                                segments.push_back(new Segment32(file));
                                break;

                        case LC_SEGMENT64:
                                segments.push_back(new Segment64(file));
                                break;

                        case LC_SYMTAB:
                                symbolTableHeader = SymbolTableHeader(file);
                                symbolTableHeaderPresent = true;
                                break;

                        case LC_LOAD_DYLINKER:
                                loadDyLinkerCmd = new LoadDyLinkerCmd(file);
                                break;

                        case LC_UUID:
                                FileUtils::readUint32(file, &size);
                                FileUtils::readBytes(file, (char *)uuid, UUID_SIZE);
                                break;

                        case LC_MAIN:
                                loadMainCmd = LoadMainCmd(file);
                                loadMainCmdPresent = true;
                                break;

                        case LC_LOAD_DYLIB:
                                dynamicLibraries.push_back(new LibraryInfo(file));
                                break;

                        case LC_FUNCTION_STARTS:
                                functionStartsCmd = LinkEditCmd(file);
                                functionStartsCmdPresent = true;
                                break;

                        case LC_DYSYMTAB:
                                dynamicSymbolTableHeader = DynamicSymbolTableHeader(file);
                                break;

                        case LC_CODE_SIGNATURE:
                                codeSignatureCmd = LinkEditCmd(file);
                                codeSignatureCmdPresent = true;
                                break;

                        /*parsing not yet implemented - -skip*/
                        default:
                                FileUtils::readUint32(file, &size);
                                fseek(file, size - 8, SEEK_CUR);
                                break;
                        }
        }

        stringTableComputed = false;
        symbolTableComputed = false;
        functionsOffsetComputed = false;
        kextsInfoComputed = false;
        DynamicSymbolTableComputed = false;


}

MachHeader MachO::getHeader()
{
        return header;
}

std::vector<Segment *>MachO::getSegments()
{
        return segments;
}

/*get's a section based on the index*/
/*the index starts at 1*/
Section *MachO::getSectionByIndex(uint32_t index)
{
        uint32_t segment_index = 0, section_index = 0, i;

        while(segment_index < segments.size()) {
                Segment * seg = segments[segment_index];
                std::vector<Section *> sections =  seg->getSections();

                for( i = 0; i < sections.size(); i++) {
                        if (section_index == index - 1)
                                return sections[i];
                        section_index++;
                }
                segment_index++;
        }
        printf("invalid section index %d\n", index);
        return NULL;
}

Segment *MachO::getSegmentByName(char * name)
{
        uint32_t index;

        for(index = 0; index < segments.size(); index++)
                if(strcmp (segments[index]->getName(), name ) == 0)
                        return segments[index];

        printf("invalid segment name %s\n", name);
        return NULL;
}

Section *MachO::getSectionByName(char *segmentName, char *sectionName)
{

        uint32_t index;
        Segment *seg;
        std::vector<Section *> sections;

        seg = getSegmentByName(segmentName);
        if (seg == NULL) {
                printf("invalid segment name\n");
                return NULL;
        }

        sections = seg->getSections();
        for (index = 0; index < sections.size(); index++) {
                if (strcmp(sectionName, sections[index]->getSectionName()) == 0) {
                        return sections[index];
                }
        }

        return NULL;
}

SymbolTableHeader MachO::getSymbolTableHeader()
{
        if (!symbolTableHeaderPresent) {
                throw std::runtime_error("LC_SYMTAB not present");
        }

        return symbolTableHeader;
}

StringTable *MachO::getStringTable()
{

        if (!symbolTableHeaderPresent) {
                throw std::runtime_error("LC_SYMTAB not present");
        }

        if(!stringTableComputed) {
                stringTable =  new StringTable(file, symbolTableHeader);
                stringTableComputed = true;
        }

        return stringTable;
}

std::vector<SymbolTableEntry *> MachO::getSymbolTable()
{
        uint32_t index;
        StringTable *stringTable;
        char *strings;

        if(!symbolTableComputed) {

                stringTable = getStringTable();

                fseek(file, symbolTableHeader.getTableOffset(), SEEK_SET);
                strings = stringTable->getRaw();
                /*get the symbols*/
                for(index = 0; index < symbolTableHeader.getNumberofSymbols(); index++) {
                        if(header.getIs32())
                                symbolTable.push_back(new SymbolTableEntry32(file, strings));
                        else
                                symbolTable.push_back(new SymbolTableEntry64(file, strings));
                }

                symbolTableComputed = true;
        }

        return symbolTable;
}

LoadDyLinkerCmd *MachO::getLoadDyLinkerCmd()
{
        return loadDyLinkerCmd;
}

uint8_t *MachO::getUUID()
{
        return uuid;
}

LoadMainCmd MachO::getLoadMainCmd()
{
        if (!loadMainCmdPresent) {
                throw std::runtime_error("Main not present");
        }

        return loadMainCmd;
}

std::vector<LibraryInfo *> MachO::getDynamicLibrariesInfo()
{
        return dynamicLibraries;
}

std::vector<char *> MachO::listDynamicLibraries()
{
        uint32_t index;
        std::vector<char *> names(dynamicLibraries.size());

        for(index = 0; index < dynamicLibraries.size(); index++)
                names[index] = dynamicLibraries[index]->getName();

        return names;
}

LinkEditCmd MachO::getFunctionStartsCmd()
{
        if (!functionStartsCmdPresent) {
                throw std::runtime_error("LC_FUNCTION_STARTS not present");
        }

        return functionStartsCmd;
}

LinkEditCmd MachO::getCodeSignatureCmd()
{
        if (!codeSignatureCmdPresent) {
                throw std::runtime_error("LC_CODE_SIGNATURE not present");
        }
        return codeSignatureCmd;
}

std::map<uint64_t, char *> MachO::getFunctionsOffset()
{
        uint64_t addr;
        uint8_t *start, *end, *data, *cursor;
        uint32_t size = functionStartsCmd.getDataSize();

        if (!functionStartsCmdPresent) {
                throw std::runtime_error("LC_FUNCTION_STARTS not present");
        }

        addr = getSegmentByName((char*)"__TEXT")->getFileOffset();
        computeSymbolsFileOffset();

        if (!functionsOffsetComputed && size > 0) {
                data = new uint8_t[size];
                fseek(file, functionStartsCmd.getDataOffset(), SEEK_SET);
                FileUtils::readBytes(file, (char *)data, size);

                start = data;
                end = &data[size];

                for (cursor = start; (*cursor != 0) && (cursor < end); ) {
                        uint64_t delta = 0;
                        uint32_t shift = 0;
                        bool isMore = true;

                        do {
                                uint8_t byte = *cursor++;
                                delta |= ((byte & 0x7F) << shift);
                                shift += 7;
                                if (byte < 0x80) {
                                        addr += delta;
                                        functionsOffset[addr] = getFunctionName(addr);
                                        isMore = false;

                                }
                        } while(isMore);
                }

                delete data;
                functionsOffsetComputed = true;
        }

        return functionsOffset;

}

/*computes the symbol file offsets*/
void MachO::computeSymbolsFileOffset()
{
        std::vector<SymbolTableEntry *> symbolTable;
        uint32_t index;
        uint64_t symbolFileoffset;

        symbolTable = getSymbolTable();
        for (index = 0; index < symbolTable.size(); index++) {
                symbolFileoffset = getSymbolFileOffset(symbolTable[index]);
                if (symbolFileoffset != 0) {
                        symbolsFileOffset[symbolFileoffset] = symbolTable[index]->getName();
                        /*hack for thumb offset*/
                        symbolsFileOffset[symbolFileoffset + 1] = symbolTable[index]->getName();
                }
        }

}

/*computes the fileoffset for a symbol*/
uint64_t MachO::getSymbolFileOffset(SymbolTableEntry *symbol)
{

        uint64_t symbolFileoffset = 0, sectionOffset;
        Section *section;

        if (symbol->getSectionIndex() == 1  &&
                symbol->isDefinedInSection()) {
                section = getSectionByIndex(symbol->getSectionIndex());

                uint64_t sectionVirtualAddress = section->getVirtualAddress();
                uint64_t sectionFileOffset = section->getOffset();

                sectionOffset = symbol->getValue() - sectionVirtualAddress;
                symbolFileoffset = sectionFileOffset + sectionOffset;
        }

        return symbolFileoffset;
}

/*get's the name for a given offset*/
char *MachO::getFunctionName(uint64_t functionFileOffset)
{
        if(symbolsFileOffset.find(functionFileOffset) == symbolsFileOffset.end()) {
                char * name = new char[20];
                sprintf(name, "%s%llu", NAMEPREFIX, functionFileOffset);
                return name;
        }
        return symbolsFileOffset.at(functionFileOffset);
}

std::vector<std::map<char *, char *, myKextComp> > MachO::getKextsInfo()
{
        Segment *seg;
        Section *sec;
        uint64_t fileOffset;

        if (kextsInfoComputed)
                return kextsInfo;

        seg = getSegmentByName((char *)"__PRELINK_INFO");
        if (seg == NULL) {
                printf("no __PRELINK_INFO segment\n");
                return kextsInfo;
        }

        sec = seg->getSections()[0];

        fileOffset = sec->getOffset();

        char * raw = new char[sec->getSize()];


        fseek(file, fileOffset, SEEK_SET);
        FileUtils::readBytes(file, raw, sec->getSize());

        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_buffer(raw, sec->getSize());
        std::map<uint64_t, char *> integers;
        std::map<uint64_t, char *> strings;
        char *key;
        char *value;
        if (result) {
                printf("parse without errors\n");
                pugi::xml_node dict = doc.first_child();
                pugi::xml_node array = dict.child("array");

                for(pugi::xml_node dictionary = array.first_child(); dictionary; dictionary = dictionary.next_sibling()) {
                        std::map<char *, char *, myKextComp> kext;
                        for (pugi::xml_node node = dictionary.first_child(); node; node = node.next_sibling()) {
                                key = strdup(node.child_value());
                                node = node.next_sibling();
                                value = NULL;
                                if (strcmp(node.name(), "string") == 0) {
                                        if (strlen(node.attribute("ID").value()) != 0) {
                                                uint64_t id = node.attribute("ID").as_int();
                                                value = strdup(node.child_value());
                                                strings[id] = value;
                                        }

                                        if (strlen(node.attribute("IDREF").value()) != 0) {

                                                uint64_t id = node.attribute("IDREF").as_int();
                                                value = strdup(strings[id]);
                                        }
                                        if (value == NULL)
                                                value = strdup(node.child_value());
                                        kext[key] = value;
                                        continue;
                                }

                                if (strcmp(node.name(), "integer") == 0) {
                                        if (strlen(node.attribute("ID").value()) != 0) {
                                                uint64_t id = node.attribute("ID").as_int();
                                                value = strdup(node.child_value());
                                                integers[id] = value;
                                        }

                                        if (strlen(node.attribute("IDREF").value()) != 0) {

                                                uint64_t id = node.attribute("IDREF").as_int();
                                                if (integers[id] == NULL) {
                                                        value = strdup("1234");
                                                }
                                                else {
                                                        value = strdup(integers[id]);
                                                }
                                        }
                                        if (value == NULL)
                                                value = strdup(node.child_value());
                                        kext[key] = value;
                                        continue;
                                }

                                if (strcmp(node.name(), "data") == 0) {
                                        value = strdup(node.child_value());
                                        kext[key] = value;
                                        continue;
                                }

                                if (strcmp (node.name(), "true") == 0 || strcmp(node.name(), "false") == 0) {
                                        value = strdup(node.name());
                                        kext[key] = value;
                                        continue;
                                }
                                delete key;
                        }
                        kextsInfo.push_back(kext);
                }
                delete[] raw;
                kextsInfoComputed = true;
                return kextsInfo;
        }
        else {
                printf("error parsing the kexts information\n");
                return kextsInfo;
        }
}

std::vector<std::map<char *, char *, myKextComp> > MachO::getKextByProperty(char *key,
                                                char *value)
{
        uint32_t index;
        std::vector<std::map<char *, char *, myKextComp> > result;

        if (!kextsInfoComputed) {
                getKextsInfo();
        }

        for (index = 0; index < kextsInfo.size(); index++) {
                if (kextsInfo[index].find(key) != kextsInfo[index].end()) {
                        if (strcmp(kextsInfo[index][key], value) == 0) {
                                result.push_back(kextsInfo[index]);
                        }
                }
        }

        return result;
}

std::map<char *, char *, myKextComp> MachO::getKextByBundleId(char * bundleId)
{
        std::vector<std::map<char *, char *, myKextComp> > array;
        std::map<char *, char *, myKextComp> result;

        array = getKextByProperty((char *)"CFBundleIdentifier", bundleId);
        if (array.size() > 0) {
                result = array[0];
        }
        else {
                printf("invalid id\n");
        }

        return result;
}

uint64_t MachO::getVirtToFile(uint64_t virtualAddress)
{
        uint32_t index;
        Segment *seg = NULL;
        uint64_t segmentOffset;

        for (index = 1; index < segments.size(); index++) {
                if (virtualAddress >= segments[index]->getVirtualAddress() &&
                virtualAddress <= segments[index]->getVirtualAddress() + segments[index]->getVirtualSize()) {
                        seg = segments[index];
                        break;
                }
        }

        if (seg == NULL) {
                printf("invalid address\n");
                return 0;
        }

        segmentOffset = virtualAddress -  seg->getVirtualAddress();
        return seg->getFileOffset() + segmentOffset;
}

void MachO::dumpKext(char *bundleId, char *fileName)
{
        uint32_t index;
        uint64_t virtualAddress, fileOffset, size;
        std::map<char *, char *, myKextComp> extension;
        FILE *destination;
        char * raw;

        if (! kextsInfoComputed) {
                getKextsInfo();
        }

        for (index = 0; index < kextsInfo.size(); index++) {
                if (strcmp(kextsInfo[index][(char *) "CFBundleIdentifier"], bundleId) == 0) {
                        extension = kextsInfo[index];
                        if (extension.find((char *)"_PrelinkExecutableLoadAddr") == extension.end()) {
                                printf("kernel extension not loaded\n");
                                return;
                        }
                        virtualAddress = strtoul(extension[(char *) "_PrelinkExecutableLoadAddr"], NULL, 16);
                        fileOffset = getVirtToFile(virtualAddress);
                        size = strtoul(extension[(char *)"_PrelinkExecutableSize"], NULL, 16);
                        //printf("got size = %llx\n", size);

                        raw = new char[size];
                        fseek(file, fileOffset, SEEK_SET);
                        FileUtils::readBytes(file, raw, size);

                        destination = fopen(fileName, "wb");
                        if (destination == NULL) {
                                printf("can not create the file\n");
                                return;
                        }

                        fwrite(raw, size, 1, destination);
                        delete raw;
                        return;
                }
        }

        printf("extension not found\n");
        return;
}

std::vector<DynamicSymbolTableEntry *> MachO::getDynamicSymbolTable()
{
        uint32_t index, value, sectionIndex, symIndex, stubSize;
        uint32_t noSymbols, entrySize;
        uint64_t offset;
        std::vector<uint32_t> indexes;
        std::vector<Section *> sections;
        Section *section;

        if (DynamicSymbolTableComputed) {
                return dynamicSymbolTable;
        }

        if (!symbolTableComputed) {
                getSymbolTable();
        }

        fseek(file, dynamicSymbolTableHeader.getTableOffset(), SEEK_SET);
        /*read the indexes*/
        for(index = 0; index < dynamicSymbolTableHeader.getNumberEntries(); index++) {
                FileUtils::readUint32(file, &value);
                indexes.push_back(value);
        }

        /*search for the stub, lazy, non lazy pointer section*/
        for (index = 0; index < segments.size(); index++) {
                sections = segments[index]->getSections();
                for (sectionIndex = 0; sectionIndex < sections.size(); sectionIndex++) {
                        /*symbol stub*/
                        if (sections[sectionIndex]->getType() == S_SYMBOL_STUBS) {
                                section = sections[sectionIndex];
                                offset = section->getVirtualAddress();
                                symIndex = section->getReserved1();
                                stubSize = section->getReserved2();
                                noSymbols = section->getSize() / stubSize;
                                for (uint32_t  i = symIndex; i < symIndex + noSymbols; i++, offset = offset + stubSize) {
                                        char * name = symbolTable[indexes[i]]->getName();
                                        dynamicSymbolTable.push_back(new DynamicSymbolTableEntry(indexes[i], offset, name, section));
                                }
                        }
                        /*lazy and non lazy pointers*/
                        if (sections[sectionIndex]->getType() == S_NON_LAZY_SYMBOL_POINTER ||
                                sections[sectionIndex]->getType() == S_LAZY_SYMBOL_POINTERS) {
                                section = sections[sectionIndex];
                                offset = section->getVirtualAddress();
                                symIndex = section->getReserved1();
                                entrySize = header.getIs32() ? 4 : 8;
                                noSymbols = section->getSize() / entrySize;
                                for (uint32_t  i = symIndex; i < symIndex + noSymbols; i++, offset = offset + entrySize) {
                                        char *name = NULL;
                                        if (indexes[i] == INDIRECT_SYMBOL_ABS )
                                                name = (char *)"ABSOLUTE";

                                        if (indexes[i] == INDIRECT_SYMBOL_LOCAL)
                                                name = (char *)"LOCAL";

                                        if (indexes[i] == (INDIRECT_SYMBOL_ABS | INDIRECT_SYMBOL_LOCAL) )
                                                name = (char *)"ABSOLUTE AND LOCAL";

                                        if (name == NULL)
                                                name = symbolTable[indexes[i]]->getName();

                                        dynamicSymbolTable.push_back(new DynamicSymbolTableEntry(indexes[i], offset, name, section));
                                }
                        }
                }
        }
        DynamicSymbolTableComputed = true;
        return dynamicSymbolTable;
}

char *MachO::getFileName()
{
        return fileName;
}

MachO::~MachO()
{
        uint32_t index;

        for(index = 0; index < segments.size(); index++){
                delete segments[index];
        }

        if(stringTableComputed)
                delete stringTable;

        if(symbolTableComputed) {
                for(index = 0; index < symbolTable.size(); index++)
                        delete symbolTable[index];
        }

        delete loadDyLinkerCmd;

        for(index = 0; index < dynamicLibraries.size(); index++)
                delete dynamicLibraries[index];

        if (kextsInfoComputed) {
                for (index = 0; index < kextsInfo.size(); index++) {
                        std::map<char *, char *, myKextComp> map = kextsInfo[index];
                        std::map<char *, char *, myKextComp>::iterator it;
                        for (it = map.begin(); it != map.end(); ++it) {
                                delete(it->first);
                                delete(it->second);
                        }
                }
        }

        if (DynamicSymbolTableComputed) {
                for (index = 0; index < dynamicSymbolTable.size(); index++)
                        delete dynamicSymbolTable[index];
        }

        if (!functionsOffset.empty()) {
                std::map<uint64_t, char *>::iterator it;
                for (it = functionsOffset.begin(); it != functionsOffset.end(); ++it) {
                        if (strncmp(NAMEPREFIX, it->second, 4) == 0) {
                                delete(it->second);
                        }
                }
        }

        fclose(file);
}

SuperBlob MachO::getSuperBlob()
{
        if (!codeSignatureCmdPresent) {
                throw std::runtime_error("LC_CODE_SIGNATURE not present");
        }
	return SuperBlob(file, codeSignatureCmd);
}
