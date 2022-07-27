## Image processing testbench

Compiler used: g++ (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0

Compile using: 
> g++ -o main.o src/*

Run using:
> ./main.o

## Overview

- Testbench for prototyping image processing algorithms in c++
- A collection of reusable image data structures & test methods accelerates development
- Uses 24-bit BMP files as input/output

### Components

Algorithms: 
- Linear box filter with variable kernel size
- Channel/pixel transform

Buffers & Tests:
- Row buffer
- Kernel window buffer
