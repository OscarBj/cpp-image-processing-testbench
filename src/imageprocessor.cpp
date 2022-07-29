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
    int j;
    for( int i = 0; i < (kernelSize*2)+1 ; i++ ) {
        ii = row_idx - kernelSize + i;

        for (j = 0; j < width; j++) {

            if((row_idx-kernelSize) + i > (height-1)) { // For rows above height

                buffers[i][j] = 0;

            } else if(row_idx + i < kernelSize) { // For rows below height

                buffers[i][j] = 0;
            } else {
                buffers[i][j] = data[(ii*width) + j]; // Middle

            }
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

            if(jj >= width) { // For columns larger than width
                window[(i*((kernelSize*2)+1)) + j] = 0;
            }
            else if(jj < 0) { // For columns smaller than width
                window[(i*((kernelSize*2)+1)) + j] = 0;
            }
            else {
                window[(i*((kernelSize*2)+1)) + j] = rowBuffers[i][jj];
            }
        }
    }
    return window;
}

vector<vector<unsigned int>> boxFilt(unsigned int channel, unsigned int width, unsigned int height, unsigned int kernelSize, vector<vector<unsigned int>> data)
{
    vector<unsigned int> ch0_data(width*height,0);
    vector<unsigned int> ch1_data(width*height,0);
    vector<unsigned int> ch2_data(width*height,0);
    for(int i = 0; i < data.size(); i++)
    {
        ch0_data[i] = data[i][0];
        ch1_data[i] = data[i][1];
        ch2_data[i] = data[i][2];
    }

    vector<vector<unsigned int>> odata(data.size(),{0,0,0});

    vector<vector<unsigned int>> ch0_rowBuffers;
    vector<vector<unsigned int>> ch1_rowBuffers;
    vector<vector<unsigned int>> ch2_rowBuffers;
    vector<unsigned int> ch0_window;
    vector<unsigned int> ch1_window;
    vector<unsigned int> ch2_window;

    int ch0_avg;
    int ch1_avg;
    int ch2_avg;
    for(int i = 0; i < height; i++)
    {
        ch0_rowBuffers = getRowBuffers(i, width, height, kernelSize, ch0_data);
        ch1_rowBuffers = getRowBuffers(i, width, height, kernelSize, ch1_data);
        ch2_rowBuffers = getRowBuffers(i, width, height, kernelSize, ch2_data);

        for(int j = 0; j < width; j++)
        {
            ch0_window = getRectWindow(j, width, height, kernelSize, ch0_rowBuffers);
            ch1_window = getRectWindow(j, width, height, kernelSize, ch1_rowBuffers);
            ch2_window = getRectWindow(j, width, height, kernelSize, ch2_rowBuffers);

            ch0_avg = (unsigned int) accumulate(ch0_window.begin(), ch0_window.end(), 0.0) / ch0_window.size();
            ch1_avg = (unsigned int) accumulate(ch1_window.begin(), ch1_window.end(), 0.0) / ch1_window.size();
            ch2_avg = (unsigned int) accumulate(ch2_window.begin(), ch2_window.end(), 0.0) / ch2_window.size();


            odata[(i*width)+(j)][0] = ch0_avg;
            odata[(i*width)+(j)][1] = ch1_avg;
            odata[(i*width)+(j)][2] = ch2_avg;
        }
    }

    return odata;
}

