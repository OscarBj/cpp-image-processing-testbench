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

            buffers[i] = vector<unsigned int>(data.begin() + j, data.begin() + (j + (width-1)));
            // buffers[i] = vector<unsigned int>(width,255);
        }
    }
    else { // MIDDLE
        int j = 0;
        for( int i = 0; i < (kernelSize*2)+1 ; i++ ) {

            j = ((row_idx - kernelSize) + i)*width;

            buffers[i] = vector<unsigned int>(data.begin() + j, data.begin() + (j + (width-1)));
            // buffers[i] = vector<unsigned int>(width,0);

        }
    }
    
    return buffers;
}

vector<unsigned int> getRectWindow(unsigned int px_idx, unsigned int width, unsigned int height, unsigned int kernelSize, vector<vector<unsigned int>> rowBuffers) {
    
    // Create rectangular window with kernelSize as radius
    vector<unsigned int> window(((kernelSize*2)+1)*((kernelSize*2)+1), 0);
    
    if(px_idx < kernelSize)
    {   int j;
        for(int i = 0; i < (kernelSize*2)+1; i++)
        {
            for(j = 0; j < (kernelSize*2)+1; j++)
            {
                window[(i*((kernelSize*2)+1)) + j] = rowBuffers[i][j];
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
                window[(i*((kernelSize*2)+1)) + j] = rowBuffers[i][jj];
            }
        }
    }
    else {
        int jj;
        for(int i = 0; i < (kernelSize*2)+1; i++)
        {
            for(int j = 0; j < (kernelSize*2)+1; j++)
            {
                
                jj = px_idx - kernelSize + j;

                window[(i*((kernelSize*2)+1)) + j] = rowBuffers[i][jj];

            }
        }
    }

    return window;
}

vector<vector<unsigned int>> boxFilt(unsigned int channel, unsigned int width, unsigned int height, unsigned int kernelSize, vector<vector<unsigned int>> data)
{
    vector<unsigned int> ch_data(width*height,0);
    for(int i = 0; i < data.size(); i++)
    {
        ch_data[i] = data[i][channel]; // Operate on 1 channel;
    }
    
    vector<vector<unsigned int>> odata(data.size(),{0,0,0});

    vector<vector<unsigned int>> rowBuffers;
    vector<unsigned int> window;

    int avg;
    for(int i = 0; i < height; i++)
    {
        rowBuffers = getRowBuffers(i, width, height, kernelSize, ch_data);

        for(int j = 0; j < width; j++)
        {
            window = getRectWindow(j, width, height, kernelSize, rowBuffers);
            avg = (unsigned int) accumulate(window.begin(), window.end(), 0.0) / window.size();

            odata[(i*width)+(j)][0] = avg;
            odata[(i*width)+(j)][1] = avg;
            odata[(i*width)+(j)][2] = avg;
        }
    }

    return odata;
}

vector<vector<unsigned int>> bufferTest(unsigned int channel, unsigned int width, unsigned int height, unsigned int kernelSize, vector<vector<unsigned int>> data)
{
    vector<unsigned int> ch_data(width*height,0);
    for(int i = 0; i < data.size(); i++)
    {
        ch_data[i] = data[i][channel]; // Operate on 1 channel;
    }
    
    vector<vector<unsigned int>> rowBuffers;
    vector<unsigned int> window;

    int windowErrors = 0;
    int rowBufferErrors = 0;
    for(int i = 0; i < height; i++)
    {
        rowBuffers = getRowBuffers(i, width, height, kernelSize, ch_data);
        
        
        for (int ij = 0; ij < width; ij++) {
            
            if(i < kernelSize) rowBufferErrors = data[ij][channel] == rowBuffers[0][ij] ? rowBufferErrors : rowBufferErrors + 1;
            
            else if (i + kernelSize > (height-1)) rowBufferErrors = data[((height-1)*width)+(ij)][channel] == rowBuffers[kernelSize*2][ij] ? rowBufferErrors : rowBufferErrors + 1;

            else rowBufferErrors = data[(i*width)+(ij)][channel] == rowBuffers[kernelSize][ij] ? rowBufferErrors : rowBufferErrors + 1;
            
        }

        for(int j = 0; j < width; j++)
        {
            // Generate window
            window = getRectWindow(j, width, height, kernelSize, rowBuffers);

            // Data mismatch
            if(data[(i*width)+(j)][channel] != window[(((kernelSize*2)+1)*((kernelSize*2)+1)-1)/2]) {
                windowErrors = windowErrors + 1;
                // Set pixel red
                data[(i*width)+(j)][0] = 255;
                data[(i*width)+(j)][1] = 0;
                data[(i*width)+(j)][2] = 0;  
            }   
        }
    }
    
    cout << "      Row buffer Errors found: " << rowBufferErrors << endl;
    
    cout << "   Kernel buffer Errors found: " << windowErrors << endl;
    return data;
}