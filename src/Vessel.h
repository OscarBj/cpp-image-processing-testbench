#ifndef VESSEL_H
#define VESSEL_H
#include <functional>   /*  uint8_t    */
#include <vector>

#include "ImageTypes.h"

typedef enum {
    HIGH, LOW
} BowType;

typedef struct {
    uint8_t x1;
    uint8_t y1;
    uint8_t x2;
    uint8_t y2;
} HullDimensions;

typedef struct {
    uint8_t x1;
    uint8_t y1;
    uint8_t x2;
    uint8_t y2;
} BridgeDimensions;

typedef struct {
    HullDimensions hull;
    BowType bowType;
    BridgeDimensions bridge;
    int bowSize;
} VesselDescriptor;

VesselDescriptor generateVesselDescriptor(TypeDescriptorConstraints constraints);

int getWidth(HullDimensions hull);

int getHeight(HullDimensions hull);

int isInsideVesselHull(int x, int y, VesselDescriptor descriptor);

int isInsideVesselBridge(int x, int y, VesselDescriptor descriptor);

// class Vessel {

    // public:

        // Vessel(int width, int height);
// };

#endif