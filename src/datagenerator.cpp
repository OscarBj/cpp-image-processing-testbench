#include <stdlib.h>   /* srand, rand */
#include <functional> /*  uint8_t    */

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
            if (isInsideVesselHull(x, y, descriptor) || isInsideVesselBridge(x, y, descriptor))
            {
                data[(y * sampleWidth) + x][0] = 255;
                data[(y * sampleWidth) + x][1] = 255;
                data[(y * sampleWidth) + x][2] = 255;
            }
        }
    }

    writeBMP(out, sampleWidth, sampleHeight, header, data);
}