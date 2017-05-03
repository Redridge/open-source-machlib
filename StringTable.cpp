#include "StringTable.hpp"

StringTable::StringTable(FILE *file, SymbolTableHeader tableHeader)
{
        uint32_t tableSize, index, dataIndex;

        fseek(file, tableHeader.getStringTableOffset(), SEEK_SET);

        tableSize = tableHeader.getStringTableSize();
        raw = new char[tableSize];

        FileUtils::readBytes(file, raw, tableSize);

        /*index the table*/
        index = 0;
        dataIndex = 0;
        while(dataIndex < tableSize) {
                table.push_back(&raw[dataIndex]);
                dataIndex += strlen(&raw[dataIndex]) + 1;
        }

        numberOfStrings = table.size();

}

StringTable::StringTable()
{

}

char *StringTable::getRaw()
{
        return raw;
}

char *StringTable::get(uint32_t index)
{
        if(index < table.size())
                return table[index];
        //TODO throw exception when index out of bounds
        return NULL;
}

uint32_t StringTable::getNumberOfStrings()
{
        return numberOfStrings;
}

StringTable::~StringTable()
{
        delete raw;
}
