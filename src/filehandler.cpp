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
    int height = *(int*)&header[22];

    cout << "   File: " << inp_filename << endl;
    cout << "  Width: " << width << endl;
    cout << " Height: " << height << endl;
    cout << endl;

    int row_padded = (width*3 + 3) & (~3);

    int size = width * abs(height);

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
    delete row;

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

void BMPFilehandler::WriteBMP(int width, int height, vector<vector<unsigned int>> data)
{
    FILE* imageFile = fopen(outp_filename, "wb");

    cout << "   File: " << outp_filename << endl;

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