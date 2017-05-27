#include "FileReader.hpp"

std::map<uint32_t, cs_arch> FileReader::makeCapstoneArch()
{
        std::map<uint32_t, cs_arch> map;
        map[CPU_TYPE_I386] = CS_ARCH_X86;
        map[CPU_TYPE_X86_64] = CS_ARCH_X86;
        map[CPU_TYPE_ARM] = CS_ARCH_ARM;
        map[CPU_TYPE_ARM64] = CS_ARCH_ARM64;

        return map;
}
std::map<uint32_t, cs_arch> FileReader::capstoneArch = FileReader::makeCapstoneArch();


/*void FileReader::makeSymbolsMap()
{
        uint64_t symbolOffset;
        uint32_t index;

        std::vector<SymbolTableEntry *> symbols = binary->getSymbolTable();
        for (index = 0; index < symbols.size(); index++) {
                symbolOffset = binary->getSymbolFileOffset(symbols[index]);
                if (symbolOffset != 0) {
                        symbolsOffset.push_back(symbolOffset);
                        symbolsMap[symbolOffset] = symbols[index];
                }
        }

        std::sort(symbolsOffset.begin(), symbolsOffset.end());
}*/

FileReader::FileReader(MachO *binary)
{
        this->binary = binary;
        functionStartsMap = binary->getFunctionsOffset();
        printf("opened %s\n", this->binary->getFileName());
        file = fopen(this->binary->getFileName(), "rb");

        MachHeader header = this->binary->getHeader();
        if (capstoneArch.find(header.getCpuType()) == capstoneArch.end()) {
                //TODO throw exception when cpu not supported;
                printf("cpu not supported\n");
                return;
        }

        capstoneArchOption = capstoneArch[header.getCpuType()];
        if(capstoneArchOption == CS_ARCH_ARM)
                printf("funcking yeah \n");
        capstoneModeOption = CS_MODE_THUMB;
        if (header.getIs32()) {
                if (capstoneArchOption == CS_ARCH_X86)
                        capstoneModeOption = CS_MODE_32;
                }
        else {
                if (capstoneArchOption == CS_ARCH_X86)
                        capstoneModeOption = CS_MODE_32;
                }
        if(capstoneModeOption == CS_MODE_THUMB)
                printf("fucking yeah\n");

        if (cs_open(capstoneArchOption, capstoneModeOption, &capstoneHandle) != CS_ERR_OK)
                printf("mare eroare\n");
                //TODO throw exception

        //makeSymbolsMap();

        functionStartsMap = this->binary->getFunctionsOffset();

        std::map<uint64_t, char *>::iterator it;

        for (it = functionStartsMap.begin(); it != functionStartsMap.end(); ++it) {
                if (it->first % 2)
                        functionsOffset.push_back(it->first - 1);
                else
                        functionsOffset.push_back(it->first);

                functionNamesMap[it->second] = it->first;
        }
        std::sort(functionsOffset.begin(), functionsOffset.end());


        std::map<char *, uint64_t>::iterator it2;
        for (it2 = functionNamesMap.begin(); it2 != functionNamesMap.end(); ++it2) {

                printf ("%s --- %llx\n", it2->first, it2->second);
        }

}

void FileReader::Disassemble()
{
        Section *sec;
        Segment *seg;
        uint64_t offset;

        sec = binary->getSectionByIndex(1);
        seg = binary->getSegmentByName(sec->getSegmentName());

        offset = seg->getFileOffset() + sec->getOffset();
        printf("%s %s\n", sec->getSegmentName(), sec->getSectionName());
        printf("%llu %llu", offset, sec->getSize());

        Disassemble(offset, sec->getSize());
}

void FileReader::Disassemble(char *functionName)
{
        uint64_t fileOffset, nextOffset, codeSize;
        const uint8_t *code;

        if (functionNamesMap.find(functionName) != functionNamesMap.end()) {
                fileOffset = functionNamesMap[functionName];
                printf("functionn %s found at %llx\n", functionName, fileOffset);
        }
        else {
                printf("function %s not found\n", functionName);
                return;
        }

        nextOffset = getNextSymbolOffset(fileOffset);
        codeSize = nextOffset - fileOffset;
        printf("code size = %llx\n", codeSize);

        code = new uint8_t[codeSize];
        fseek(file, fileOffset, SEEK_SET);
        FileUtils::readBytes(file, (char *) code, codeSize);

        if (capstoneArchOption == CS_ARCH_X86) {
                Disassemblex86(&code, codeSize, fileOffset);
                return;
        }

        if (capstoneArchOption == CS_ARCH_ARM) {
                capstoneModeOption = getCapstoneMode(fileOffset);
                cs_option(capstoneHandle, CS_OPT_MODE, capstoneModeOption);
                if (capstoneModeOption == CS_MODE_THUMB)
                        printf ("thumb ----\n");
                else
                        printf("arm ---------\n");

                DisassembleARM(&code, codeSize, fileOffset);
        }


}

