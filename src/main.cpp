#include <vector>
#include <iostream>
#include <chrono>
#include <string>

#include "filehandler.h"
#include "imageprocessor.h"
#include "datagenerator.h"

using namespace std;

int main(int argc, char** argv)
{
    // ARGS
    bool testmode = false;
    unsigned int kernelSize = 1;
    unsigned int channel = 0;
    unsigned int fg_th = 128;
    unsigned int feature_th = 128;
    bool vessel_data = false;
    bool guilog = false;
    bool batchlog = false;

    feature_descriptors descriptors;

    char *in = (char*) "inputs/test_in.bmp";
    char *out = (char*) "outputs/out_put.bmp";
    if(argc > 1) {
        for(int i = 1; i < argc; i++) {
            if(string(argv[i]) == "-t") testmode = true;
            if(string(argv[i]) == "-g") guilog = true;
            if(string(argv[i]) == "-b") batchlog = true;
            if(string(argv[i]) == "-vd") vessel_data = true;
            if(string(argv[i]) == "-k" && argc > i+1) {
                kernelSize = atoi(argv[i+1]) > 0 ? atoi(argv[i+1]) : 1;
            }
            if(string(argv[i]) == "-c" && argc > i+1) {
                channel = atoi(argv[i+1]) >= 0 && atoi(argv[i+1]) < 3 ? atoi(argv[i+1]) : 0;
            }
            if(string(argv[i]) == "-f" && argc > i+1) {
                in = argv[i+1];
            }
            if(string(argv[i]) == "-foreground_th" && argc > i+1) {
                fg_th = atoi(argv[i+1]) >= 0 && atoi(argv[i+1]) < 255 ? atoi(argv[i+1]) : 128;
            }
            if(string(argv[i]) == "-feature_th" && argc > i+1) {
                feature_th = atoi(argv[i+1]);
            }
        }
    }
    if(vessel_data) {
        generateVesselData(1,100,100);
    } else {
        chrono::steady_clock::time_point start = chrono::steady_clock::now();

        BMPFilehandler filehandler(in, out);

        if(guilog) {
            cout << endl;
            cout << " ---- TESTBENCH CONFIG --" << endl << endl;
            cout << "    Test mode: " << (testmode == 0 ? "false" : "true") << endl;
            cout << "  Kernel Size: " << kernelSize << endl;
            cout << "      Channel: " << channel << endl;
            cout << " Foreground threshold: " << fg_th << endl;
            cout << "    Feature threshold: " << feature_th << endl << endl;
            cout << " ---- INPUT INFO --------" << endl;
            cout << endl;
        }

        vector<vector<unsigned int>> dout = filehandler.ReadBMP();
        unsigned char *header = filehandler.ReadHeader();
        int width = *(int*)&header[18];
        int height = abs(*(int*)&header[22]);

        // Transform
        // transformPxChVal(1,255,0,dout); // Set channel 1 (green) to 0
        // transformPxChVal(2,255,0,dout); // Set channel 2 (blue) to 0

        if(testmode) {
            cout << " ---- TESTS --------------" << endl;
            cout << endl;
            dout = bufferTest(width,height,kernelSize,dout);
        } else {
            if(guilog) {
                cout << endl;
                cout << " ---- RUN ----------------" << endl;
                cout << endl;
                cout << "   running box filter...";
                cout << endl;
            }

            chrono::steady_clock::time_point box_begin = chrono::steady_clock::now();

            boxFilt(width,height,kernelSize,dout);

            chrono::steady_clock::time_point box_end = chrono::steady_clock::now();
            if(guilog) {
                cout << endl;
                cout << "   box filter finished in: " << chrono::duration_cast<std::chrono::milliseconds>(box_end - box_begin).count() << "[ms]" << endl;
                cout << endl;
                cout << "   running connected component labeling...";
                cout << endl;
            }
            chrono::steady_clock::time_point ccl_begin = chrono::steady_clock::now();

            descriptors = ccl(channel, fg_th, width, height, dout, false);

            chrono::steady_clock::time_point ccl_end = chrono::steady_clock::now();

            if(guilog) {
                cout << endl;
                cout << "   ccl finished in: " << chrono::duration_cast<std::chrono::milliseconds>(ccl_end - ccl_begin).count() << "[ms]" << endl;
                cout << endl;
            }
            chrono::steady_clock::time_point centroid_begin = chrono::steady_clock::now();

            centroid(channel, feature_th, width, height, descriptors, dout);

            chrono::steady_clock::time_point centroid_end = chrono::steady_clock::now();

            if(guilog) {
                cout << endl;
                cout << "   centroid finished in: " << chrono::duration_cast<std::chrono::microseconds>(centroid_end - centroid_begin).count() << "[us]" << endl;
                cout << endl;
            }
            if(batchlog) {
                cout << fg_th << ";";
                cout << feature_th << ";";
                cout << chrono::duration_cast<std::chrono::milliseconds>(box_end - box_begin).count() << ";";
                cout << chrono::duration_cast<std::chrono::milliseconds>(ccl_end - ccl_begin).count() << ";";
                cout << chrono::duration_cast<std::chrono::microseconds>(centroid_end - centroid_begin).count() << endl;
            }
            // gradientFilter(channel, fg_th, width,height,dout);


        }
        if(guilog) {
            cout << endl;
            cout << " ---- OUTPUT INFO -------" << endl;
            cout << endl;
        }

        filehandler.WriteBMP(width, height, dout);

        if(guilog) {
            cout << endl;
            cout << " -----------------------" << endl;
            cout << endl;
        }
        chrono::steady_clock::time_point finish = chrono::steady_clock::now();

        if(guilog) {
            cout << "   Total: " << chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << "[ms]" << endl;
            cout << endl;
        }
    }

    return 0;
}