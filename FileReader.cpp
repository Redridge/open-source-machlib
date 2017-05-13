#include "FileReader.hpp"

FileReader::FileReader(MachO *binary)
{
        this->binary = binary;
        functionStartsMap = binary->getFunctionsOffset();
        printf("opened %s\n", this->binary->getFileName());
        file = fopen(this->binary->getFileName(), "rb");
}

void FileReader::Disassemble(uint64_t fileOffset, uint64_t size)
{
        fseek(file, fileOffset, SEEK_SET);
        uint8_t *code;
        csh handle;
        cs_insn *insn;
        size_t count;

        code = new uint8_t[size];
        FileUtils::readBytes(file, (char *) code, size);

        if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK)
                printf("mare eroare\n");
                //TODO throw exception

        count = cs_disasm(handle, code, size - 1, fileOffset, 0, &insn);
        printf("count = %lu\n", count);

        if (count > 0) {
                size_t j;
                for (j = 0; j < count; j++) {
                        printf("0x%"PRIx64":\t%s\t\t%s\n", insn[j].address, insn[j].mnemonic, insn[j].op_str);
                }

        cs_free(insn, count);
        }
        else
                printf("ERROR: Failed to disassemble given code!\n");

        cs_close(&handle);
}

void FileReader::Disassemble(char *functionName)
{

}
