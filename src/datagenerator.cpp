#include <stdlib.h>     /* srand, rand */
#include <functional>   /*  uint8_t    */
#include "filehandler.h"

using namespace std;

void generateVesselData(unsigned int nrOfSamples, int sampleWidth, int sampleHeight) {

    int length, length_x1, length_x2, hull_x1, hull_x2, hull_height, bridge_height, bow_type, bridge_offset, t; 

    int widthInBytes = sampleWidth * 3;
    int paddingSize = (4 - (widthInBytes) % 4) % 4;
    int stride = (widthInBytes) + paddingSize;

    int fileSize = 54 + sampleHeight*stride;

    vector<vector<uint8_t>> data(sampleWidth*sampleHeight,{0,0,0});

    char *out = (char*) "outputs/generated/vessel.bmp";
    unsigned char *header;
    header = new unsigned char[54];

    header[ 0] = (unsigned char)('B');
    header[ 1] = (unsigned char)('M');

    header[ 2] = (unsigned char)(fileSize);
    header[ 3] = (unsigned char)(fileSize>> 8);
    header[ 4] = (unsigned char)(fileSize>>16);
    header[ 5] = (unsigned char)(fileSize>>24);

    header[ 6] = (unsigned char)(0);
    header[ 7] = (unsigned char)(0);
    header[ 8] = (unsigned char)(0);
    header[ 9] = (unsigned char)(0);
    
    header[10] = (unsigned char)(54);
//
    header[40] = (unsigned char)(       40    );
    header[44] = (unsigned char)(       sampleWidth    );
    header[45] = (unsigned char)(       sampleWidth>> 8);
    header[46] = (unsigned char)(       sampleWidth>>16);
    header[47] = (unsigned char)(       sampleWidth>>24);
    header[48] = (unsigned char)(       sampleHeight    );
    header[49] = (unsigned char)(       sampleHeight>> 8);
    header[50] = (unsigned char)(       sampleHeight>>16);
    header[51] = (unsigned char)(       sampleHeight>>24);
    header[52] = (unsigned char)(       1);
    header[54] = (unsigned char)(       24);


    for(int sampleNr = 0; sampleNr < nrOfSamples; sampleNr++) {

        length_x1          = rand() % (int) ((0.1*sampleWidth) + (0.4*sampleWidth));
        length_x2          = rand() % (int) ((0.6*sampleWidth) + (0.9*sampleWidth));
        length = length_x2 - length_x1;

        hull_x1   = rand() % (int) ((0.1*sampleHeight) + (0.4*sampleHeight));
        hull_x2   = rand() % (int) ((0.5*sampleHeight) + (0.7*sampleHeight));
        hull_height = hull_x2 - hull_x1;

        // bridge_height   = rand() % (deck_height) + (0.1*deck_height);;
        // bow_type        = rand() % 3 + 1;
        // bridge_offset   = rand() % 3 + 1;

        for(int y = 0; y < sampleHeight; y++) {
            for(int x = 0; x < sampleWidth; x++) {

            }
        }

        writeBMP(out, sampleWidth, sampleHeight, header, data);
    }
}