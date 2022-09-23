#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <numeric>
#include <math.h>
#include <functional>

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

// void traverseImage(unsigned int width, unsigned int height, unsigned int kernelSize, vector<vector<unsigned int>>& data, void(*kernelTask)(unsigned int, unsigned int, vector<vector<unsigned int>>&, vector<vector<unsigned int>>&))
void traverseImage(unsigned int width, unsigned int height, unsigned int kernelSize, vector<vector<unsigned int>>& data, function< void (unsigned int, unsigned int, vector<vector<unsigned int>>&, vector<vector<unsigned int>>&)> kernelTask)
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
    
    vector<vector<unsigned int>> kernelWindow(3, vector<unsigned int>(((kernelSize*2)+1)*((kernelSize*2)+1), 0));

    int buf_start = 0;
    int buf_center = kernelSize;
    int buf_end = 0;

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
            getRectWindow(j, buf_start, width, height, kernelSize, kernelWindow[0], ch0_rowBuffers);
            getRectWindow(j, buf_start, width, height, kernelSize, kernelWindow[1], ch1_rowBuffers);
            getRectWindow(j, buf_start, width, height, kernelSize, kernelWindow[2], ch2_rowBuffers);

            kernelTask((i*width)+(j), kernelSize, kernelWindow, data);
        }
    }
}

