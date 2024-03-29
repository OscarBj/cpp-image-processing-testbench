## Image processing testbench

## Overview

- Testbench for prototyping image processing algorithms in c++
- A collection of reusable image data structures & test methods accelerates development
- Uses 24-bit BMP files as input/output
- The image container consists of 3 vectors, each containg the image data for thier respective channel.
- Images are traversed as a coordinate system, origo being in the left bottom corner

Algorithms:
- Linear box filter with variable kernel size
- Connected component labeling (CCL)
- Channel/pixel transform
- Weighted centroiding

Buffers & Tests:
- Row buffer
- Kernel window buffer
---
## Getting started

Compiler used: g++ (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0

Compile using:
> g++ -o main.o src/*.cpp (-std=c++11)

Run using:
> ./main.o

Parameters:

**-t** : Run testbench in test mode.

    Tests included: Row buffer, kernel window buffer.
    Input image is compared against output from buffers. Detected errors are rendered on output image.

**-foreground_th** : Foreground threshold value.

    Threshold value for separating foreground from background.
    Default = 128
    Applies to ccl

**-feature_th** : Feature threshold value.

    Threshold value for passing feature brightness threshold
    Default = 128
    Applies to centroiding

**-c** : Set channel.

    Selected image channel is used for processing.
    0 = Red (default)
    1 = Green
    2 = Blue
    Applies to ccl

**-k** : Set kernel size (in pixels).

    Kernel size specifies the geometry of the buffers: number of rows in the row buffer and dimensions of the kernel window.
    Default kernel size = 1 (=3x3 pixel kernel)
    Applies to box filter

**-f** : Input file

    Location of input file. Default = test_in.bmp

**-vd** : Generate vessel data

    Creates a random vessel shape in a bmp file

## CCL Operation Mode
### features[label] -> feature_id
- label = 1 -> 255
- feature_id = 255 -> 1
### featureCoordinates[feature_id] -> x1,y1,x2,y2