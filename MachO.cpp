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
                                break;

                        case LC_LOAD_DYLIB:
                                dynamicLibraries.push_back(new LibraryInfo(file));
                                break;

                        case LC_FUNCTION_STARTS:
                                functionStartsCmd = FunctionStartsCmd(file);
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
        //TODO throw exception when index too big;
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
Section *MachO::getSectionByName(char *segmentName, char *sectionName) {

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
        return symbolTableHeader;
}

StringTable *MachO::getStringTable()
{
        if(!stringTableComputed) {
                stringTable =  new StringTable(file, symbolTableHeader);
                stringTableComputed = true;
        }

        return stringTable;
}

std::vector<SymbolTableEntry *> MachO::getSymbolTable()
{
        int index;
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
        //TODO not all files have main
        return loadMainCmd;
}

std::vector<LibraryInfo *> MachO::getDynamicLibrariesInfo()
{
        return dynamicLibraries;
}

std::vector<char *> MachO::listDynamicLibraries()
{
        int index;
        std::vector<char *> names(dynamicLibraries.size());

        for(index = 0; index < dynamicLibraries.size(); index++)
                names[index] = dynamicLibraries[index]->getName();

        return names;
}

FunctionStartsCmd MachO::getFunctionStartsCmd()
{
        return functionStartsCmd;
}

std::map<uint64_t, char *> MachO::getFunctionsOffset()
{
        uint64_t addr;
        uint8_t *start, *end, *data, *cursor;
        uint32_t size = functionStartsCmd.getDataSize();

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
        Segment *segment;

        if (symbol->getSectionIndex() == 1  &&
                symbol->isDefinedInSection()) {
                section = getSectionByIndex(symbol->getSectionIndex());
                segment = getSegmentByName(section->getSegmentName());

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
                //TODO dont forget to free this stuff
                char * name = new char[20];
                sprintf(name, "%s%llu", NAMEPREFIX, functionFileOffset);
                return name;
        }
        return symbolsFileOffset.at(functionFileOffset);
}

std::vector<std::map<char *, char *, myKextComp> > MachO::getKextsInfo()
{
        printf("dumping\n");
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
        printf("got segment %s\n", seg->getName());
        sec = seg->getSections()[0];
        printf("got sections %s with size %llu\n", sec->getSectionName(), sec->getSize());

        fileOffset = sec->getOffset();
        printf("fileoffset = %llu\n", fileOffset);

        char * raw = new char[sec->getSize()];


        fseek(file, fileOffset, SEEK_SET);
        FileUtils::readBytes(file, raw, sec->getSize());
        //      printf("%s\n", raw);

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
                                //printf("%s -- ", node.child_value());
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
                                        //printf("string %s\n", value);
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
                                        //printf(" integer %s\n", value);
                                        kext[key] = value;
                                        continue;
                                }

                                if (strcmp(node.name(), "data") == 0) {
                                        value = strdup(node.child_value());
                                        //printf("%s\n", value);
                                        kext[key] = value;
                                        continue;
                                }

                                if (strcmp (node.name(), "true") == 0 || strcmp(node.name(), "false") == 0) {
                                        value = strdup(node.name());
                                        //printf(" bool %s\n", value);
                                        kext[key] = value;
                                        continue;
                                }
                                //printf("value not found\n");
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

        printf("segment name %s\n", seg->getName());
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
char *MachO::getFileName()
{
        return fileName;
}
MachO::~MachO()
{
        //printf("destructor\n");
        fflush(stdout);
        int index;

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
                //printf("freeing kext\n");
                fflush(stdout);
                for (index = 0; index < kextsInfo.size(); index++) {
                        std::map<char *, char *, myKextComp> map = kextsInfo[index];
                        std::map<char *, char *, myKextComp>::iterator it;
                        for (it = map.begin(); it != map.end(); ++it) {
                                delete(it->first);
                                delete(it->second);
                        }
                }
        }

        fclose(file);
}
