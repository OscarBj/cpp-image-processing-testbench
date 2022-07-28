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

    int ii;
    for( int i = 0; i < (kernelSize*2)+1 ; i++ ) {
        ii = row_idx - kernelSize + i;
     
        for (int j = 0; j < width; j++) {
            if(row_idx + kernelSize - i > (height-1)) { // For rows above height
                buffers[i][j] = 0;
            }
            if(row_idx + i < kernelSize) { // For rows below height
                buffers[i][j] = 0;
            }
            else buffers[i][j] = data[(ii*width) + j]; // Middle
        }
    }
    return buffers;
}

vector<unsigned int> getRectWindow(unsigned int px_idx, unsigned int width, unsigned int height, unsigned int kernelSize, vector<vector<unsigned int>> rowBuffers) {
    
    // Create rectangular window with kernelSize as radius
    vector<unsigned int> window(((kernelSize*2)+1)*((kernelSize*2)+1), 0);

    int jj;
    for(int i = 0; i < (kernelSize*2)+1; i++)
    {
        for(int j = 0; j < (kernelSize*2)+1; j++)
        {
            
            jj = px_idx - kernelSize + j;

            if(px_idx + kernelSize - i > (width-1)) { // For columns larger than width
                window[(i*((kernelSize*2)+1)) + j] = 0;
            }
            if(px_idx + i < kernelSize) { // For columns smaller than width
                window[(i*((kernelSize*2)+1)) + j] = 0;
            }

            window[(i*((kernelSize*2)+1)) + j] = rowBuffers[i][jj];

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
            // if(i < 5) cout << avg;
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
    vector<vector<unsigned int>> odata(data.size(),{0,0,0});

    vector<vector<unsigned int>> rowBuffers;
    vector<unsigned int> window;

    int windowErrors = 0;
    int rowBufferErrors = 0;

    int windowCenter = (((kernelSize*2)+1)*((kernelSize*2)+1)-1)/2;

    bool rbfe;
    for(int i = 0; i < height; i++)
    {
        rowBuffers = getRowBuffers(i, width, height, kernelSize, ch_data);

        rbfe = false;
        for (int ij = 0; ij < width; ij++) {

            // if(i < kernelSize) rowBufferErrors = data[ij][channel] == rowBuffers[0][ij] ? rowBufferErrors : rowBufferErrors + 1;

            // else if (i + kernelSize > (height-1)) rowBufferErrors = data[((height-1)*width)+(ij)][channel] == rowBuffers[kernelSize*2][ij] ? rowBufferErrors : rowBufferErrors + 1;

            // else 
            if(data[(i*width)+(ij)][channel] != rowBuffers[kernelSize][ij]) {
                rowBufferErrors = rowBufferErrors + 1;
                rbfe = true;
            }
        }

        for(int j = 0; j < width; j++)
        {
            // Generate window
            window = getRectWindow(j, width, height, kernelSize, rowBuffers);

            // Data mismatch
            if(data[(i*width)+(j)][channel] != window[windowCenter]) {
                windowErrors = windowErrors + 1;
                // // Set pixel red
                // data[(i*width)+(j)][0] = window[(((kernelSize*2)+1)*((kernelSize*2)+1)-1)/2];
                // data[(i*width)+(j)][1] = window[(((kernelSize*2)+1)*((kernelSize*2)+1)-1)/2];
                // data[(i*width)+(j)][2] = window[(((kernelSize*2)+1)*((kernelSize*2)+1)-1)/2];
                odata[(i*width)+(j)][0] = 255;
                odata[(i*width)+(j)][1] = 0;
                odata[(i*width)+(j)][2] = 0;  
            } else if(rbfe) { // Buffer row error as blue
                odata[(i*width)+(j)][0] = 0;
                odata[(i*width)+(j)][1] = 0;
                odata[(i*width)+(j)][2] = 255; 
            } else { // OK
                odata[(i*width)+(j)][0] = window[windowCenter];
                odata[(i*width)+(j)][1] = window[windowCenter];
                odata[(i*width)+(j)][2] = window[windowCenter];
            }
        }
    }

    cout << "      Row buffer Errors found: " << rowBufferErrors << endl;

    cout << "   Kernel buffer Errors found: " << windowErrors << endl;
    return odata;
}

// TODO
vector<vector<unsigned int>> ccl(unsigned int channel, unsigned int threshold, unsigned int width, unsigned int height, unsigned int kernelSize, vector<vector<unsigned int>> data)
{
    vector<unsigned int> ch_data(width*height,0);
    for(int i = 0; i < data.size(); i++)
    {
        ch_data[i] = data[i][channel] >= threshold ? data[i][channel] : 0; // Operate on 1 channel & apply threshold;
    }

    vector<vector<unsigned int>> odata(data.size(),{0,0,0});

    vector<vector<unsigned int>> rowBuffers;
    vector<unsigned int> window;

    vector<unsigned int> labels(255,0);
    vector<unsigned int> features(255,0);

    int avg;
    for(int i = 0; i < height; i++)
    {
        rowBuffers = getRowBuffers(i, width, height, kernelSize, ch_data);

        for(int j = 0; j < width; j++)
        {
            window = getRectWindow(j, width, height, kernelSize, rowBuffers);

            // odata[(i*width)+(j)][0] = avg;
            // odata[(i*width)+(j)][1] = avg;
            // odata[(i*width)+(j)][2] = avg;
        }
    }

    return odata; 
}