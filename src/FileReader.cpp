#include "FileReader.hpp"

/*generate constants for capstone configurations*/
std::map<uint32_t, cs_arch> FileReader::makeCapstoneArch()
{
        std::map<uint32_t, cs_arch> map;
        map[CPU_TYPE_I386] = CS_ARCH_X86;
        map[CPU_TYPE_X86_64] = CS_ARCH_X86;
        map[CPU_TYPE_ARM] = CS_ARCH_ARM;

        return map;
}
std::map<uint32_t, cs_arch> FileReader::capstoneArch = FileReader::makeCapstoneArch();


FileReader::FileReader(MachO *binary)
{
        Section *sec;
        uint64_t offset;

        this->binary = binary;


        /*decide on capstone mode and arch  based on the mach header*/
        MachHeader header = this->binary->getHeader();
        if (capstoneArch.find(header.getCpuType()) == capstoneArch.end()) {
                throw std::invalid_argument("cpu type not supported");
        }

        /*get the map with the starts offset of each function*/
        functionStartsMap = binary->getFunctionsOffset();
        file = fopen(this->binary->getFileName(), "rb");

        capstoneArchOption = capstoneArch[header.getCpuType()];
        capstoneModeOption = CS_MODE_THUMB;
        if (header.getIs32()) {
                if (capstoneArchOption == CS_ARCH_X86)
                        capstoneModeOption = CS_MODE_32;
                }
        else {
                if (capstoneArchOption == CS_ARCH_X86)
                        capstoneModeOption = CS_MODE_64;
                }

        if (cs_open(capstoneArchOption, capstoneModeOption, &capstoneHandle) != CS_ERR_OK) {
                throw std::runtime_error("error opening capstone");
        }

        /*insert in the functions offset the end of the __text section*/
        sec = binary->getSectionByIndex(1);
        offset = sec->getOffset() + sec->getSize();
        functionsOffset.push_back(offset);

        std::map<uint64_t, char *>::iterator it;

        /*extract only the functions offsets*/
        /*computes the name offset map*/
        for (it = functionStartsMap.begin(); it != functionStartsMap.end(); ++it) {
                if (it->first % 2) {
                        if (it->first - 1 < offset) {
                                functionsOffset.push_back(it->first - 1);
                                functionNamesMap[it->second] = it->first - 1;
                        }
                }
                else {
                        if (it->first < offset) {
                                functionsOffset.push_back(it->first);
                                functionNamesMap[it->second] = it->first;
                        }
                }
        }

        std::sort(functionsOffset.begin(), functionsOffset.end());

}

/*Disassemble all code*/
void FileReader::Disassemble()
{
        Section *sec;
        Segment *seg;
        uint64_t offset;

        /*get the size of the code*/
        sec = binary->getSectionByIndex(1);
        seg = binary->getSegmentByName(sec->getSegmentName());

        offset = seg->getFileOffset() + sec->getOffset();
        printf("%s %s\n", sec->getSegmentName(), sec->getSectionName());
        printf("%llu %llu\n", offset, sec->getSize());

        DisassembleAll(offset, sec->getSize());
}

/*Disassemble a function given by name*/
void FileReader::Disassemble(char *functionName)
{
        uint64_t fileOffset, nextOffset, codeSize;

        /*get the function offset*/
        if (functionNamesMap.find(functionName) != functionNamesMap.end()) {
                fileOffset = functionNamesMap[functionName];
        }
        else {
                printf("function %s not found\n", functionName);
                return;
        }

        /*get the next function offset and compute size*/
        nextOffset = getNextOffset(fileOffset);
        codeSize = nextOffset - fileOffset;
        Disassemble(fileOffset, codeSize);

}

/*Disassemble the function in which the fileOffset param falls*/
void FileReader::Disassemble(uint64_t fileOffset)
{
        uint64_t begin, end;

        if (!getFunctionBoundaries(fileOffset, &begin, &end)) {
                printf("invalid offset %llx\n", fileOffset);
                return;
        }
        Disassemble(begin, end - begin);

}

/*helper function*/
/*Disassemble the code from fileOffset through fileOffset + codeSize*/
void FileReader::Disassemble(uint64_t fileOffset, uint64_t codeSize)
{
        const uint8_t *code, *initialCode;

        /*read the raw data*/
        code = new uint8_t[codeSize];
        initialCode = code;
        fseek(file, fileOffset, SEEK_SET);
        FileUtils::readBytes(file, (char *) code, codeSize);

        /*based on the binary type call corresponding helper function*/
        if (capstoneArchOption == CS_ARCH_X86) {
                printf("%s\n", functionStartsMap[fileOffset]);
                Disassemblex86(&code, codeSize, fileOffset, false);
                return;
        }

        if (capstoneArchOption == CS_ARCH_ARM) {
                capstoneModeOption = getCapstoneMode(fileOffset);
                cs_option(capstoneHandle, CS_OPT_MODE, capstoneModeOption);
                if (capstoneModeOption == CS_MODE_THUMB)
                        printf("%s\n", functionStartsMap[fileOffset + 1]);
                else
                        printf("%s\n", functionStartsMap[fileOffset]);

                DisassembleARM(&code, codeSize, fileOffset);
        }

        delete initialCode;
}

