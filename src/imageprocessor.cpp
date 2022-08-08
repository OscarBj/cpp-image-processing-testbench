#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <numeric>

#include "imageprocessor.h"

using namespace std;

// Select channel, pixel value to transform, value to transform into, data
void transformPxChVal(unsigned int channel, unsigned int from, unsigned int to, vector<vector<unsigned int>>& data)
{
    for(int i = 0; i < data.size(); i++)
    {
        if(data[i][channel] == from)
        {
            data[i][channel] = to;
        }
    }
}

void getRowBuffers(unsigned int row_idx, unsigned int width, unsigned int height, unsigned int kernelSize,vector<vector<unsigned int>>& buffers, vector<unsigned int>& data)
{
    // Pass buffers with following geometry to this function:
    //
    //      kernelSize { [ <- width -> ]
    //

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
}

void getNextRowBuffer(unsigned int row_idx, unsigned int write_idx, unsigned int width, unsigned int height, unsigned int kernelSize,vector<vector<unsigned int>>& buffers, vector<unsigned int>& data)
{
    int ii = row_idx + kernelSize;

    for (int j = 0; j < width; j++) {

        if(row_idx+kernelSize > (height-1)) { // For rows above height
            buffers[write_idx][j] = 0;
        } else if(row_idx + j < kernelSize) { // For rows below height
            buffers[write_idx][j] = 0;
        } else {
            buffers[write_idx][j] = data[(ii*width) + j]; // Middle
        }
    }
}

void getRectWindow(unsigned int px_idx, unsigned int read_idx, unsigned int width, unsigned int height, unsigned int kernelSize, vector<unsigned int>& window, vector<vector<unsigned int>>& rowBuffers)
{

    // Create rectangular window with kernelSize as radius

    int jj,ii = read_idx;
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
                window[(i*((kernelSize*2)+1)) + j] = rowBuffers[ii][jj];
            }
        }

        ii = ii < (kernelSize*2) ? ii + 1 : 0;
    }
}

void boxFilt(unsigned int width, unsigned int height, unsigned int kernelSize, vector<vector<unsigned int>>& data)
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

    vector<vector<unsigned int>> ch0_rowBuffers((kernelSize*2)+1, vector<unsigned int>(width, 0));
    vector<vector<unsigned int>> ch1_rowBuffers((kernelSize*2)+1, vector<unsigned int>(width, 0));
    vector<vector<unsigned int>> ch2_rowBuffers((kernelSize*2)+1, vector<unsigned int>(width, 0));
    vector<unsigned int> ch0_window(((kernelSize*2)+1)*((kernelSize*2)+1), 0);
    vector<unsigned int> ch1_window(((kernelSize*2)+1)*((kernelSize*2)+1), 0);
    vector<unsigned int> ch2_window(((kernelSize*2)+1)*((kernelSize*2)+1), 0);

    int buf_start = 0;
    int buf_center = kernelSize;
    int buf_end = 0;

    int ch0_avg;
    int ch1_avg;
    int ch2_avg;
    for(int i = 0; i < height; i++)
    {
        if(i < (kernelSize*2)) {
        getRowBuffers(i, width, height, kernelSize, ch0_rowBuffers, ch0_data);
        getRowBuffers(i, width, height, kernelSize, ch1_rowBuffers, ch1_data);
        getRowBuffers(i, width, height, kernelSize, ch2_rowBuffers, ch2_data);
        } else {
            getNextRowBuffer(i, buf_start, width, height, kernelSize, ch0_rowBuffers, ch0_data);
            getNextRowBuffer(i, buf_start, width, height, kernelSize, ch1_rowBuffers, ch1_data);
            getNextRowBuffer(i, buf_start, width, height, kernelSize, ch2_rowBuffers, ch2_data);

            buf_start = buf_start == kernelSize*2 ? 0 : buf_start + 1;
            buf_center = buf_start + kernelSize > kernelSize*2 ? (buf_start + kernelSize) - ((kernelSize*2)+1) : buf_start + kernelSize;
            buf_end = buf_start == 0 ? kernelSize*2 : buf_start - 1;
        }

        for(int j = 0; j < width; j++)
        {
            getRectWindow(j, buf_start, width, height, kernelSize, ch0_window, ch0_rowBuffers);
            getRectWindow(j, buf_start, width, height, kernelSize, ch1_window, ch1_rowBuffers);
            getRectWindow(j, buf_start, width, height, kernelSize, ch2_window, ch2_rowBuffers);

            ch0_avg = (unsigned int) accumulate(ch0_window.begin(), ch0_window.end(), 0.0) / ch0_window.size();
            ch1_avg = (unsigned int) accumulate(ch1_window.begin(), ch1_window.end(), 0.0) / ch1_window.size();
            ch2_avg = (unsigned int) accumulate(ch2_window.begin(), ch2_window.end(), 0.0) / ch2_window.size();


            data[(i*width)+(j)][0] = ch0_avg;
            data[(i*width)+(j)][1] = ch1_avg;
            data[(i*width)+(j)][2] = ch2_avg;
        }
    }
}

