#ifndef VESSEL_H
#define VESSEL_H
#include <functional>   /*  uint8_t    */
#include <vector>

#include "ImageTypes.h"

typedef enum {
    FRONT, CENTER, BACK
} BridgeLocation;

typedef struct {
    uint8_t x1;
    uint8_t y1;
    uint8_t x2;
    uint8_t y2;
} VesselHull;

typedef struct {
    VesselHull hull;
    BridgeLocation bridgeLocation;
} VesselDescriptor;

VesselDescriptor generateVesselDescriptor(TypeDescriptorConstraints constraints);

int getWidth(VesselHull hull);

int getHeight(VesselHull hull);

int isInsideVessel(int x, int y, VesselDescriptor descriptor);

// class Vessel {

    // public:

        // Vessel(int width, int height);
// };

#endif