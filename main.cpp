#include <stdio.h>
#include <stdint.h>
#include "MachHeader.hpp"
#include "Segment32.hpp"
#include "Segment64.hpp"


void print_header(MachHeader header)
{
        if (header.getMagic() == MAGIC_32)
                printf("MACH-O 32\n");
        else
                printf("MACH-O 64\n");

        printf("cputype:%d\n subtype:%x\n filetype:%x\n ncmds:%d\n size:%d\n flags:%x\n", header.getCpuType(),
                        header.getCpuSubType(), header.getFileType(), header.getNumberCmds(), header.getSizeOfCmds(), header.getFlags());

        if (header.getMagic() == MAGIC_64)
                printf("reserved:%x\n", header.getReserved());
}

int main(int argc, char *argv[])
{
        FILE *file;

        if (argc != 2) {
                printf("Usage: %s <binary>\n", argv[0]);
                return -1;
        }

        file = fopen(argv[1], "rb");
        if(!file) {
                printf("file not opened");
                return 0;
        }

        MachHeader header(file);
        print_header(header);

        Segment *seg;
        uint32_t command;
        fread(&command, sizeof(uint32_t), 1, file);
        if(header.getIs32())
                seg = new Segment32(file);
        else
                seg = new Segment64(file);
        printf("name=%s\n", seg->getName());

        printf("vmsize=%llu\n", seg->getVirtualSize());
        printf("filesize=%llu\n", seg->getFileSize());
        printf("fileoff=%llu\n", seg->getFileOffset());



        return 0;
}
