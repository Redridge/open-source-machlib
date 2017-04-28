#include "MachO.hpp"

MachO::MachO(char *fileName)
{
        uint32_t command, index;

        FILE *file;

        file = fopen(fileName, "rb");

        header = MachHeader(file);

        for (index = 0; index < header.getNumberCmds(); index++) {
                FileUtils::readUint32(file, &command);

                switch(command) {

                        case LC_SEGMENT32:
                                segments.push_back(new Segment32(file));
                                break;

                        case LC_SEGMENT64:
                                segments.push_back(new Segment64(file));
                                break;

                        default:
                                uint32_t size;
                                FileUtils::readUint32(file, &size);
                                fseek(file, size - 8, SEEK_CUR);
                                break;
                        }
        }

}

MachHeader MachO::getHeader()
{
        return header;
}
std::vector<Segment *>MachO::getSegments()
{
        return segments;
}