void boxFilt(unsigned int width, unsigned int height, unsigned int kernelSize, vector<vector<unsigned int>>& data)
{
    auto compute3chAvg = [](unsigned int xy, unsigned int kernelSize, vector<vector<unsigned int>>& kernelWindow, vector<vector<unsigned int>>& data) {
        unsigned int ch0_avg = 0;
        unsigned int ch1_avg = 0;
        unsigned int ch2_avg = 0;

        for(int i = 0; i < (kernelSize*2)+1; i++)
        {
            for(int j = 0; j < (kernelSize*2)+1; j++)
            {

                ch0_avg += kernelWindow[0][(i*((kernelSize*2)+1)) + j];
                ch1_avg += kernelWindow[1][(i*((kernelSize*2)+1)) + j];
                ch2_avg += kernelWindow[2][(i*((kernelSize*2)+1)) + j];

            }
        }

        data[xy][0] = ch0_avg/kernelWindow[0].size();
        data[xy][1] = ch1_avg/kernelWindow[1].size();
        data[xy][2] = ch2_avg/kernelWindow[2].size();
    };
    traverseImage(width, height, kernelSize, data, compute3chAvg);
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

void ccl(unsigned int channel, unsigned int threshold, unsigned int width, unsigned int height, vector<vector<unsigned int>>& data, bool centroiding = false)
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
    vector<vector<int>> featureCoordinates(255,vector<int>(4, -1));
    // Coordinate indecies
    uint8_t xMin = 0;
    uint8_t yMin = 1;
    uint8_t xMax = 2;
    uint8_t yMax = 3;

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
    uint8_t feature = 254;
    
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

                if(centroiding) featureCoordinates[features[window[windowBottom]]][yMax] = i > featureCoordinates[features[window[windowBottom]]][yMax] ? i : featureCoordinates[features[window[windowBottom]]][yMax];

                if(window[windowLeft] > 0x0 && j>0 && features[window[windowLeft]] != features[window[windowBottom]]) { // Merge bottom & left features

                    if(centroiding) {
                        featureCoordinates[features[window[windowBottom]]][xMin] = featureCoordinates[features[window[windowBottom]]][xMin] > featureCoordinates[features[window[windowLeft]]][xMin] ? featureCoordinates[features[window[windowLeft]]][xMin] : featureCoordinates[features[window[windowBottom]]][xMin];
                        featureCoordinates[features[window[windowBottom]]][yMin] = featureCoordinates[features[window[windowBottom]]][yMin] > featureCoordinates[features[window[windowLeft]]][yMin] ? featureCoordinates[features[window[windowLeft]]][yMin] : featureCoordinates[features[window[windowBottom]]][yMin];
                        featureCoordinates[features[window[windowBottom]]][xMax] = featureCoordinates[features[window[windowBottom]]][xMax] < featureCoordinates[features[window[windowLeft]]][xMax] ? featureCoordinates[features[window[windowLeft]]][xMax] : featureCoordinates[features[window[windowBottom]]][xMax];
                        featureCoordinates[features[window[windowBottom]]][yMax] = featureCoordinates[features[window[windowBottom]]][yMax] < featureCoordinates[features[window[windowLeft]]][yMax] ? featureCoordinates[features[window[windowLeft]]][yMax] : featureCoordinates[features[window[windowBottom]]][yMax];
                    }

                    features[window[windowLeft]] = features[window[windowBottom]];
                    ch_data[(i*width)+(j-1)] = window[windowBottom];
                    rowBuffers[buf_center][j-1] = window[windowBottom];

                    lb--; // -1 Feature
                }
            } else if(window[windowLeft] > 0x0 && window[windowCenter] > 0x0 && features[window[windowLeft]] != features[window[windowCenter]]) { // Assign to left feature
                ch_data[(i*width)+(j)] = window[windowLeft];
                rowBuffers[buf_center][j] = window[windowLeft];

                if(centroiding) featureCoordinates[features[window[windowLeft]]][xMax] = j > featureCoordinates[features[window[windowLeft]]][xMax] ? j : featureCoordinates[features[window[windowLeft]]][xMax];

            } else if(window[windowCenter] > 0x0) { // New feature
                lb++; // +1 Feature
                ch_data[(i*width)+(j)] = (unsigned int) label;
                rowBuffers[buf_center][j] = (unsigned int) label;
                features[label] = (unsigned int) feature;

                if(centroiding) featureCoordinates[features[label]] = {j,i,j,i}; // x1, y1, x2, y2

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

    if(centroiding) {
        int i,x,y,x1,y1,x2,y2,cx,cy;

        for(i = 0; i < 255; i++) {
            if(featureCoordinates[features[i]][xMin] >= 0) {
                x1 = featureCoordinates[features[i]][xMin] - 2 >= 0 ? featureCoordinates[features[i]][xMin] - 2 : 0;
                y1 = featureCoordinates[features[i]][yMin] - 2 >= 0 ? featureCoordinates[features[i]][yMin] - 2 : 0;
                x2 = featureCoordinates[features[i]][xMax] + 2 <= width ? featureCoordinates[features[i]][xMax] + 2 : 0;
                y2 = featureCoordinates[features[i]][yMax] + 2 <= height ? featureCoordinates[features[i]][yMax] + 2 : 0;

                cx = (x2-x1)/2;
                cy = (y2-y1)/2;

                vector<unsigned int> feature_columns(vector<unsigned int>((x2-x1),1));
                vector<unsigned int> feature_rows(vector<unsigned int>((y2-y1),1));

                
                for(y = y1; y <= y2; y++) {
                    for(x = x1; x <= x2; x++) {
                        feature_columns[x] = (feature_columns[x] + 0xffff) - (ch_data[(y*width)+(x)]*abs(x-cx));
                        feature_rows[y] = (feature_rows[y] + 0xffff) - (ch_data[(y*width)+(x)]*abs(y-cy));
                    }
                }
                cx = 0;
                cy = 0;
                for(y = y1; y <= y2; y++) {
                    if(feature_rows[y] > feature_rows[cy]) cy = y;
                    for(x = x1; x <= x2; x++) {
                        if(feature_columns[x] > feature_columns[cx]) cx = x;
                    }
                }
                for(y = y1; y <= y2; y++) {
                    for(x = x1; x <= x2; x++) {
                        if(y == y1) { //Bottom
                            data[(y1*width)+x][0] = 255;
                            data[(y1*width)+x][1] = 255;
                            data[(y1*width)+x][2] = 255;
                        } else if(y == y2) { // Top
                            data[(y2*width)+x][0] = 255;
                            data[(y2*width)+x][1] = 255;
                            data[(y2*width)+x][2] = 255;
                        } else if (x == x1) { // Left
                            data[(y*width)+x1][0] = 255;
                            data[(y*width)+x1][1] = 255;
                            data[(y*width)+x1][2] = 255;
                        } else if (x == x2) { // Right
                            data[(y*width)+x2][0] = 255;
                            data[(y*width)+x2][1] = 255;
                            data[(y*width)+x2][2] = 255;
                        } else if(features[ch_data[(y*width)+(x)]] == features[i]) { // Center
                            // data[(y*width)+x][0] = 255;
                            // data[(y*width)+x][1] = 0;
                            // data[(y*width)+x][2] = 0;

                        } else if(y == cy && x == cx) {
                            data[(y*width)+x][0] = 255;
                            data[(y*width)+x][1] = 255;
                            data[(y*width)+x][2] = 0;
                        }
                    }
                }
            }
        }
    } else {
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
    }
    cout << endl << "   " << (int) lb << " features found" << endl;
}

void gradientFilter(unsigned int channel, unsigned int threshold, unsigned int width, unsigned int height, vector<vector<unsigned int>>& data)
{
    auto applyGradient = [&](unsigned int xy, unsigned int kernelSize, vector<vector<unsigned int>>& kernelWindow, vector<vector<unsigned int>>& data) {

        int magX = 0;
        int magY = 0;

        int Gx[] = {-1,0,1, -2,0,2, -1,0,1};
        int Gy[] = {1,2,1, 0,0,0, -1-2-1};

        for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                magX += kernelWindow[channel][(i*3) + j]*Gx[(i*3)+j];
                magY += kernelWindow[channel][(i*3) + j]*Gy[(i*3)+j];
            }
        }

        int mag = (int)sqrt((magX * magX) + (magY * magY));
        mag = abs(mag);

        data[xy][0] = mag > threshold ? mag : 0;
        data[xy][1] = mag > threshold ? mag : 0;
        data[xy][2] = mag > threshold ? mag : 0;
    };
    traverseImage(width, height, 1, data, applyGradient);
}