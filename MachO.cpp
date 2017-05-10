#include "MachO.hpp"

MachO::MachO(char *fileName)
{
        uint32_t command, index, size;

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

                        /*parsing not yet implemented - -skip*/
                        default:
                                FileUtils::readUint32(file, &size);
                                fseek(file, size - 8, SEEK_CUR);
                                break;
                        }
        }

        stringTableComputed = false;
        symbolTableComputed = false;

}

MachHeader MachO::getHeader()
{
        return header;
}
std::vector<Segment *>MachO::getSegments()
{
        return segments;
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
