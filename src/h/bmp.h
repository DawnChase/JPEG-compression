// read and write a bmp

#ifndef BMP_H
#define BMP_H

#include "head.h"

// input BMP file
unsigned char *ReadBMP(string FileName, BMPInfoHeader *InfoHeader);

// set BMP file header
BMPInfoHeader *setInfoHeader(int height, int width);

// print BMP file
void PrintBMP(string FileName, unsigned char *Info, BMPInfoHeader *InfoHeader);

#endif