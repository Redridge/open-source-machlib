#include "MachO.hpp"

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

                uint64_t sectionVirtualAddress = segment->getVirtualAddress() +
                                        section->getOffset();
                uint64_t sectionFileOffset = segment->getFileOffset() +
                                        section->getOffset();

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


char *MachO::getFileName()
{
        return fileName;
}
MachO::~MachO()
{
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

        fclose(file);
}
