#include <vector>
#include <iostream>
#include <chrono>
#include <string>

#include "filehandler.h"
#include "imageprocessor.h"

using namespace std;

int main(int argc, char** argv)
{
    // ARGS
    bool testmode = false;
    unsigned int kernelSize = 1;
    unsigned int channel = 0;
    unsigned int threshold = 128;

    char *in = (char*) "inputs/test_in.bmp";
    char *out = (char*) "outputs/out_put.bmp";
    if(argc > 1) {
        for(int i = 1; i < argc; i++) {
            if(string(argv[i]) == "-t") testmode = true;
            if(string(argv[i]) == "-k" && argc > i+1) {
                kernelSize = atoi(argv[i+1]) > 0 ? atoi(argv[i+1]) : 1;
            }
            if(string(argv[i]) == "-c" && argc > i+1) {
                channel = atoi(argv[i+1]) >= 0 && atoi(argv[i+1]) < 3 ? atoi(argv[i+1]) : 0;
            }
            if(string(argv[i]) == "-f" && argc > i+1) {
                in = argv[i+1];
            }
            if(string(argv[i]) == "-th" && argc > i+1) {
                threshold = atoi(argv[i+1]) >= 0 && atoi(argv[i+1]) < 255 ? atoi(argv[i+1]) : 128;
            }
        }
    }

    chrono::steady_clock::time_point start = chrono::steady_clock::now();

    BMPFilehandler filehandler(in, out);

    cout << endl;
    cout << " ---- TESTBENCH CONFIG --" << endl << endl;
    cout << "    Test mode: " << (testmode == 0 ? "false" : "true") << endl;
    cout << "  Kernel Size: " << kernelSize << endl;
    cout << "      Channel: " << channel << endl;
    cout << "    Threshold: " << threshold << endl << endl;
    cout << " ---- INPUT INFO --------" << endl;
    cout << endl;

    vector<vector<unsigned int>> dout = filehandler.ReadBMP();
    unsigned char *header = filehandler.ReadHeader();
    int width = *(int*)&header[18];
    int height = *(int*)&header[22];

    // Transform
    // transformPxChVal(1,255,0,dout); // Set channel 1 (green) to 0
    // transformPxChVal(2,255,0,dout); // Set channel 2 (blue) to 0

    if(testmode) {
        cout << " ---- TESTS --------------" << endl;
        cout << endl;
        dout = bufferTest(width,height,kernelSize,dout);
    } else {
        cout << endl;
        cout << " ---- RUN ----------------" << endl;
        cout << endl;
        cout << "   running box filter...";
        cout << endl;

        chrono::steady_clock::time_point box_begin = chrono::steady_clock::now();

        boxFilt(width,height,kernelSize,dout);

        chrono::steady_clock::time_point box_end = chrono::steady_clock::now();

        cout << endl;
        cout << "   box filter finished in: " << chrono::duration_cast<std::chrono::milliseconds>(box_end - box_begin).count() << "[ms]" << endl;
        cout << endl;
        cout << "   running connected component labeling...";
        cout << endl;

        chrono::steady_clock::time_point ccl_begin = chrono::steady_clock::now();

        ccl(channel,5,width,height,dout);

        chrono::steady_clock::time_point ccl_end = chrono::steady_clock::now();

        cout << endl;
        cout << "   ccl finished in: " << chrono::duration_cast<std::chrono::milliseconds>(ccl_end - ccl_begin).count() << "[ms]" << endl;
        cout << endl;
    }

    cout << endl;
    cout << " ---- OUTPUT INFO -------" << endl;
    cout << endl;

    filehandler.WriteBMP(width, height, dout);

    cout << endl;
    cout << " -----------------------" << endl;
    cout << endl;

    chrono::steady_clock::time_point finish = chrono::steady_clock::now();
    cout << "   Total: " << chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << "[ms]" << endl;
    cout << endl;

    return 0;
}