void FileReader::Disassemble(uint64_t fileOffset, uint64_t size)
{

        const uint8_t *code;
        csh handle;

        size_t count, code_size;
        uint64_t nextOffset;
        uint64_t initialFileOffset = fileOffset;

        code = new uint8_t[size];
        fseek(file, fileOffset, SEEK_SET);
        FileUtils::readBytes(file, (char *) code, size);

        if (capstoneArchOption == CS_ARCH_X86) {
                Disassemblex86(&code, size, fileOffset);
                return;
        }

        if (capstoneArchOption == CS_ARCH_ARM) {

                while(true) {
                        capstoneModeOption = getCapstoneMode(fileOffset);
                        cs_option(capstoneHandle, CS_OPT_MODE, capstoneModeOption);
                        if (capstoneModeOption == CS_MODE_THUMB)
                                printf ("thumb ----\n");
                        else
                                printf("arm ---------\n");

                        nextOffset = getNextSymbolOffset(fileOffset);
                        if (nextOffset == 0) {
                                printf ("size = 0x%llx\n", initialFileOffset + size - fileOffset);
                                DisassembleARM(&code, initialFileOffset + size - fileOffset, fileOffset);
                                break;
                        }
                        printf ("size = 0x%llx\n", nextOffset - fileOffset);

                        DisassembleARM(&code, nextOffset - fileOffset, fileOffset);

                        fileOffset = nextOffset;
                }

        }
}

cs_mode FileReader::getCapstoneMode(uint64_t fileOffset)
{
        if (functionStartsMap.find(fileOffset) != functionStartsMap.end())
                return CS_MODE_ARM;

        if (functionStartsMap.find(fileOffset + 1) != functionStartsMap.end())
                return CS_MODE_THUMB;

        //TODO throw exception

        printf("something went wrong %llx\n", fileOffset);



}

uint64_t FileReader::getNextSymbolOffset(uint64_t currentOffset)
{
        uint32_t index;

        for (index = 0; index < functionsOffset.size(); index++) {
                if (functionsOffset[index] == currentOffset) {
                        if (index < functionsOffset.size() - 1) {
                                printf("returned offset 0x%llx\n", functionsOffset[index + 1]);
                                return functionsOffset[index + 1];
                        }
                        else {
                                printf("returned offset 0\n");
                                return 0;
                        }
                }
        }

        printf("returned offset 0 2\n");
        return 0;
}
void FileReader::Disassemblex86(const uint8_t **code, uint64_t size,
                        uint64_t startAddress)
{
        cs_insn *insn;
        size_t codeSize;
        uint64_t address;

        codeSize = size;
        address = startAddress;

        insn = cs_malloc(capstoneHandle);

        while (cs_disasm_iter(capstoneHandle, code, &codeSize, &address, insn)) {
                printf("0x%"PRIx64":\t%s\t\t%s\n", insn[0].address, insn[0].mnemonic, insn[0].op_str);
        }

        cs_free(insn, 1);
}

void FileReader::DisassembleARM(const uint8_t **code, uint64_t size,
                        uint64_t startAddress)
{
        cs_insn *insn;
        size_t codeSize;
        uint64_t address;
        const uint8_t *initialCode = *code;

        codeSize = size;
        address = startAddress;

        insn = cs_malloc(capstoneHandle);
        while(true) {
                while (cs_disasm_iter(capstoneHandle, code, &codeSize, &address, insn)) {
                        printf("0x%"PRIx64":\t%s\t\t%s\n", insn[0].address, insn[0].mnemonic, insn[0].op_str);
                }

                if (codeSize > 0) {
                        printf("fuck this shit 0x%llx\n", address);
                        address = startAddress + size;
                        *code = initialCode + size;
                        codeSize = 0;
                }
                else
                        break;
                }

        cs_free(insn, 1);
}

FileReader::~FileReader()
{
        cs_close(&capstoneHandle);
}
