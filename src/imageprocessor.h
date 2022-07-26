#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <vector>

std::vector<std::vector<unsigned int>> transformPxChVal(unsigned int channels, unsigned int from, unsigned int to, std::vector<std::vector<unsigned int>> data);

std::vector<std::vector<unsigned int>> avgFilt(unsigned int channel, unsigned int width, unsigned int height, unsigned int kernelSize, std::vector<std::vector<unsigned int>> data);

#endif