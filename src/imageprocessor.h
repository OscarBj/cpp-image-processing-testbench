#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <vector>

void transformPxChVal(unsigned int channels, unsigned int from, unsigned int to, std::vector<std::vector<unsigned int>>& data);

void boxFilt(unsigned int width, unsigned int height, unsigned int kernelSize, std::vector<std::vector<unsigned int>>& data);

std::vector<std::vector<unsigned int>> bufferTest(unsigned int width, unsigned int height, unsigned int kernelSize, std::vector<std::vector<unsigned int>>& data);

void ccl(unsigned int channel, unsigned int threshold, unsigned int width, unsigned int height, std::vector<std::vector<unsigned int>>& data);

#endif