#include <stdlib.h>   /* srand, rand */
#include <functional> /*  uint8_t    */
#include <iostream>

#include "filehandler.h"
#include "Vessel.h"

using namespace std;

void generateVesselData(unsigned int nrOfSamples, int sampleWidth, int sampleHeight)
{
    // TODO: Add support for nrOfSamples & Check resource use

    TypeDescriptorConstraints constraints;
    constraints.frameWidth = sampleWidth;
    constraints.frameHeight = sampleHeight;
    
    char *out = (char *)"outputs/generated/vessel.bmp";
    vector<vector<uint8_t>> data(sampleWidth * sampleHeight, {0, 0, 0});
    unsigned char *header = generateBMPHeader(constraints);
    
    VesselDescriptor descriptor = generateVesselDescriptor(constraints);
    
        for (int y = 0; y < sampleHeight; y++)
        {
            for (int x = 0; x < sampleWidth; x++)
            {
                if (isInsideVessel(x, y, descriptor))
                {
                    data[(y * sampleWidth) + x][0] = 255;
                    data[(y * sampleWidth) + x][1] = 255;
                    data[(y * sampleWidth) + x][2] = 255;
                
                }
                
                // Vessel hull
                // if (x >= hull_x1 && x <= hull_x2 && y >= hull_y1 && y <= hull_y2)
                // {
                //     data[(y * sampleWidth) + x][0] = 255;
                //     data[(y * sampleWidth) + x][1] = 255;
                //     data[(y * sampleWidth) + x][2] = 255;
                // }
                // Bridge
                // if (x >= (hull_x1+5) && x <= (hull_x2-5) && y >= hull_y1 && y <= (hull_y2+bridge_height))
                // {
                //     data[(y * sampleWidth) + x][0] = 255;
                //     data[(y * sampleWidth) + x][1] = 255;
                //     data[(y * sampleWidth) + x][2] = 255;
                // }
                
            }
        }

    writeBMP(out, sampleWidth, sampleHeight, header, data);
}