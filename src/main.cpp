#include <vector>

#include "filehandler.h"
#include "imageprocessor.h"

using namespace std;

int main() {
    
    char *in = "inputs/test_in.bmp";
    char *out = "outputs/out_put.bmp";

    BMPFilehandler filehandler(in, out);

    vector<vector<unsigned int>> dout = filehandler.ReadBMP();
    unsigned char *header = filehandler.ReadHeader();

    // dout = transformPxChVal(1,255,0,dout); // Set channel 1 (green) to 0
    // dout = transformPxChVal(2,255,0,dout); // Set channel 2 (blue) to 0
    
    dout = avgFilt(0,100,100,1,dout);

    filehandler.WriteBMP(dout);

    return 0;
}