#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <numeric>

#include "imageprocessor.h"

using namespace std;

// Select channel, pixel value to transform, value to transform into, data 
vector<vector<unsigned int>> transformPxChVal(unsigned int channel, unsigned int from, unsigned int to, vector<vector<unsigned int>> data)
{
    for(int i = 0; i < data.size(); i++)
    {
        if(data[i][channel] == from)
        {
            data[i][channel] = to;
        }
    }
    return data;
}

vector<vector<unsigned int>> getRowBuffers(unsigned int row_idx, unsigned int width, unsigned int height, unsigned int kernelSize, vector<unsigned int> data)
{
    // Create buffers with geometry:
    //
    //      kernelSize { [ <- width -> ]
    //
    vector<vector<unsigned int>> buffers((kernelSize*2)+1, vector<unsigned int>(width, 0));

    if(row_idx < kernelSize) // BOTTOM
    {
        for( int i = 0; i < (kernelSize*2)+1 ; i++ ) {

            buffers[i] = vector<unsigned int>(data.begin() + (i*width), data.begin() + ((i+1)*width));
            // buffers[i] = vector<unsigned int>(width,255);
        }
    }
    else if (row_idx + kernelSize > (height-1)) // TOP
    {
        int j = 0;
        for( int i = 0; i < (kernelSize*2)+1 ; i++ ) {
            
            j = ((height - ((kernelSize*2)+1)) + i)*width;

            buffers[i] = vector<unsigned int>(data.begin() + j, data.begin() + (j + width));
            // buffers[i] = vector<unsigned int>(width,255);
        }
    }
    else { // MIDDLE
        int j = 0;
        for( int i = 0; i < (kernelSize*2)+1 ; i++ ) {

            j = ((row_idx - kernelSize) + i)*width;

            buffers[i] = vector<unsigned int>(data.begin() + j, data.begin() + (j + width));
            // buffers[i] = vector<unsigned int>(width,0);

        }
    }

    return buffers;
}

vector<unsigned int> getRectWindow(unsigned int px_idx, unsigned int width, unsigned int height, unsigned int kernelSize, vector<vector<unsigned int>> rowBuffers, bool extrapolate) {
    
    // Create rectangular window with kernelSize as radius
    vector<unsigned int> window(((kernelSize*2)+1)*((kernelSize*2)+1), 0);
    
    if(px_idx < kernelSize)
    {
        for(int i = 0; i < (kernelSize*2)+1; i++)
        {
            for(int j = 0; j < (kernelSize*2)+1; j++)
            {
                window[(i*((kernelSize*2)+1)) + j] = rowBuffers[i][j];
                // window[((i*(kernelSize*2)+1)) + j] = 255;
            }
        }
    }
    else if(px_idx + kernelSize > (width-1))
    {
        int jj;
        for(int i = 0; i < (kernelSize*2)+1; i++)
        {
            for(int j = 0; j < (kernelSize*2)+1; j++)
            {
                jj = (width - ((kernelSize*2)+1)) + j;
                window[(i*(kernelSize*2)+1) + j] = rowBuffers[i][jj];
                // window[((i*(kernelSize*2)+1)) + j] = 255;
            }
        }
    }
    else {
        int jj;
        for(int i = 0; i < (kernelSize*2)+1; i++)
        {
            for(int j = 0; j < (kernelSize*2)+1; j++)
            {
                jj = (px_idx - (kernelSize*2)+1) + j;
                window[(i*(kernelSize*2)+1) + j] = rowBuffers[i][jj];
                // window[((i*(kernelSize*2)+1)) + j] = 0;
            }
        }
    }

    return window;
}

vector<vector<unsigned int>> avgFilt(unsigned int channel, unsigned int width, unsigned int height, unsigned int kernelSize, vector<vector<unsigned int>> data)
{
    vector<unsigned int> ch_data(width*height,0);
    for(int i = 0; i < data.size(); i++)
    {
        ch_data[i] = data[i][channel]; // Operate on 1 channel;
    }
    
    vector<vector<unsigned int>> rowBuffers;
    vector<unsigned int> window;

    for(int i = 0; i < height; i++)
    {
        rowBuffers = getRowBuffers(i, width, height, kernelSize, ch_data);
        
        for(int j = 0; j < width; j++)
        {
            
            window = getRectWindow(j, width, height, kernelSize, rowBuffers, false);

            // Set all channels to avg from 1 channel
            data[(i*width)+(j)][0] = (unsigned int) accumulate(window.begin(), window.end(), 0.0) / window.size();
            data[(i*width)+(j)][1] = (unsigned int) accumulate(window.begin(), window.end(), 0.0) / window.size();
            data[(i*width)+(j)][2] = (unsigned int) accumulate(window.begin(), window.end(), 0.0) / window.size();
        
        }
    }

    return data;
}