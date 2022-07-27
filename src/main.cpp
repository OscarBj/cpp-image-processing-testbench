#include <vector>
#include <iostream>
#include <chrono>

#include "filehandler.h"
#include "imageprocessor.h"

using namespace std;

int main(int argc, char** argv)
{
    chrono::steady_clock::time_point start = chrono::steady_clock::now();

    char *in = (char*) "inputs/test_in.bmp";
    char *out = (char*) "outputs/out_put.bmp";

    BMPFilehandler filehandler(in, out);

    cout << endl;
    cout << " ---- INPUT INFO --------" << endl;
    cout << endl;

    vector<vector<unsigned int>> dout = filehandler.ReadBMP();
    unsigned char *header = filehandler.ReadHeader();

    // dout = transformPxChVal(1,255,0,dout); // Set channel 1 (green) to 0
    // dout = transformPxChVal(2,255,0,dout); // Set channel 2 (blue) to 0

    cout << " ---- TESTS --------------" << endl;
    cout << endl;
    
    dout = bufferTest(0,100,100,2,dout);

    cout << endl;   
    cout << " ---- RUN ----------------" << endl;
    cout << endl;
    cout << "   running box filter...";
    cout << endl;

    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    
    dout = boxFilt(0,100,100,1,dout);
    
    chrono::steady_clock::time_point end = chrono::steady_clock::now();

    cout << endl;
    cout << "   Finished in: " << chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << endl;
    cout << endl;

    cout << endl;
    cout << " ---- OUTPUT INFO -------" << endl;
    cout << endl;

    filehandler.WriteBMP(dout);

    cout << endl;
    cout << " -----------------------" << endl;
    cout << endl;

    chrono::steady_clock::time_point finish = chrono::steady_clock::now();
    cout << "   Total: " << chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << "[ms]" << endl;
    cout << endl;

    return 0;
}