#include "Vessel.h"

int getHullHeight(VesselHull hull) {
    return hull.y2 - hull.y1;
}

int getHullWidth(VesselHull hull) {
    return hull.x2 - hull.x1;
}


VesselDescriptor generateVesselDescriptor(TypeDescriptorConstraints constraints) {
    VesselDescriptor descriptor;

    srand(time(nullptr));
    double x11 = (0.1 * constraints.frameWidth);
    double x12 = (0.3 * constraints.frameWidth);
    double x21 = (0.8 * constraints.frameWidth);
    double x22 = (0.9 * constraints.frameWidth);

    double y11 = (0.1 * constraints.frameHeight);
    double y12 = (0.3 * constraints.frameHeight);
    double y21 = (0.5 * constraints.frameHeight);
    double y22 = (0.6 * constraints.frameHeight);

    descriptor.hull.x1 = x11 + (rand() % (int)(x12 - x11));
    descriptor.hull.x2 = x21 + (rand() % (int)(x22 - x21));
    descriptor.hull.y1 = y11 + (rand() % (int)(y12 - y11));
    descriptor.hull.y2 = y21 + (rand() % (int)(y22 - y21));

    // cout << "  X1: " << hull_x1 << endl;

    // cout << "  X2: " << hull_x2 << endl;

    // cout << "  Y1: " << hull_y1 << endl;

    // cout << "  Y2: " << hull_y2 << endl;

    return descriptor;
}

int isInsideVessel(int x, int y, VesselDescriptor descriptor) {
    return x >= descriptor.hull.x1 && x <= descriptor.hull.x2 && y >= descriptor.hull.y1 && y <= descriptor.hull.y2;
}