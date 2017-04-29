#include "MachO.hpp"

MachO::MachO(char *fileName)
{
        uint32_t command, index;

        FILE *file;

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

                        /*parsing not yet implemented - -skip*/
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

MachO::~MachO()
{
        int index;

        for(index = 0; index < segments.size(); index++){
                delete segments[index];
        }
}
