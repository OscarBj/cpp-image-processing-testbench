## Image processing testbench

## Overview

- Testbench for prototyping image processing algorithms in c++
- A collection of reusable image data structures & test methods accelerates development
- Uses 24-bit BMP files as input/output

Algorithms:
- Linear box filter with variable kernel size
- Connected component labeling (CCL)
- Channel/pixel transform

Buffers & Tests:
- Row buffer
- Kernel window buffer
---
## Getting started

Compiler used: g++ (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0

Compile using:
> g++ -o main.o src/*.cpp

Run using:
> ./main.o

Parameters:

**-t** : Run testbench in test mode.

    Tests included: Row buffer, kernel window buffer.
    Input image is compared against output from buffers. Detected errors are rendered on output image.

**-th** : Threshold value.

    Threshold value for separating foreground from background.
    Default = 128
    Applies to ccl

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