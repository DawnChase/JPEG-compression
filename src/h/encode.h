// compress BMP file

#ifndef ENCODE_H
#define ENCODE_H

#include "head.h"
#include "huffman.h"
#define N 8
#define Pi 3.14159265358979323846

void Compress(unsigned char *Info, BMPInfoHeader *InfoHeader);

#endif