#include "MachO.hpp"

MachO::MachO(char *fileName)
{
        uint32_t command, index;

        file = fopen(fileName, "rb");
        /*parse header*/
        header = MachHeader(file);

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

                        /*parsing not yet implemented - -skip*/
                        default:
                                uint32_t size;
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

                fseek(file, symbolTableHeader.getTableOffset(), SEEK_SET);

                stringTable = getStringTable();
                strings = stringTable->getRaw();

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

        fclose(file);
}