/*DisassembleAll the code from fileOffset onwards*/
void FileReader::DisassembleAll(uint64_t fileOffset, uint64_t size)
{

        const uint8_t *code, *initialCode;
        uint64_t nextOffset;

        code = new uint8_t[size];
        initialCode = code;
        fseek(file, fileOffset, SEEK_SET);
        FileUtils::readBytes(file, (char *) code, size);

        if (capstoneArchOption == CS_ARCH_X86) {
                Disassemblex86(&code, size, fileOffset, true);
                return;
        }

        if (capstoneArchOption == CS_ARCH_ARM) {

                while(true) {
                        nextOffset = getNextOffset(fileOffset);
                        if (nextOffset == 0) {
                                break;
                        }

                        capstoneModeOption = getCapstoneMode(fileOffset);
                        cs_option(capstoneHandle, CS_OPT_MODE, capstoneModeOption);
                        if (capstoneModeOption == CS_MODE_THUMB)
                                printf("%s\n", functionStartsMap[fileOffset + 1]);
                        else
                                printf("%s\n", functionStartsMap[fileOffset]);

                        DisassembleARM(&code, nextOffset - fileOffset, fileOffset);

                        fileOffset = nextOffset;
                }

        }

        delete initialCode;
}

/*decides the capstoneMode based on the offset*/
/*even --> CS_MODE_ARM uneven --> CS_MODE_THUMB*/
cs_mode FileReader::getCapstoneMode(uint64_t fileOffset)
{
        if (functionStartsMap.find(fileOffset) != functionStartsMap.end())
                return CS_MODE_ARM;

        if (functionStartsMap.find(fileOffset + 1) != functionStartsMap.end())
                return CS_MODE_THUMB;

        //TODO throw exception

        printf("something went wrong %llx\n", fileOffset);
        return CS_MODE_THUMB;



}

/*gets the next function offset*/
uint64_t FileReader::getNextOffset(uint64_t currentOffset)
{
        uint32_t index;

        for (index = 0; index < functionsOffset.size(); index++) {
                if (functionsOffset[index] == currentOffset) {
                        if (index < functionsOffset.size() - 1)
                                return functionsOffset[index + 1];
                        else
                                return 0;
                }
        }

        return 0;
}

/*gets the fuction boundaries for a given offset*/
bool FileReader::getFunctionBoundaries(uint64_t offset, uint64_t *begin,
                                uint64_t *end)
{
        uint32_t index;

        if (offset < functionsOffset[0])
                return false;

        for (index = 0; index < functionsOffset.size(); index++) {
                if (functionsOffset[index] >= offset) {
                        *begin = functionsOffset[index - 1];
                        *end = functionsOffset[index];
                        return true;
                }
        }

        return false;
}

void FileReader::Disassemblex86(const uint8_t **code, uint64_t size,
                        uint64_t startAddress, bool print)
{
        cs_insn *insn;
        size_t codeSize;
        uint64_t address;

        codeSize = size;
        address = startAddress;

        insn = cs_malloc(capstoneHandle);
        if (functionStartsMap.find(address) != functionStartsMap.end() && print) {
                printf("%s\n", functionStartsMap[address]);
        }
        while (cs_disasm_iter(capstoneHandle, code, &codeSize, &address, insn)) {
                printf("0x%" PRIx64 ":\t%s\t\t%s\n", insn[0].address, insn[0].mnemonic, insn[0].op_str);

                if (functionStartsMap.find(address) != functionStartsMap.end() && print) {
                        printf("%s\n", functionStartsMap[address]);
                }
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

        printf("code %lu\n", codeSize);
        insn = cs_malloc(capstoneHandle);
        while(true) {
                while (cs_disasm_iter(capstoneHandle, code, &codeSize, &address, insn)) {
                        printf("0x%" PRIx64 ":\t%s\t\t%s\n", insn[0].address, insn[0].mnemonic, insn[0].op_str);
                }

                /*error probably define instructions --> skip to the end*/
                if (codeSize > 0) {
                        printf("skipping from 0x%llx %lu\n bytes", address, codeSize);
                        address = startAddress + size;
                        *code = initialCode + size;
                        codeSize = 0;
                        break;
                }
                else
                        break;
        }

        cs_free(insn, 1);
}

char * FileReader::dumpSection(char *segmentName, char *sectionName, uint64_t *size)
{
        Section *sec;
        char *raw;

        sec = this->binary->getSectionByName(segmentName, sectionName);

        if (sec == NULL) {
                printf("404 not found\n");
                return NULL;
        }

        raw = new char[sec->getSize()];
        fseek(this->file, sec->getOffset(), SEEK_SET);
        FileUtils::readBytes(this->file, raw, sec->getSize());
        *size = sec->getSize();

        return raw;
}

FileReader::~FileReader()
{
        cs_close(&capstoneHandle);
        fclose(file);
        fflush(stdout);
}