vector<vector<unsigned int>> bufferTest(unsigned int width, unsigned int height, unsigned int kernelSize, vector<vector<unsigned int>>& data)
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

    vector<vector<unsigned int>> ch0_rowBuffers((kernelSize*2)+1, vector<unsigned int>(width, 0));
    vector<vector<unsigned int>> ch1_rowBuffers((kernelSize*2)+1, vector<unsigned int>(width, 0));
    vector<vector<unsigned int>> ch2_rowBuffers((kernelSize*2)+1, vector<unsigned int>(width, 0));
    vector<unsigned int> ch0_window(((kernelSize*2)+1)*((kernelSize*2)+1), 0);
    vector<unsigned int> ch1_window(((kernelSize*2)+1)*((kernelSize*2)+1), 0);
    vector<unsigned int> ch2_window(((kernelSize*2)+1)*((kernelSize*2)+1), 0);

    int windowErrors = 0;
    int rowBufferErrors = 0;

    int windowCenter = (((kernelSize*2)+1)*((kernelSize*2)+1)-1)/2;

    int buf_start = 0;
    int buf_center = kernelSize;
    int buf_end = 0;

    bool rbfe;
    for(int i = 0; i < height; i++)
    {
        if(i < (kernelSize*2)) {
        getRowBuffers(i, width, height, kernelSize, ch0_rowBuffers, ch0_data);
        getRowBuffers(i, width, height, kernelSize, ch1_rowBuffers, ch1_data);
        getRowBuffers(i, width, height, kernelSize, ch2_rowBuffers, ch2_data);
        } else {
            getNextRowBuffer(i, buf_start, width, height, kernelSize, ch0_rowBuffers, ch0_data);
            getNextRowBuffer(i, buf_start, width, height, kernelSize, ch1_rowBuffers, ch1_data);
            getNextRowBuffer(i, buf_start, width, height, kernelSize, ch2_rowBuffers, ch2_data);

            buf_start = buf_start == kernelSize*2 ? 0 : buf_start + 1;
            buf_center = buf_start + kernelSize > kernelSize*2 ? (buf_start + kernelSize) - ((kernelSize*2)+1) : buf_start + kernelSize;
            buf_end = buf_start == 0 ? kernelSize*2 : buf_start - 1;
        }
        rbfe = false;
        for (int ij = 0; ij < width; ij++) {

            if(data[(i*width)+(ij)][0] != ch0_rowBuffers[buf_center][ij]) {
                rowBufferErrors = rowBufferErrors + 1;
                rbfe = true;
            }
            if(data[(i*width)+(ij)][1] != ch1_rowBuffers[buf_center][ij]) {
                rowBufferErrors = rowBufferErrors + 1;
                rbfe = true;
            }
            if(data[(i*width)+(ij)][2] != ch2_rowBuffers[buf_center][ij]) {
                rowBufferErrors = rowBufferErrors + 1;
                rbfe = true;
            }
        }

        for(int j = 0; j < width; j++)
        {
            // Generate window
            getRectWindow(j, buf_start, width, height, kernelSize, ch0_window, ch0_rowBuffers);
            getRectWindow(j, buf_start, width, height, kernelSize, ch1_window, ch1_rowBuffers);
            getRectWindow(j, buf_start, width, height, kernelSize, ch2_window, ch2_rowBuffers);

            // Data mismatch
            if(data[(i*width)+(j)][0] != ch0_window[windowCenter] // CENTER PIXEL
                || data[(i*width)+(j)][1] != ch1_window[windowCenter]
                || data[(i*width)+(j)][2] != ch2_window[windowCenter]
                ) {
                windowErrors = windowErrors + 1;
                odata[(i*width)+(j)][0] = 255;
                odata[(i*width)+(j)][1] = 0;
                odata[(i*width)+(j)][2] = 0;
            } else if (j > 0)
                if (data[(i*width)+(j-1)][0] != ch0_window[windowCenter-1] // LEFT PIXEL
                || data[(i*width)+(j-1)][1] != ch1_window[windowCenter-1]
                || data[(i*width)+(j-1)][2] != ch2_window[windowCenter-1])
                {
                windowErrors = windowErrors + 1;
                odata[(i*width)+(j)][0] = 0;
                odata[(i*width)+(j)][1] = 255;
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

    cout << "      Row buffer Errors: " << rowBufferErrors << endl;

    cout << "   Kernel buffer Errors: " << windowErrors << endl;
    return odata;
}

void ccl(unsigned int channel, unsigned int threshold, unsigned int width, unsigned int height, vector<vector<unsigned int>>& data)
{
    vector<unsigned int> ch_data(width*height,0);
    for(int i = 0; i < data.size(); i++)
    {
        ch_data[i] = data[i][channel] >= threshold ? 0xff : 0x0; // Operate on 1 channel & apply threshold;
    }

    int kernelSize = 1;

    vector<vector<unsigned int>> rowBuffers((kernelSize*2)+1, vector<unsigned int>(width, 0));
    vector<unsigned int> window(((kernelSize*2)+1)*((kernelSize*2)+1), 0);

    vector<unsigned int> labels(255,0);
    vector<unsigned int> features(255,0);

    int windowCenter = 4;
    int windowLeft = 3;
    int windowRight = 5;
    int windowBottom = 1;
    int windowTop = 7;

    int buf_start = 0;
    int buf_center = kernelSize;
    int buf_end = 0;

    uint8_t label = 1;
    int lb = 0;
    uint8_t feature = 255;
    
    bool halt_process = false;

    for(int i = 0; i < height; i++)
    {

        if(i < (kernelSize*2)) {
            getRowBuffers(i, width, height, kernelSize, rowBuffers, ch_data);
        } else {
            getNextRowBuffer(i, buf_start, width, height, kernelSize, rowBuffers, ch_data);

            buf_start = buf_start == kernelSize*2 ? 0 : buf_start + 1;
            buf_center = buf_start + kernelSize > kernelSize*2 ? (buf_start + kernelSize) - ((kernelSize*2)+1) : buf_start + kernelSize;
            buf_end = buf_start == 0 ? kernelSize*2 : buf_start - 1;
        }
        for(int j = 0; j < width; j++)
        {
            getRectWindow(j, buf_start, width, height, kernelSize, window, rowBuffers);

            if(window[windowBottom] > 0x0 && window[windowCenter] > 0x0 && features[window[windowBottom]] != features[window[windowCenter]]) { // Assign to feature below

                ch_data[(i*width)+(j)] = window[windowBottom];
                rowBuffers[buf_center][j] = window[windowBottom];

                if(window[windowLeft] > 0x0 && j>0 && features[window[windowLeft]] != features[window[windowBottom]]) { // Merge bottom & left features
                    features[window[windowLeft]] = features[window[windowBottom]];
                    ch_data[(i*width)+(j-1)] = window[windowBottom];
                    rowBuffers[buf_center][j-1] = window[windowBottom];

                    lb--; // -1 Feature
                }
            } else if(window[windowLeft] > 0x0 && window[windowCenter] > 0x0 && features[window[windowLeft]] != features[window[windowCenter]]) { // Assign to left feature
                ch_data[(i*width)+(j)] = window[windowLeft];
                rowBuffers[buf_center][j] = window[windowLeft];

            } else if(window[windowCenter] > 0x0) { // New feature
                lb++; // +1 Feature
                ch_data[(i*width)+(j)] = (unsigned int) label;
                rowBuffers[buf_center][j] = (unsigned int) label;
                features[label] = (unsigned int) feature;
                label++;
                feature--;
            }

            // Handle feature table & label limitations
            if(label == 255) {
                halt_process = true;
                break;
            }
            if(feature == 0) {
                halt_process = true;
                break;
            }
        }
        if(halt_process) {
            cout << "   CCL stopped because of feature/ label overflow" << endl;
            break;
        }
    }

    buf_start = 0;
    buf_center = kernelSize;
    buf_end = 0;

    for(int i = 0; i < height; i++)
    {
        if(i < (kernelSize*2)) {
            getRowBuffers(i, width, height, 1, rowBuffers, ch_data);
        } else {
            getNextRowBuffer(i, buf_start, width, height, kernelSize, rowBuffers, ch_data);

            buf_start = buf_start == kernelSize*2 ? 0 : buf_start + 1;
            buf_center = buf_start + kernelSize > kernelSize*2 ? (buf_start + kernelSize) - ((kernelSize*2)+1) : buf_start + kernelSize;
            buf_end = buf_start == 0 ? kernelSize*2 : buf_start - 1;
        }
        for(int j = 0; j < width; j++)
        {
            getRectWindow(j, buf_start, width, height, 1, window, rowBuffers);
            if(features[window[windowCenter]] > 0) {
                data[(i*width)+(j)][0] = (unsigned int)features[window[windowCenter]];
                data[(i*width)+(j)][1] = 0;
                data[(i*width)+(j)][2] = 0;

            }
            else {
                data[(i*width)+(j)][0] = ch_data[(i*width)+(j)];
                data[(i*width)+(j)][1] = ch_data[(i*width)+(j)];
                data[(i*width)+(j)][2] = ch_data[(i*width)+(j)];
            }
        }
    }
    cout << endl << "   " << (int) lb << " features found" << endl;
}