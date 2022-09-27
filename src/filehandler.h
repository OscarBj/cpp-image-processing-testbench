#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include <functional>   /*  uint8_t    */
#include <vector>

class BMPFilehandler {


    public:
        char *inp_filename;
        char *outp_filename;
        unsigned char *header;

        BMPFilehandler(char *infl, char *outfl);
        std::vector<std::vector<unsigned int>> ReadBMP();
        unsigned char* ReadHeader();
        void WriteBMP(int width, int height, std::vector<std::vector<unsigned int>>& data);
};

void writeBMP(char *outp_filename, int width, int height, unsigned char *header, std::vector<std::vector<uint8_t>>& data);

#endif