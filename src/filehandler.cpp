#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>

#include "filehandler.h"

using namespace std;


BMPFilehandler::BMPFilehandler(char *infl, char *outfl)
{

    inp_filename    = infl;
    outp_filename   = outfl;
    header = new unsigned char[54];

}

vector<vector<unsigned int>> BMPFilehandler::ReadBMP()
{
    FILE* f = fopen(inp_filename, "rb");

    if(f == NULL) {
        cout << "   ERROR: Invalid input file" << endl << endl;
        exit(0);
    }

    fread(header, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    int width = *(int*)&header[18];
    int height = abs(*(int*)&header[22]);

    int row_padded = (width*3 + 3) & (~3);

    int size = width * height;

    vector<vector<unsigned int>> data(size,{0,0,0});

    unsigned char* row = new unsigned char[row_padded];

    unsigned char tmp;

    int r,g,b;

    for(int i = 0; i < height; i++)
    {
        fread(row, sizeof(unsigned char), row_padded, f);
        for(int j = 0; j < width; j++)
        {
            // Convert (B, G, R) to (R, G, B)
            r = row[(j*3)+2];
            g = row[(j*3)+1];
            b = row[(j*3)];

            data[(i*width)+(j)][0] = r;
            data[(i*width)+(j)][1] = g;
            data[(i*width)+(j)][2] = b;
        }
    }
    delete[] row;

    fclose(f);
    return data;
}

unsigned char* BMPFilehandler::ReadHeader() {
    int i;
    FILE* f = fopen(inp_filename, "rb");

    if(f == NULL)
        throw "Argument Exception";

    unsigned char* info = new unsigned char[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header
    fclose(f);
    return info;
}

void BMPFilehandler::WriteBMP(int width, int height, vector<vector<unsigned int>>& data)
{
    FILE* imageFile = fopen(outp_filename, "wb");

    int row_padded = (width*3 + 3) & (~3);

    int widthInBytes = width * 8;
    int fileSize = 54 + (row_padded * height);

    fwrite(header, 1, 54, imageFile);

    unsigned char tmp[3] = { '0' };

    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            // Convert (R, G, B) to (B, G, R)
            tmp[2] = data[(i*width)+(j)][0];
            tmp[1] = data[(i*width)+(j)][1];
            tmp[0] = data[(i*width)+(j)][2];

            fwrite(tmp, sizeof(unsigned char), 3, imageFile);
        }
    }

    fclose(imageFile);
}

void writeBMP(char *outp_filename, int width, int height, unsigned char *header, vector<vector<uint8_t>>& data)
{
    FILE* imageFile = fopen(outp_filename, "wb");

    int row_padded = (width*3 + 3) & (~3);

    int widthInBytes = width * 8;
    int fileSize = BMP_HEADER_SIZE + (row_padded * height);

    fwrite(header, 1, BMP_HEADER_SIZE, imageFile);

    unsigned char tmp[3] = { '0' };

    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            // Convert (R, G, B) to (B, G, R)
            tmp[2] = data[(i*width)+(j)][0];
            tmp[1] = data[(i*width)+(j)][1];
            tmp[0] = data[(i*width)+(j)][2];

            fwrite(tmp, sizeof(unsigned char), 3, imageFile);
        }
    }

    fclose(imageFile);
}

unsigned char *generateBMPHeader(TypeDescriptorConstraints constraints) {
    int widthInBytes = constraints.frameWidth * 3;
    int paddingSize = (4 - (widthInBytes) % 4) % 4;
    int stride = (widthInBytes) + paddingSize;

    int fileSize = BMP_HEADER_SIZE + constraints.frameHeight * stride;

    vector<vector<uint8_t>> data(constraints.frameWidth * constraints.frameHeight, {0, 0, 0});

    unsigned char *header;
    header = new unsigned char[BMP_HEADER_SIZE];

    // Hardcoded file header for 24-bit 200x200 bmp image.
    header[0] = (unsigned char)('B');
    header[1] = (unsigned char)('M');

    header[2] = (unsigned char)(fileSize);
    header[3] = (unsigned char)(fileSize >> 8);
    header[4] = (unsigned char)(fileSize >> 16);
    header[5] = (unsigned char)(fileSize >> 24);

    header[6] = (unsigned char)(0);
    header[7] = (unsigned char)(0);
    header[8] = (unsigned char)(0);
    header[9] = (unsigned char)(0);

    header[10] = (unsigned char)(54);
    header[14] = (unsigned char)(40);

    header[18] = (unsigned char)(constraints.frameWidth);  // 0xc8
    header[22] = (unsigned char)(constraints.frameHeight); // 0xc8
    header[26] = (unsigned char)(1);
    header[28] = (unsigned char)(0x18); // Bits per pixel - 24

    return header;
}