#include "Vessel.h"

int getHullHeight(HullDimensions hull) {
    return hull.y2 - hull.y1;
}

int getHullWidth(HullDimensions hull) {
    return hull.x2 - hull.x1;
}


VesselDescriptor generateVesselDescriptor(TypeDescriptorConstraints constraints) {
    VesselDescriptor descriptor;

    srand(time(nullptr));
    int x11 = (0.1 * constraints.frameWidth);
    int x12 = (0.3 * constraints.frameWidth);
    int x21 = (0.8 * constraints.frameWidth);
    int x22 = (0.9 * constraints.frameWidth);

    int y11 = (0.1 * constraints.frameHeight);
    int y12 = (0.3 * constraints.frameHeight);
    int y21 = (0.5 * constraints.frameHeight);
    int y22 = (0.6 * constraints.frameHeight);

    descriptor.hull.x1 = x11 + rand() % (int)(x12 - x11);
    descriptor.hull.x2 = x21 + rand() % (int)(x22 - x21);
    descriptor.hull.y1 = y11 + rand() % (int)(y12 - y11);
    descriptor.hull.y2 = y21 + rand() % (int)(y22 - y21);

    descriptor.bowType = BowType(rand()%2);
    descriptor.bowSize = descriptor.bowType == LOW ? (getHullHeight(descriptor.hull)-2) : (int)(getHullHeight(descriptor.hull)/2);

    descriptor.bridge.x1 = descriptor.hull.x1 + rand() % (int)(descriptor.hull.x2 - descriptor.hull.x1);
    descriptor.bridge.x2 = descriptor.bridge.x1 + rand() % (int)(descriptor.hull.x2 - (descriptor.bridge.x1));
    descriptor.bridge.y1 = descriptor.hull.y2;
    descriptor.bridge.y2 = (descriptor.bridge.y1+5) + rand() % (int)((constraints.frameHeight-15) - (descriptor.bridge.y1+5));

    return descriptor;
}

int isInsideBow(int x, int y, VesselDescriptor descriptor) {
    if(y < descriptor.hull.y1 || x < descriptor.hull.x1) return 0;
    return x >= (descriptor.hull.x1) + (descriptor.bowSize - (y - descriptor.hull.y1));
}

int isInsideVesselHull(int x, int y, VesselDescriptor descriptor) {
    return isInsideBow(x, y, descriptor) && x <= descriptor.hull.x2 && y <= descriptor.hull.y2;
}

int isInsideVesselBridge(int x, int y, VesselDescriptor descriptor) {
    return x >= descriptor.bridge.x1 && x <= descriptor.bridge.x2 && y >= descriptor.bridge.y1 && y <= descriptor.bridge.y2;
}