vector<vector<unsigned int>> bufferTest(unsigned int channel, unsigned int width, unsigned int height, unsigned int kernelSize, vector<vector<unsigned int>> data)
{
    vector<unsigned int> ch0_data(width*height,0);
    vector<unsigned int> ch1_data(width*height,0);
    vector<unsigned int> ch2_data(width*height,0);
    for(int i = 0; i < data.size(); i++)
    {
        ch0_data[i] = data[i][0];
        ch1_data[i] = data[i][1];
        ch2_data[i] = data[i][2];
    }
    vector<vector<unsigned int>> odata(data.size(),{0,0,0});

    vector<vector<unsigned int>> ch0_rowBuffers;
    vector<vector<unsigned int>> ch1_rowBuffers;
    vector<vector<unsigned int>> ch2_rowBuffers;
    vector<unsigned int> ch0_window;
    vector<unsigned int> ch1_window;
    vector<unsigned int> ch2_window;

    int windowErrors = 0;
    int rowBufferErrors = 0;

    int windowCenter = (((kernelSize*2)+1)*((kernelSize*2)+1)-1)/2;

    bool rbfe;
    for(int i = 0; i < height; i++)
    {
        ch0_rowBuffers = getRowBuffers(i, width, height, kernelSize, ch0_data);
        ch1_rowBuffers = getRowBuffers(i, width, height, kernelSize, ch1_data);
        ch2_rowBuffers = getRowBuffers(i, width, height, kernelSize, ch2_data);

        rbfe = false;
        for (int ij = 0; ij < width; ij++) {

            if(data[(i*width)+(ij)][0] != ch0_rowBuffers[kernelSize][ij]) {
                rowBufferErrors = rowBufferErrors + 1;
                rbfe = true;
            }
            if(data[(i*width)+(ij)][1] != ch1_rowBuffers[kernelSize][ij]) {
                rowBufferErrors = rowBufferErrors + 1;
                rbfe = true;
            }
            if(data[(i*width)+(ij)][2] != ch2_rowBuffers[kernelSize][ij]) {
                rowBufferErrors = rowBufferErrors + 1;
                rbfe = true;
            }
        }

        for(int j = 0; j < width; j++)
        {
            // Generate window
            ch0_window = getRectWindow(j, width, height, kernelSize, ch0_rowBuffers);
            ch1_window = getRectWindow(j, width, height, kernelSize, ch1_rowBuffers);
            ch2_window = getRectWindow(j, width, height, kernelSize, ch2_rowBuffers);

            // Data mismatch
            if(data[(i*width)+(j)][0] != ch0_window[windowCenter]
                || data[(i*width)+(j)][1] != ch1_window[windowCenter]
                || data[(i*width)+(j)][2] != ch2_window[windowCenter]
                ) {
                windowErrors = windowErrors + 1;
                odata[(i*width)+(j)][0] = 255;
                odata[(i*width)+(j)][1] = 0;
                odata[(i*width)+(j)][2] = 0;

            } else if(rbfe) { // Buffer row error as blue
                odata[(i*width)+(j)][0] = 0;
                odata[(i*width)+(j)][1] = 0;
                odata[(i*width)+(j)][2] = 255;

            } else { // OK
                odata[(i*width)+(j)][0] = ch0_window[windowCenter];
                odata[(i*width)+(j)][1] = ch1_window[windowCenter];
                odata[(i*width)+(j)][2] = ch2_window[windowCenter];
            }
        }
    }

    cout << "      Row buffer Errors found: " << rowBufferErrors << endl;

    cout << "   Kernel buffer Errors found: " << windowErrors << endl;
    return odata;
}

// TODO
vector<vector<unsigned int>> ccl(unsigned int channel, unsigned int threshold, unsigned int width, unsigned int height, vector<vector<unsigned int>> data)
{
    vector<uint8_t> ch_data(width*height,0);
    for(int i = 0; i < data.size(); i++)
    {
        ch_data[i] = data[i][channel] >= threshold ? 0xf : 0x0; // Operate on 1 channel & apply threshold;
    }

    vector<vector<unsigned int>> odata(data.size(),{0,0,0});

    vector<vector<unsigned int>> rowBuffers;
    vector<unsigned int> window;

    vector<unsigned int> labels(255,0);
    vector<unsigned int> features(255,0);

    int windowCenter = 4;
    int windowLeft = 3;
    int windowBottom = 7;
    for(int i = 0; i < height; i++)
    {
        rowBuffers = getRowBuffers(i, width, height, 1, ch_data);

        for(int j = 0; j < width; j++)
        {
            window = getRectWindow(j, width, height, 1, rowBuffers);

            if(window[windowBottom] > 0x0) {

            } else if(window[windowLeft] > 0x0) {

            } else {

            }

            // odata[(i*width)+(j)][0] = avg;
            // odata[(i*width)+(j)][1] = avg;
            // odata[(i*width)+(j)][2] = avg;
        }
    }

    return odata;
}