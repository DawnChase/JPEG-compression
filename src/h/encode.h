// compress BMP file

#ifndef ENCODE_H
#define ENCODE_H

#include "head.h"
#include "huffman.h"
#define N 8
#define Pi 3.14159265358979323846

// Compress BMP file by JPEG
void Compress(unsigned char *Info, string FileName, BMPInfoHeader *InfoHeader);

#endif