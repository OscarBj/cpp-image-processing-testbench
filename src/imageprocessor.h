#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <vector>

typedef struct {
    std::vector<unsigned int> labels;
    std::vector<unsigned int> features;
    std::vector<unsigned int> featureBrightness;
    std::vector<std::vector<int>> featureCoordinates;
} feature_descriptors;

void transformPxChVal(unsigned int channels, unsigned int from, unsigned int to, std::vector<std::vector<unsigned int>>& data);

void boxFilt(unsigned int width, unsigned int height, unsigned int kernelSize, std::vector<std::vector<unsigned int>>& data);

std::vector<std::vector<unsigned int>> bufferTest(unsigned int width, unsigned int height, unsigned int kernelSize, std::vector<std::vector<unsigned int>>& data);

feature_descriptors ccl(unsigned int channel, unsigned int threshold, unsigned int width, unsigned int height, std::vector<std::vector<unsigned int>>& data, bool render);

void gradientFilter(unsigned int channel, unsigned int threshold, unsigned int width, unsigned int height, std::vector<std::vector<unsigned int>>& data);

void centroid(unsigned int channel, unsigned int feature_th, unsigned int width, unsigned int height, feature_descriptors descriptors, std::vector<std::vector<unsigned int>>& data);

#endif